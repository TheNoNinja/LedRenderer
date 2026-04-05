#ifdef ARDUINO
#include <Arduino.h>
#elif EMULATOR
#include <emulator.h>
#endif

#include <led_engine.h>
#include "effects/effects.h"
#include <cmath>

static constexpr int   COLS         = 16;
static constexpr int   ROWS         = 8;
static constexpr int   PANELS       = 4;
static constexpr size_t PIXEL_COUNT  = COLS * ROWS * PANELS;

static Pixel  gPixels[PIXEL_COUNT];
static Color  gColors[PIXEL_COUNT];

static ColorWaveEffect gColorWaveEffect;
static GlareEffect  gGlareEffect;
static Renderer<PIXEL_COUNT> renderer(gPixels);
static float lastTime = 0.0f;

void setup() {
    auto pixelMapper = PixelMapper(gPixels, PIXEL_COUNT);

    pixelMapper.mapMatrix(Point( COLS - 1.0f, ROWS -1.0f, 0), COLS, ROWS, Point::Left(), Point::Down(), Point::Forward());
    pixelMapper.mapMatrix(Point(COLS + 3.0f, 0, 0), COLS, ROWS, Point::Right(), Point::Up(), Point::Forward());
    pixelMapper.setLedStrip(2);
    pixelMapper.mapMatrix(Point(-2.0f, ROWS -1.0f, -3.0f), COLS, ROWS, Point::Backward(), Point::Down(), Point::Left());
    pixelMapper.setLedStrip(3);
    pixelMapper.mapMatrix(Point(5.0f + COLS * 2, ROWS -1.0f, -3.0f), COLS, ROWS, Point::Backward(), Point::Down(), Point::Right());

    renderer.addEffect(&gColorWaveEffect);
    renderer.addEffect(&gGlareEffect);
    lastTime = millis();
}

void loop() {
    const float now = millis();
    const float dt  = now - lastTime;
    lastTime = now;

    renderer.render(dt, now);
    renderer.copyFrontBuffer(gColors);
}

#ifdef EMULATOR
int main() {
    setup();
    if (!Emulator::init(1280, 720, gPixels, PIXEL_COUNT)) return 1;

    while (!Emulator::ShouldQuit) {
        loop();
        Emulator::render(gColors);
    }

    Emulator::shutdown();
    return 0;
}
#endif
