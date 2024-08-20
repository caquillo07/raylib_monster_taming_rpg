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
    Timer noticedTimer;
    bool noticed;
} Player;

Player player_new(Vector2 position);
void player_free(const Player *p);
void player_input(Player *p);
void player_update(Player *p, f32 deltaTime);
void player_draw(const Player *p);
Vector2 player_get_center(const Player *p);
void player_block(Player *p);
void player_unblock(Player *p);
void player_set_noticed(Player *p);
void player_unset_noticed(Player *p);

#endif //RAYLIB_POKEMON_CLONE_PLAYER_H
