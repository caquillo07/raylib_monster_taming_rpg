//
// Created by Hector Mejia on 5/4/24.
//

#include <stdlib.h>
#include "game.h"
#include "common.h"
#include "raylib.h"
#include "maps_manager.h"
#include "memory/memory.h"
#include "assets.h"
#include "settings.h"

static void init_game();
static void setup_game(Game *game, MapID mapID);
static void game_draw_debug_camera(Game *game);
static void update_camera(Game *game);
static void game_draw_debug_screen(Game *game);

const MapID startingMap = MapIDWorld;

static bool frameStepMode = false;
static bool shouldRenderFrame = true;

Game *game_new() {
    Game *game = mallocate(sizeof(*game), MemoryTagGame);
    panicIf(game == nil, "failed to allocate game");

    init_game();
    setup_game(game, startingMap);

    return game;
}

void game_destroy(Game *game) {
    map_free(game->currentMap);
    game->currentMap = nil;

    player_free(game->player);
    game->player = nil;

    mfree(game, sizeof(*game), MemoryTagGame);
    unload_textures();
}

static void init_game() {
    maps_manager_init();
    load_textures();
}

void game_handle_input(Game *game) {
    if (isDebug && IsKeyPressed(KEY_F1)) {
        frameStepMode = !frameStepMode;
        shouldRenderFrame = false;
        return;
    }

    if (isDebug && frameStepMode && IsKeyPressed(KEY_SPACE)) {
        shouldRenderFrame = true;
        return;
    }

    if (IsKeyPressed(KEY_F2)) {
        isDebug = !isDebug;
        frameStepMode = false;
        shouldRenderFrame = true;
        return;
    }

    if (frameStepMode && !shouldRenderFrame) {
        return;
    }

    if (IsKeyPressed(KEY_F3) && game->currentMap->id != MapIDWorld) {
        map_free(game->currentMap);
        game->currentMap = load_map(MapIDWorld);
        game->player->characterComponent.frame.x = game->currentMap->playerStartingPosition.x;
        game->player->characterComponent.frame.y = game->currentMap->playerStartingPosition.y;
        return;
    }
    if (IsKeyPressed(KEY_F4) && game->currentMap->id != MapIDHospital) {
        map_free(game->currentMap);
        game->currentMap = load_map(MapIDHospital);
        game->player->characterComponent.frame.x = game->currentMap->playerStartingPosition.x;
        game->player->characterComponent.frame.y = game->currentMap->playerStartingPosition.y;
        return;
    }

    player_input(game->player);
}

void game_update(Game *game, f32 deltaTime) {
    if (frameStepMode && !shouldRenderFrame) {
        return;
    }
    player_update(game->player, deltaTime);
    map_update(game->currentMap, deltaTime);

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
            game_draw_debug_camera(game);
        }
        EndMode2D();
        game_draw_debug_screen(game);
    }
    EndDrawing();
    if (frameStepMode) {
        shouldRenderFrame = false;
    }
}

static void game_draw_debug_screen(Game *game) {
    if (!isDebug) { return; }

    DrawFPS(10, 10);

    const size_t textBufSize = 120;
    char mousePosText[textBufSize];
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), game->camera);
    snprintf(mousePosText, textBufSize, "Mouse %dx%d", (i32) mousePos.x, (i32) mousePos.y);
    DrawText(mousePosText, 12, 30, 20, DARKGREEN);

}

static void game_draw_debug_camera(Game *game) {
    if (!isDebug) { return; }

    // Draw camera anchor
    DrawCircleV(game->camera.target, 5.f, BLUE);
}

static void setup_game(Game *game, MapID mapID) {
    if (game == nil) return;

    Map *map = load_map(mapID);
    game->currentMap = map;
    game->player = player_new(map->playerStartingPosition);

    // camera
    game->camera = (Camera2D) {0};
    update_camera(game);
}

static void update_camera(Game *game) {
    if (game == nil) return;

    game->camera.target = player_get_center(game->player);
    game->camera.offset = (Vector2) {
        .x = ((f32) GetScreenWidth() / 2.0f),
        .y = ((f32) GetScreenHeight() / 2.0f),
    };
    game->camera.zoom = (f32) GetScreenHeight() / PixelWindowHeight;
    game->camera.rotation = 0.0f;
}

