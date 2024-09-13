
#include "raylib.h"
#include "common.h"
#include "game.h"
#include "memory/memory.h"

#define DUAL_SCREENS true

static void init() {
    srand(time(nil));

    initialize_memory();
    initLogger();
    InitWindow(ScreenWidth, ScreenHeight, "Monster Taming RPG");
    panicIf(!IsWindowReady(), "Window failed to initialize");

    SetWindowState(FLAG_WINDOW_RESIZABLE);
#ifdef DUAL_SCREENS
    SetWindowMonitor(1);
    SetWindowSize(GetMonitorWidth(1), GetMonitorHeight(1));
//    SetWindowState(FLAG_FULLSCREEN_MODE);
//    ToggleFullscreen();
#endif

    SetTargetFPS(120);
    SetTextLineSpacing(16);
}

int main() {
    init();
    game_init();

    while (!WindowShouldClose()) {
        const f32 deltaTime = GetFrameTime();
        game_handle_input();
        game_update(deltaTime);
        game_draw();
    }

    game_shutdown();

    CloseWindow();

    char *memUsage = get_memory_usage_str();
    slogi(memUsage);
    free(memUsage);
    shutdown_memory();

    return 0;
}

