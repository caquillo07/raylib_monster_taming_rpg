//
// Created by Hector Mejia on 5/18/24.
//

#include <stdlib.h>
#include "player.h"
#include "common.h"
#include "settings.h"
#include "assets.h"
#include "character_entity.h"
#include "game.h"
#include "array/array.h"
#include "raymath.h"


Player player_new(const Vector2 position) {
    const Player player = {
        .characterComponent = character_new(position, TileMapIDPlayer, CharacterDirectionDown),
    };
    return player;
}

void player_free(const Player *player) {
    character_free(&player->characterComponent);
}

void player_input(Player *player) {
    //    character_input(&player->characterComponent);

    player->characterComponent.velocity = (Vector2){};
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        player->characterComponent.direction = CharacterDirectionUp;
        player->characterComponent.velocity.y -= 1;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->characterComponent.direction = CharacterDirectionDown;
        player->characterComponent.velocity.y += 1;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->characterComponent.direction = CharacterDirectionLeft;
        player->characterComponent.velocity.x -= 1;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->characterComponent.direction = CharacterDirectionRight;
        player->characterComponent.velocity.x += 1;
    }
    
    printf("vel.x = %f - vel.y = %f / ", player->characterComponent.velocity.x, player->characterComponent.velocity.y);
    player->characterComponent.velocity = Vector2Normalize(player->characterComponent.velocity);
    printf("nvel.x = %f - nvel.y = %f\n", player->characterComponent.velocity.x, player->characterComponent.velocity.y);
}

void player_update(Player *player, const f32 deltaTime) {
    const Rectangle oldPlayerFrame = player->characterComponent.frame;
    character_update(&player->characterComponent, deltaTime);
    // collisions
    array_range(game.currentMap->collisionBoxes, i) {
        const Rectangle box = game.currentMap->collisionBoxes[i];
        if (CheckCollisionRecs(player->characterComponent.hitBox, box)) {
            player->characterComponent.frame = oldPlayerFrame;
            break;
        }
    }
    array_range(game.currentMap->mainSprites, i) {
        const Entity entity = game.currentMap->mainSprites[i].entity;
        if (!entity.collideable) { continue; }

        if (CheckCollisionRecs(player->characterComponent.hitBox, entity.hitBox)) {
            game.player.characterComponent.frame = oldPlayerFrame;
            break;
        }
    }
    array_range(game.currentMap->overWorldCharacters, i) {
        const Rectangle box = game.currentMap->overWorldCharacters[i].hitBox;

        if (CheckCollisionRecs(player->characterComponent.hitBox, box)) {
            game.player.characterComponent.frame = oldPlayerFrame;
            break;
        }
    }
}

void player_move(Player *player, f32 deltaTime) {
    panic("unimplemented");
}

void player_draw(const Player *player) {
    // ok because the characters and the player are the same exact structure.
    character_draw(&player->characterComponent);
}

Vector2 player_get_center(const Player *player) {
    // ok because the characters and the player are the same exact structure.
    return character_get_center(&player->characterComponent);
}
