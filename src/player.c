//
// Created by Hector Mejia on 5/18/24.
//

#include <stdlib.h>
#include "player.h"
#include "common.h"
#include "memory/memory.h"
#include "settings.h"
#include "assets.h"
#include "array/array.h"
#include "character_entity.h"


Player *player_new(Vector2 position) {
    Player *player = mallocate(sizeof(*player), MemoryTagEntity);
    panicIfNil(player, "failed to allocate player");

    player->characterComponent = character_new(position, TileMapIDPlayer);
    return player;
}

void player_free(Player *player) {
    character_free(&player->characterComponent);
    mfree(player, sizeof(*player), MemoryTagEntity);
}

void player_input(Player *player) {
//    character_input(&player->characterComponent);

    player->characterComponent.velocity = (Vector2) {};
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        player->characterComponent.direction = CharacterDirectionUp;
        player->characterComponent.velocity.y -= settings.playerSpeed;
    } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->characterComponent.direction = CharacterDirectionDown;
        player->characterComponent.velocity.y += settings.playerSpeed;
    } else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->characterComponent.direction = CharacterDirectionLeft;
        player->characterComponent.velocity.x -= settings.playerSpeed;
    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->characterComponent.direction = CharacterDirectionRight;
        player->characterComponent.velocity.x += settings.playerSpeed;
    }
}

void player_update(Player *player, f32 deltaTime) {
    character_update(&player->characterComponent, deltaTime);
}

void player_move(Player *player, f32 deltaTime) {
    panic("unimplemented");
}

void player_draw(Player *player) {
    // ok because the characters and the player are the same exact structure.
    character_draw(&player->characterComponent);
}

Vector2 player_get_center(Player *player) {
    // ok because the characters and the player are the same exact structure.
    return character_get_center(&player->characterComponent);
}

