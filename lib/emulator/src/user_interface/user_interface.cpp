#include "user_interface/user_interface.h"
#include "emulator.h"

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui_internal.h>

namespace UserInterface {
    static bool IsViewportFocused = false;

    bool init(SDL_Window* window, SDL_GLContext* glContext) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGui_ImplSDL3_InitForOpenGL(window, glContext);
        ImGui_ImplOpenGL3_Init();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(2, 2);
        style.FramePadding = ImVec2(2, 2);
        style.ItemSpacing = ImVec2(6, 2);
        style.ItemInnerSpacing = ImVec2(4, 4);
        style.IndentSpacing = 21;
        style.GrabMinSize = 12;

        style.WindowBorderSize = 1;
        style.ChildBorderSize = 1;
        style.PopupBorderSize = 1;
        style.FrameBorderSize = 0;

        style.WindowRounding = 0.0f;
        style.ChildRounding = 0.0f;
        style.FrameRounding = 0.0f;
        style.PopupRounding = 0.0f;
        style.GrabRounding = 0.0f;

        style.ScrollbarSize = 4;
        style.ScrollbarRounding = 0.0f;
        style.ScrollbarPadding = 1;

        style.TabBorderSize= 2;
        style.TabBarBorderSize = 1;
        style.TabBarOverlineSize = 1;
        style.TabMinWidthBase = 1;
        style.TabMinWidthShrink = 80;
        style.TabCloseButtonMinWidthSelected = -1;
        style.TabCloseButtonMinWidthUnselected = 0;
        style.TabRounding = 0.0f;

        style.CellPadding = ImVec2(2, 2);
        style.TableAngledHeadersAngle = 50;
        style.TableAngledHeadersTextAlign = ImVec2(0.5f, 0.0f);

        style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesFull;
        style.TreeLinesSize = 1;
        style.TreeLinesRounding = 0.0f;

        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style.WindowBorderHoverPadding = 4;
        style.WindowMenuButtonPosition = ImGuiDir_Left;

        style.ColorMarkerSize = 3;
        style.ColorButtonPosition = ImGuiDir_Right;
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
        style.SeparatorSize = 1;
        style.SeparatorTextBorderSize = 3;
        style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
        style.SeparatorTextPadding = ImVec2(20, 3);
        style.LogSliderDeadzone = 4;
        style.ImageRounding = 0;
        style.ImageBorderSize = 0;

        style.DockingNodeHasCloseButton = false;
        style.DockingSeparatorSize = 1;

        style.HoverFlagsForTooltipMouse = ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_Stationary;
        style.HoverFlagsForTooltipNav = ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay;

        style.DisplayWindowPadding = ImVec2(19, 19);
        style.DisplaySafeAreaPadding = ImVec2(3, 3);

        const auto colors = style.Colors;

        colors[ImGuiCol_Text]                   = ImVec4(0.88f, 0.89f, 0.90f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.45f, 0.47f, 0.50f, 1.00f);
        colors[ImGuiCol_TextLink]               = ImVec4(0.58f, 0.44f, 0.85f, 1.00f);

        colors[ImGuiCol_WindowBg]               = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.16f, 0.16f, 0.17f, 0.98f);

        colors[ImGuiCol_TitleBg]                = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);

        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);

        colors[ImGuiCol_Border]                 = ImVec4(0.20f, 0.20f, 0.22f, 0.65f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0,0,0,0);

        colors[ImGuiCol_FrameBg]                = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.22f, 0.22f, 0.24f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);

        colors[ImGuiCol_Button]                 = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.28f, 0.28f, 0.30f, 1.00f);

        colors[ImGuiCol_Header]                 = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);

        colors[ImGuiCol_Tab]                    = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_TabSelectedOverline]    = ImVec4(0.58f, 0.44f, 0.85f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.14f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);

        colors[ImGuiCol_CheckMark]              = ImVec4(0.58f, 0.44f, 0.85f, 1.00f);

        colors[ImGuiCol_SliderGrab]             = ImVec4(0.55f, 0.40f, 0.80f, 0.80f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.65f, 0.50f, 0.95f, 1.00f);

        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.58f, 0.44f, 0.85f, 0.60f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.58f, 0.44f, 0.85f, 1.00f);

        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.58f, 0.44f, 0.85f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.58f, 0.44f, 0.85f, 0.60f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.68f, 0.54f, 0.98f, 1.00f);

        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.58f, 0.44f, 0.85f, 0.35f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.58f, 0.44f, 0.85f, 0.65f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.58f, 0.44f, 0.85f, 1.00f);


        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.22f, 0.22f, 0.24f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.36f, 0.36f, 0.38f, 1.00f);
        return true;
    }

    bool onEvent(const SDL_Event &event) {
        const bool isHandled = ImGui_ImplSDL3_ProcessEvent(&event);
        if (IsViewportFocused) return false;
        return isHandled;
    }

    ImVec2 beginFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("##Dockhost", nullptr, flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockId = ImGui::GetID("MainDockspace");
        ImGui::DockSpace(dockId, ImVec2(0.0f, 0.0f), ImDrawFlags_None);

        static bool dockBuilt = false;
        if (!dockBuilt) {
            dockBuilt = true;
            ImGui::DockBuilderRemoveNode(dockId);
            ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockId, viewport->WorkSize);
            ImGuiID leftId;
            const ImGuiID rightId = ImGui::DockBuilderSplitNode(dockId, ImGuiDir_Right, 0.25f, nullptr, &leftId);

            if (ImGuiDockNode* leftNode = ImGui::DockBuilderGetNode(leftId)) {
                leftNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoUndocking;
            }
            ImGui::DockBuilderDockWindow("Viewport", leftId);
            ImGui::DockBuilderDockWindow("Inspector", rightId);
            ImGui::DockBuilderFinish(dockId);
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        constexpr ImGuiWindowFlags viewportFlags = ImGuiWindowFlags_NoTitleBar
                                                 | ImGuiWindowFlags_NoCollapse
                                                 | ImGuiWindowFlags_NoMove
                                                 | ImGuiWindowFlags_NoResize
                                                 | ImGuiWindowFlags_NoBringToFrontOnFocus
                                                 | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin("Viewport", nullptr, viewportFlags);
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImGui::End();
        ImGui::PopStyleVar();
        return viewportSize;
    }

    void endFrame(const GLuint viewportTextureId) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        constexpr ImGuiWindowFlags viewportFlags = ImGuiWindowFlags_NoTitleBar
                                                 | ImGuiWindowFlags_NoCollapse
                                                 | ImGuiWindowFlags_NoMove
                                                 | ImGuiWindowFlags_NoResize
                                                 | ImGuiWindowFlags_NoBringToFrontOnFocus
                                                 | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin("Viewport", nullptr, viewportFlags);
        IsViewportFocused = ImGui::IsWindowFocused();
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        const int w = std::max(1, (int)viewportSize.x);
        const int h = std::max(1, (int)viewportSize.y);
        ImGui::Image((ImTextureID)(intptr_t)viewportTextureId, ImVec2((float)w, (float)h), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        ImGui::PopStyleVar();

        static bool show = false;
        if (show) ImGui::ShowDemoWindow(&show);

        ImGui::Begin("Inspector");
        ImGui::Text("Hello, world!");
        if (ImGui::Button("Show Demo Window")) show = true;
        ImGui::End();

        ImGui::Render();

        const ImGuiIO& io = ImGui::GetIO();
        glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
}
