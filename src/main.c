#include "raylib.h"
#include "common.h"
#include "game.h"
#include "maps_manager.h"

static void init() {
    initLogger();
    InitWindow(ScreenWidth, ScreenHeight, "Monster Taming RPG");
    panicIf(!IsWindowReady(), "Window failed to initialize");

    SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(120);
}

int main() {
    init();

    Game* game = game_new();
    panicIf(game == nil, "Failed to create game");

    while (!WindowShouldClose()) {
        f32 deltaTime = GetFrameTime();
        game_handle_input(game);
        game_update(game, deltaTime);
        game_draw(game);
    }

    game_destroy(game);

    CloseWindow();
    return 0;
}
