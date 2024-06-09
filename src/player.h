//
// Created by Hector Mejia on 5/18/24.
//

#ifndef RAYLIB_POKEMON_CLONE_PLAYER_H
#define RAYLIB_POKEMON_CLONE_PLAYER_H

#include "raylib.h"
#include "common.h"
#include "sprites.h"

typedef enum CharacterState {
   CharacterStateIdle,
   CharacterStateWalking,

   CharacterStateMax,
} CharacterState;

typedef enum CharacterDirection {
    CharacterDirectionDown,
    CharacterDirectionLeft,
    CharacterDirectionRight,
    CharacterDirectionUp,

    CharacterDirectionMax,
} CharacterDirection;

typedef struct Player {
    Rectangle frame;
    Rectangle bounds;
    Vector2 velocity;
    AnimatedTiledSprite animatedSprite;
    CharacterDirection direction;
    CharacterState state;
} Player;

Player *player_new(Vector2 position);
void player_free(Player *player);
void player_input(Player *player);
void player_update(Player *player, f32 deltaTime);
void player_draw(Player *player);
void player_move(Player *player, f32 deltaTime);
Vector2 player_get_center(Player *player);


// todo(hector) character.h
const char* character_direction_string(CharacterDirection d);
const char *character_state_string(CharacterState d);

#endif //RAYLIB_POKEMON_CLONE_PLAYER_H
