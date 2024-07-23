//
// Created by Hector Mejia on 5/4/24.
//

#ifndef RAYLIB_POKEMON_CLONE_GAME_H
#define RAYLIB_POKEMON_CLONE_GAME_H

#include "maps_manager.h"
#include "player.h"

typedef struct GameMetrics {
    f64 timeInInput;
    f64 timeInUpdate;
    f64 timeInDraw;
    i64 totalSprites;
    i64 drawnSprites;
} GameMetrics;

typedef struct Game {
    bool isDebug;

    Map *currentMap;
    Player player;
    Camera2D camera;
    Rectangle cameraBoundingBox;
    GameMetrics gameMetrics;
} Game;

extern Game game;

void game_init();
void game_handle_input();
void game_shutdown();
void game_update(f32 deltaTime);
void game_draw();

#endif //RAYLIB_POKEMON_CLONE_GAME_H
