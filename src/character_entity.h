//
// Created by Hector Mejia on 6/13/24.
//

#ifndef RAYLIB_POKEMON_CLONE_CHARACTER_ENTITY_H
#define RAYLIB_POKEMON_CLONE_CHARACTER_ENTITY_H

#include "raylib.h"
#include "common.h"
#include "sprites.h"
#include "assets.h"
#include "timer.h"

typedef enum CharacterState {
    CharacterStateIdle,
    CharacterStateWalking,

    CharacterStateMax,
} CharacterState;

const char *character_state_string(CharacterState d);

typedef enum CharacterDirection {
    CharacterDirectionNone,
    CharacterDirectionDown,
    CharacterDirectionLeft,
    CharacterDirectionRight,
    CharacterDirectionUp,

    CharacterDirectionMax,
} CharacterDirection;

const char *character_direction_string(CharacterDirection d);

// todo - The player is very similar, but different enough that keeping them
// as one struct is annoying. I need to decouple this.
typedef struct Character {
    char id[128];
    Rectangle frame;
    Rectangle hitBox;
    Vector2 velocity;
    AnimatedTiledSprite animatedSprite;
    CharacterDirection direction;
    CharacterState state;
    TileMap tileMap;
    f32 speed;

    // todo remove - too lazy and im tired to think
    bool isPlayer;

    // todo this can be a bitmask instead, save some space. Enums even?
    bool blocked;
    bool hasMoved;
    bool canNoticePlayer;
    bool hasNoticed;
    f32 radius;

    Timer patrolTimer;
    i32 currentDirectionIndex;
} Character;

#define MAX_CHARACTER_ID_LENGTH 128
#define MAX_MONSTER_PER_CHARACTER 6
#define MAX_REGULAR_DIALOG_ENTRIES 3
#define MAX_DEFEATED_DIALOG_ENTRIES 2
#define MAX_DIRECTIONS_ENTRIES 4

// i've kind of realized i should've gone for a component based architecture
// instead, but im too deep to start over or refactor.
typedef struct CharacterData {
    char id[MAX_CHARACTER_ID_LENGTH];

    struct {
        u8 level;
        char name[32];
    } monsters[MAX_MONSTER_PER_CHARACTER];

    struct {
        char regular[MAX_REGULAR_DIALOG_ENTRIES][128];
        char defeated[MAX_DEFEATED_DIALOG_ENTRIES][128];
    } dialog;

    CharacterDirection direction;
    CharacterDirection directions[MAX_DIRECTIONS_ENTRIES];
    u8 directionsLen;
    bool lookAround;
    bool defeated;
    char biome[64];
} CharacterData;


Character character_new(Vector2 centerPosition, TileMap tileMap, CharacterDirection direction, const char *id);
void character_free(const Character *c);
void character_update(Character *c, f32 deltaTime);
void character_draw(const Character *c);
void character_move(Character *c, f32 deltaTime);
void character_set_center_at(Character *c, Vector2 center);
Vector2 character_get_center(const Character *c);
void character_change_direction(Character *c, Vector2 target);
CharacterDirection character_direction_from_str(const char *directionStr);
void character_raycast(Character *c);
bool check_character_connection(const Character *from, const Character *to, f32 radius);
inline void character_move_towards(Character *c, Vector2 point);
void character_create_dialog(const Character *character);

#endif //RAYLIB_POKEMON_CLONE_CHARACTER_ENTITY_H
