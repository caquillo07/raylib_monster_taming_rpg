//
// Created by Hector Mejia on 5/18/24.
//

#include <stdlib.h>
#include "player.h"
#include "common.h"
#include <raymath.h>

const f32 playerSpeed = 250;

Player *player_new(Vector2 position) {
    Player *player = calloc(1, sizeof(*player));
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
    free(player);
}

void player_input(Player *player) {
    player->direction = (Vector2) {};
    if (IsKeyDown(KEY_UP)) {
        player->direction.y -= 1;
    } else if (IsKeyDown(KEY_DOWN)) {
        player->direction.y += 1;
    } else if (IsKeyDown(KEY_LEFT)) {
        player->direction.x -= 1;
    } else if (IsKeyDown(KEY_RIGHT)) {
        player->direction.x += 1;
    }
}

void player_update(Player *player, f32 deltaTime) {
    player_move(player, deltaTime);
}

void player_move(Player *player, f32 deltaTime) {
    f32 deltaSpeed = playerSpeed * deltaTime;
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




