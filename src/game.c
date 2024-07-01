//
// Created by Hector Mejia on 5/4/24.
//

#include <stdlib.h>
#include "game.h"
#include "common.h"
#include "raylib.h"
#include "maps_manager.h"
#include "assets.h"

//
static void setup_game(MapID mapID);
static void game_draw_debug_camera();
static void update_camera();
static void game_draw_debug_screen();

//
MapID startingMap = MapIDWorld;

//
static bool frameStepMode = false;
static bool shouldRenderFrame = true;

//
Game game;

void game_init() {
    game = (Game){};
    maps_manager_init();
    load_textures();

    setup_game(startingMap);
}

void game_shutdown() {
    map_free(game.currentMap);
    game.currentMap = nil;

    player_free(game.player);
    game.player = nil;

    unload_textures();
}

static void do_game_handle_input() {
    if (game.isDebug && IsKeyPressed(KEY_F1)) {
        frameStepMode = !frameStepMode;
        shouldRenderFrame = false;
        return;
    }

    if (game.isDebug && frameStepMode && IsKeyPressed(KEY_SPACE)) {
        shouldRenderFrame = true;
        return;
    }

    if (IsKeyPressed(KEY_F2)) {
        game.isDebug = !game.isDebug;
        frameStepMode = false;
        shouldRenderFrame = true;
        return;
    }

    if (frameStepMode && !shouldRenderFrame) {
        return;
    }

    if (IsKeyPressed(KEY_F3) && game.currentMap->id != MapIDWorld) {
        map_free(game.currentMap);
        game.currentMap = load_map(MapIDWorld);
        game.player->characterComponent.frame.x = game.currentMap->playerStartingPosition.x;
        game.player->characterComponent.frame.y = game.currentMap->playerStartingPosition.y;
        return;
    }
    if (IsKeyPressed(KEY_F4) && game.currentMap->id != MapIDHospital) {
        map_free(game.currentMap);
        game.currentMap = load_map(MapIDHospital);
        game.player->characterComponent.frame.x = game.currentMap->playerStartingPosition.x;
        game.player->characterComponent.frame.y = game.currentMap->playerStartingPosition.y;
        return;
    }

    // todo if any of the events happens above, we never update the player input.
    //  we should instead hold a list of events on this frame and check them on each system
    //  instead. add the key press to the global event
    //  https://github.com/raysan5/raylib/blob/52f2a10db610d0e9f619fd7c521db08a876547d0/src/rcore.c#L297
    player_input(game.player);
}

void game_handle_input() {
    const clock_t now = clock();
    do_game_handle_input();
    game.gameMetrics.timeInInput = ((double) (clock() - now)) / (CLOCKS_PER_SEC / 1000);
}

static void do_game_update(const f32 deltaTime) {
    if (frameStepMode && !shouldRenderFrame) {
        return;
    }
    player_update(game.player, deltaTime);
    map_update(game.currentMap, deltaTime);

    update_camera();
}

void game_update(const f32 deltaTime) {
    const clock_t now = clock();
    do_game_update(deltaTime);
    game.gameMetrics.timeInUpdate = ((double) (clock() - now)) / (CLOCKS_PER_SEC / 1000);
}

static void do_game_draw() {
    /* todo? */
}

void game_draw() {
    const clock_t now = clock();
    BeginDrawing(); {
        ClearBackground(DARKGRAY);
        BeginMode2D(game.camera); {
            map_draw(game.currentMap);
            player_draw(game.player);
            game_draw_debug_camera();
        }
        EndMode2D();
        game_draw_debug_screen();
    }
    EndDrawing();
    if (frameStepMode) {
        shouldRenderFrame = false;
    }
    game.gameMetrics.timeInDraw = ((double) (clock() - now)) / (CLOCKS_PER_SEC / 1000);
}

static void game_draw_debug_screen() {
    if (!game.isDebug) { return; }

    DrawFPS(10, 10);

    const i32 fontSize = 20;
    const size_t textBufSize = 120;
    char mousePosText[textBufSize];
    const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), game.camera);
    snprintf(mousePosText, textBufSize, "Mouse %dx%d", (i32) mousePos.x, (i32) mousePos.y);
    DrawText(mousePosText, 12, 30, fontSize, DARKGREEN);

    char gameMetricsText[textBufSize * 3];
    snprintf(
        gameMetricsText,
        textBufSize * 3,
        "Time in input: %0.4f\n"
        "Time in update: %0.4f\n"
        "Time in draw: %0.4f",
        game.gameMetrics.timeInInput, // todo make this static variables inside the functions instead.
        game.gameMetrics.timeInUpdate,
        game.gameMetrics.timeInDraw
    );
    const Vector2 textSize = MeasureTextEx(GetFontDefault(), gameMetricsText, (f32) fontSize, 1);
    DrawText(gameMetricsText, 12, (i32) (30.f + textSize.y), 20, DARKBLUE);
}

static void game_draw_debug_camera() {
    if (!game.isDebug) { return; }

    // Draw camera anchor
    DrawCircleV(game.camera.target, 5.f, BLUE);
}

static void setup_game(const MapID mapID) {
    Map *map = load_map(mapID);
    game.currentMap = map;
    game.player = player_new(map->playerStartingPosition);

    // camera
    game.camera = (Camera2D){0};
    update_camera();
}

static void update_camera() {
    game.camera.target = player_get_center(game.player);
    game.camera.offset = (Vector2){
        .x = ((f32) GetScreenWidth() / 2.0f),
        .y = ((f32) GetScreenHeight() / 2.0f),
    };
    game.camera.zoom = (f32) GetScreenHeight() / PixelWindowHeight;
    game.camera.rotation = 0.0f;

    game.cameraBoundingBox = (Rectangle) {
        game.camera.target.x - game.camera.offset.x / game.camera.zoom,
        game.camera.target.y - game.camera.offset.y / game.camera.zoom,
        GetScreenWidth() / game.camera.zoom,
        GetScreenHeight() / game.camera.zoom
    };

}
