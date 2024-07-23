//
// Created by Hector Mejia on 5/18/24.
//

#ifndef RAYLIB_POKEMON_CLONE_PLAYER_H
#define RAYLIB_POKEMON_CLONE_PLAYER_H

#include "raylib.h"
#include "common.h"
#include "character_entity.h"

typedef struct Player {
    Character characterComponent;
} Player;

Player player_new(Vector2 position);
void player_free(Player *player);
void player_input(Player *player);
void player_update(Player *player, f32 deltaTime);
void player_draw(const Player *player);
void player_move(Player *player, f32 deltaTime);
Vector2 player_get_center(const Player *player);


#endif //RAYLIB_POKEMON_CLONE_PLAYER_H
