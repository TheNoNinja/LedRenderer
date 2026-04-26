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
