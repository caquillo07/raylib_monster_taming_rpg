//
// Created by Hector Mejia on 5/18/24.
//

#include "player.h"
#include "common.h"
#include "assets.h"
#include "character_entity.h"
#include "game.h"
#include "array/array.h"
#include "raymath.h"

static void player_handle_dialog(Player *p);
static bool check_player_connection(const Player *p, const Character character, const f32 radius);
void create_dialog(Player * p, const Character * character);

Player player_new(const Vector2 position) {
    const Player player = {
        .characterComponent = character_new(position, TileMapIDPlayer, CharacterDirectionDown, "player"),
    };
    return player;
}

void player_free(const Player *p) {
    character_free(&p->characterComponent);
}

void player_input(Player *p) {
    p->characterComponent.velocity = (Vector2){};
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        p->characterComponent.direction = CharacterDirectionUp;
        p->characterComponent.velocity.y -= 1;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        p->characterComponent.direction = CharacterDirectionDown;
        p->characterComponent.velocity.y += 1;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        p->characterComponent.direction = CharacterDirectionLeft;
        p->characterComponent.velocity.x -= 1;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        p->characterComponent.direction = CharacterDirectionRight;
        p->characterComponent.velocity.x += 1;
    }
    p->characterComponent.velocity = Vector2Normalize(p->characterComponent.velocity);

    if (IsKeyPressed(KEY_SPACE)) {
        player_handle_dialog(p);
    }
}

void player_update(Player *p, const f32 deltaTime) {
    const Rectangle oldPlayerFrame = p->characterComponent.frame;
    character_update(&p->characterComponent, deltaTime);

    // collisions
    array_range(game.currentMap->collisionBoxes, i) {
        const Rectangle box = game.currentMap->collisionBoxes[i];
        if (CheckCollisionRecs(p->characterComponent.hitBox, box)) {
            p->characterComponent.frame = oldPlayerFrame;
            break;
        }
    }
    array_range(game.currentMap->mainSprites, i) {
        const Entity entity = game.currentMap->mainSprites[i].entity;
        if (!entity.collideable) { continue; }

        if (CheckCollisionRecs(p->characterComponent.hitBox, entity.hitBox)) {
            game.player.characterComponent.frame = oldPlayerFrame;
            break;
        }
    }
    array_range(game.currentMap->overWorldCharacters, i) {
        const Rectangle box = game.currentMap->overWorldCharacters[i].hitBox;

        if (CheckCollisionRecs(p->characterComponent.hitBox, box)) {
            game.player.characterComponent.frame = oldPlayerFrame;
            break;
        }
    }
}

static void player_handle_dialog(Player *p) {
    array_range(game.currentMap->overWorldCharacters, i) {
        Character *character = &game.currentMap->overWorldCharacters[i];

        if (check_player_connection(p, *character, 100.f)) {
            printf("dialog with %s\n", character->id);
            // block player input
            player_block(p);
            // entities face each other
            character_change_direction(character, character_get_center(&p->characterComponent));
            // create dialog
            create_dialog(p, character);
        }
    }
}

// check if there is a character in the line of sight
static bool check_player_connection(const Player *p, const Character character, const f32 radius) {
    const Vector2 distFromCharToPlayer = Vector2Subtract(
        character_get_center(&character),
        character_get_center(&p->characterComponent)
    );
    if (Vector2Length(distFromCharToPlayer) < radius) {
        const f32 tolerance = 30.f;
        if (p->characterComponent.direction == CharacterDirectionLeft &&
            // if the character is to the left of us
            distFromCharToPlayer.x < 0 &&
            // and we are in the same horizontal plane
            fabsf(distFromCharToPlayer.y) < tolerance) {
            return true;
        }

        if (p->characterComponent.direction == CharacterDirectionRight &&
            // if the character is to the right of us
            distFromCharToPlayer.x > 0 &&
            // and we are in the same horizontal plane
            fabsf(distFromCharToPlayer.y) < tolerance) {
            return true;
        }
        if (p->characterComponent.direction == CharacterDirectionUp &&
            // if the character is above us
            distFromCharToPlayer.y < 0 &&
            // and we are in the same vertical plane
            fabsf(distFromCharToPlayer.x) < tolerance) {
            return true;
        }
        if (p->characterComponent.direction == CharacterDirectionDown &&
            // if the character is below us
            distFromCharToPlayer.y > 0 &&
            // and we are in the same vertical plane
            fabsf(distFromCharToPlayer.x) < tolerance) {
            return true;
        }
    }

    return false;
}

void create_dialog(Player *p, const Character *character) {
    const CharacterData *data = game_data_for_character_id(character->id);
    printf("data: %s\n", data->id);
}

void player_draw(const Player *p) {
    // ok because the characters and the player are the same exact structure.
    character_draw(&p->characterComponent);
}

Vector2 player_get_center(const Player *p) {
    // ok because the characters and the player are the same exact structure.
    return character_get_center(&p->characterComponent);
}

void player_block(Player *p) {
    p->characterComponent.blocked = true;
    p->characterComponent.velocity = (Vector2){};
}

void player_unblock(Player *p) {
    p->characterComponent.blocked = false;
}
