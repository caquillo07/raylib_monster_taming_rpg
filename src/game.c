//
// Created by Hector Mejia on 5/4/24.
//

#include <stdlib.h>
#include <assert.h>
#include "game.h"
#include "common.h"
#include "raylib.h"
#include "maps_manager.h"

static void init_game(Game *game);
static void setup_game(Game *game, MapID mapID, Vector2 playerStartPosition);
static void game_draw_debug(Game *game);
static void update_camera(Game *game);

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

    player_free(game->player);
    game->player = nil;

    free(game);
}

void game_handle_input(Game *game) {
    if (IsKeyPressed(KEY_F2)) {
        game->debug = !game->debug;
        return;
    }
    player_input(game->player);
}

void game_update(Game *game, f32 deltaTime) {
    player_update(game->player, deltaTime);

    update_camera(game);
}

void game_draw(Game *game) {
    BeginDrawing();
    {
        ClearBackground(DARKGRAY);
        BeginMode2D(game->camera);
        {
            map_draw(game->currentMap);
            player_draw(game->player);
        }
        game_draw_debug(game);
        EndMode2D();
    }
    EndDrawing();
}

static void game_draw_debug(Game *game) {
    if (!game->debug) {
        return;
    }
    DrawFPS(10, 10);
}


static void init_game(Game *game) {
    maps_manager_init();
}

static void setup_game(Game *game, MapID mapID, Vector2 playerStartPosition) {
    Map *map = load_map(mapID);
    game->currentMap = map;

    game->player = player_new(map->playerStartingPosition);

    // camera
    game->camera = (Camera2D) {0};
    update_camera(game);
}

static void update_camera(Game *game) {
    game->camera.target = player_get_center(game->player);
    game->camera.offset = (Vector2) {
        .x = ((f32) GetScreenWidth() / 2.0f),
        .y = ((f32) GetScreenHeight() / 2.0f),
    };
    game->camera.zoom = (f32) GetScreenHeight() / PixelWindowHeight;
    game->camera.rotation = 0.0f;
}

