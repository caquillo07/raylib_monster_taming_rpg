#include <stdio.h>
#include <assert.h>
#include <string.h>

#define CUTE_TILED_IMPLEMENTATION

#include "raylib.h"
#include "common.h"
#include "game.h"
#include "maps_manager.h"

static void init() {
    initLogger();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Monster Taming RPG");
    if (!IsWindowReady()) {
        panic("Window failed to initialize");
    }

    SetTargetFPS(120);
}

int main() {
    init();

    Game* game = game_new();
    panicIf(game == nil, "Failed to create game");

    while (!WindowShouldClose()) {
        game_handle_input(game);
        game_update(game);
        game_draw(game);
    }

    game_destroy(game);

    CloseWindow();
    return 0;
}

