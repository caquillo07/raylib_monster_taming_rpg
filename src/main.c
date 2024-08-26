
#include "raylib.h"
#include "common.h"
#include "game.h"
#include "memory/memory.h"

static void init() {
    initialize_memory();
    initLogger();
    InitWindow(ScreenWidth, ScreenHeight, "Monster Taming RPG");
    panicIf(!IsWindowReady(), "Window failed to initialize");

    SetWindowState(FLAG_WINDOW_RESIZABLE);

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

