//
// Created by Hector Mejia on 5/18/24.
//

#include <stdlib.h>
#include "player.h"
#include "common.h"
#include "memory/memory.h"
#include "settings.h"


Player *player_new(Vector2 position) {
    Player *player = mallocate(sizeof(*player), MemoryTagEntity);
    panicIf(player == nil, "failed to allocate player");

    Texture2D texture = LoadTexture("graphics/characters/player.png");
    *player = (Player) {
        .texture = texture,
        .direction = {},
        .frame = {
            .x = position.x,
            .y = position.y,
//            .height = (f32) texture.height,
//            .width = (f32) texture.width,
//            .x = 50,
//            .y = 50,
            .height = 128,
            .width = 128,
        },
        .bounds = {
            .height = 128.f,
            .width = 128.f,
        }
    };
    return player;
}

void player_free(Player *player) {
    UnloadTexture(player->texture);
    mfree(player, sizeof(*player), MemoryTagEntity);
}

void player_input(Player *player) {
    player->direction = (Vector2) {};
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        player->direction.y -= 1;
    } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->direction.y += 1;
    } else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->direction.x -= 1;
    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->direction.x += 1;
    }
}

void player_update(Player *player, f32 deltaTime) {
    player_move(player, deltaTime);
}

void player_move(Player *player, f32 deltaTime) {
    f32 deltaSpeed = settings.playerSpeed * deltaTime;
    player->frame.x += player->direction.x * deltaSpeed;
    player->frame.y += player->direction.y * deltaSpeed;
}

void player_draw(Player *player) {
//    DrawTextureV(player->texture, player->position, WHITE);
    DrawRectangleRec(player->frame, RED);
}

Vector2 player_get_center(Player *player) {
    return (Vector2) {
        .x = player->frame.x + player->frame.width/2,
        .y = player->frame.y + player->frame.height/2,
    };
}




