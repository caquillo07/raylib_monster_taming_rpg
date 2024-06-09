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


Player *player_new(Vector2 position) {
    Player *player = mallocate(sizeof(*player), MemoryTagEntity);
    panicIfNil(player, "failed to allocate player");

    TileMap tm = assets.playerTileMap;
    *player = (Player) {
        .state = CharacterStateIdle,
        .direction = CharacterDirectionDown,
        .velocity = {},
        .frame = {
            .x = position.x,
            .y = position.y,
            .height = 128,
            .width = 128,
        },
        .bounds = {
            .height = 128.f,
            .width = 128.f,
        },
        .animatedSprite = {
            .id = tm.texture.id,
            .texture = tm.texture,
            .framesLen = 4,
            .frameTimer = 0,
            .animationSpeed = settings.playerAnimationSpeed,
        },
    };

    player->animatedSprite.sourceFrames = array_hold(
        player->animatedSprite.sourceFrames,
        player->animatedSprite.framesLen,
        sizeof(*player->animatedSprite.sourceFrames)
    );

    return player;
}

void player_free(Player *player) {
    array_free(player->animatedSprite.sourceFrames);
    mfree(player, sizeof(*player), MemoryTagEntity);
}

void player_input(Player *player) {
    player->velocity = (Vector2) {};
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        player->direction = CharacterDirectionUp;
        player->velocity.y -= settings.playerSpeed;
    } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->direction = CharacterDirectionDown;
        player->velocity.y += settings.playerSpeed;
    } else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->direction = CharacterDirectionLeft;
        player->velocity.x -= settings.playerSpeed;
    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->direction = CharacterDirectionRight;
        player->velocity.x += settings.playerSpeed;
    }
}

void player_update(Player *player, f32 deltaTime) {
    player_move(player, deltaTime);

    player->state = CharacterStateIdle;
    if (player->velocity.x != 0 || player->velocity.y != 0) {
        player->state = CharacterStateWalking;
    }

    // update animation sprite before draw func
    player->animatedSprite.framesLen = 1; // base case, idling
    if (player->state == CharacterStateWalking) {
        player->animatedSprite.framesLen = 4;
    }

    i32 row = 0;
    switch (player->direction) {
        case CharacterDirectionDown:
            row = 0;
            break;
        case CharacterDirectionLeft:
            row = 1;
            break;
        case CharacterDirectionRight:
            row = 2;
            break;
        case CharacterDirectionUp:
            row = 3;
            break;
        case CharacterDirectionMax:
        default:
        panic("unknown player direction requested in draw");
    }
#define colsPerRow 4
    for (i32 i = 0; i < colsPerRow; i++) {
        player->animatedSprite.sourceFrames[i] = tile_map_get_frame_at(assets.playerTileMap, i, row);
    }
    update_animated_tiled_sprite(&player->animatedSprite, deltaTime);
}

void player_move(Player *player, f32 deltaTime) {
    player->frame.x += player->velocity.x * deltaTime;
    player->frame.y += player->velocity.y * deltaTime;
}

void player_draw(Player *player) {
    Vector2 pos = {
        .x = player->frame.x,
        .y = player->frame.y,
    };

    DrawTextureRec(
        assets.playerTileMap.texture,
        player->animatedSprite.sourceFrames[player->animatedSprite.currentFrame],
        pos,
        WHITE
    );
}

Vector2 player_get_center(Player *player) {
    return (Vector2) {
        .x = player->frame.x + player->frame.width / 2,
        .y = player->frame.y + player->frame.height / 2,
    };
}

const char *character_direction_string(CharacterDirection d) {
    static const char *CharacterDirectionStrings[CharacterDirectionMax] = {
        "CharacterDirectionDown",
        "CharacterDirectionLeft",
        "CharacterDirectionRight",
        "CharacterDirectionUp",
    };

    panicIf(d >= CharacterDirectionMax, "invalid CharacterDirection provided");

    return CharacterDirectionStrings[d];
}

const char *character_state_string(CharacterState d) {
    static const char *CharacterStateStrings[CharacterStateMax] = {
        "CharacterStateIdle",
        "CharacterStateWalking",
    };

    panicIf(d >= CharacterStateMax, "invalid CharacterState provided");
    return CharacterStateStrings[d];
}
