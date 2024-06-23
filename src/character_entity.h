//
// Created by Hector Mejia on 6/13/24.
//

#ifndef RAYLIB_POKEMON_CLONE_CHARACTER_ENTITY_H
#define RAYLIB_POKEMON_CLONE_CHARACTER_ENTITY_H

#include "raylib.h"
#include "common.h"
#include "sprites.h"
#include "assets.h"

typedef enum CharacterState {
    CharacterStateIdle,
    CharacterStateWalking,

    CharacterStateMax,
} CharacterState;

const char *character_state_string(CharacterState d);

typedef enum CharacterDirection {
    CharacterDirectionDown,
    CharacterDirectionLeft,
    CharacterDirectionRight,
    CharacterDirectionUp,

    CharacterDirectionMax,
} CharacterDirection;

const char* character_direction_string(CharacterDirection d);

typedef struct Character {
    Rectangle frame;
    Rectangle bounds;
    Vector2 velocity;
    AnimatedTiledSprite animatedSprite;
    CharacterDirection direction;
    CharacterState state;
    TileMapID tileMapID;
} Character;

Character character_new(Vector2 centerPosition, TileMapID tileMapID, CharacterDirection direction);
void character_init(Character *character, Vector2 position, TileMapID tileMapID);
void character_free(Character *character);
void character_input(Character *character);
void character_update(Character *character, f32 deltaTime);
void character_draw(const Character *character);
void character_move(Character *character, f32 deltaTime);
void character_set_center_at(Character *character, Vector2 center);
Vector2 character_get_center(Character *character);


#endif //RAYLIB_POKEMON_CLONE_CHARACTER_ENTITY_H
