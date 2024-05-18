//
// Created by Hector Mejia on 5/4/24.
//

#include <stdlib.h>
#include <assert.h>
#include "game.h"
#include "common.h"
#include "raylib.h"
#include "maps_manager.h"

static void init_game(Game *game) {
    maps_manager_init();
}

static void setup_game(Game *game, MapID mapID, Vector2 playerStartPosition) {
    Map *map = load_map(mapID);
    game->currentMap = map;
}

Game *game_new() {
    Game *game = calloc(1, sizeof(*game));
    panicIf(game == nil, "failed to allocate game");

    init_game(game);
    setup_game(game, MapIDWorld, (Vector2) {.x = 300, .y = 300});

    return game;
}

void game_destroy(Game *game) {
    map_free(game->currentMap);
    game->currentMap = nil;

    free(game);
    game = nil;
}

void game_handle_input(Game *game) {

}

void game_update(Game *game) {

}

void game_draw(Game *game) {
    BeginDrawing();
    {
//        ClearBackground(DARKGRAY);
//        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        map_draw(game->currentMap);
    }
    EndDrawing();
}

