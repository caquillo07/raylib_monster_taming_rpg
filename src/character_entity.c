//
// Created by Hector Mejia on 6/13/24.
//

#include "character_entity.h"

#include <raymath.h>

#include "assets.h"
#include "game.h"
#include "raylib_extras.h"
#include "settings.h"
#include "array/array.h"

static void character_animate(Character *c, f32 deltaTime);


Character character_new(
    const Vector2 centerPosition,
    const TileMapID tileMapID,
    const CharacterDirection direction,
    const char *id
) {
    const TileMap tm = assets.tileMaps[tileMapID];

    const i32 characterTileHeight = 128;
    const i32 characterTileWidth = 128;
    const Vector2 position = {
        .x = centerPosition.x - (f32) characterTileWidth / 2,
        .y = centerPosition.y - (f32) characterTileHeight / 2,
    };
    Character character = {
        .blocked = false,
        .state = CharacterStateIdle,
        .direction = direction,
        .tileMapID = tileMapID,
        .velocity = {},
        .frame = {
            .x = position.x,
            .y = position.y,
            .height = characterTileHeight,
            .width = characterTileWidth,
        },
        .animatedSprite = {
            .entity = {
                .id = tm.texture.id,
                .layer = WorldLayerMain,
                .ySort = position.y + (characterTileHeight / 2),
                .position = position,
            },
            .texture = tm.texture,
            .framesLen = 4,
            .frameTimer = 0,
            .animationSpeed = settings.playerAnimationSpeed, // todo
        },
    };

    // i happen to know there is nothing larger for this demo, wouldnt do in a
    // real project
    strncpy(character.id, id, 16);
    character.hitBox = rectangle_deflate(character.frame, character.frame.width / 2, 60);
    character.animatedSprite.sourceFrames = array_hold(
        character.animatedSprite.sourceFrames,
        character.animatedSprite.framesLen,
        sizeof(*character.animatedSprite.sourceFrames)
    );

    return character;
}

void character_free(const Character *c) {
    array_free(c->animatedSprite.sourceFrames);
}

void character_move(Character *c, const f32 deltaTime) {
    c->frame.x += c->velocity.x * settings.playerSpeed * deltaTime;
    c->frame.y += c->velocity.y * settings.playerSpeed * deltaTime;
}

void character_update(Character *c, const f32 deltaTime) {
    if (!c->blocked) {
        character_move(c, deltaTime);
    }

    character_animate(c, deltaTime);
}

void character_draw(const Character *c) {
    const Vector2 pos = {
        .x = c->frame.x,
        .y = c->frame.y,
    };
    const Rectangle frame = c->animatedSprite.sourceFrames[c->animatedSprite.currentFrame];
    const Rectangle boundingBox = {
        .x = pos.x,
        .y = pos.y,
        .height = frame.height,
        .width = frame.width,
    };

    if (!CheckCollisionRecs(game.cameraBoundingBox, boundingBox)) {
        return;
    }

    const Vector2 shadowPos = {
        .x = pos.x + 40,
        .y = pos.y + 110,
    };
    const Rectangle shadowRect = {
        .x = 0,
        .y = 0,
        .height = assets.characterShadowTexture.height,
        .width = assets.characterShadowTexture.width,
    };
    game.gameMetrics.drawnSprites++;
    DrawTextureRec(assets.characterShadowTexture, shadowRect, shadowPos, WHITE);

    game.gameMetrics.drawnSprites++;
    DrawTextureRec(c->animatedSprite.texture, frame, pos, WHITE);

    if (!game.isDebug) { return; }

    DrawRectangleLinesEx(boundingBox, 3.f, RED);
    DrawRectangleLinesEx(c->hitBox, 3.f, BLUE);
    DrawCircleV(pos, 5.f, RED);
}

void character_animate(Character *c, const f32 deltaTime) {
    c->state = CharacterStateIdle;
    if (c->velocity.x != 0 || c->velocity.y != 0) {
        c->state = CharacterStateWalking;
    }

    // update animation sprite before draw func
    c->animatedSprite.framesLen = 1; // base case, idling
    if (c->state == CharacterStateWalking) {
        c->animatedSprite.framesLen = 4;
    }

    i32 row = 0;
    switch (c->direction) {
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
            panic("unknown character direction requested in draw");
    }
#define colsPerRow 4
    for (i32 i = 0; i < colsPerRow; i++) {
        c->animatedSprite.sourceFrames[i] = tile_map_get_frame_at(
            assets.tileMaps[c->tileMapID],
            i,
            row
        );
    }
    update_animated_tiled_sprite(&c->animatedSprite, deltaTime);
    c->animatedSprite.entity.ySort = c->frame.y + (c->frame.height / 2);
    c->hitBox = rectangle_deflate(c->frame, c->frame.width / 2, 60);
}

Vector2 character_get_center(const Character *c) {
    return (Vector2){
        .x = c->frame.x + c->frame.width / 2,
        .y = c->frame.y + c->frame.height / 2,
    };
}

void character_change_direction(Character *c, const Vector2 target) {
    const f32 threshold = 30;
    const Vector2 relationToTarget = Vector2Subtract(target, character_get_center(c));
    if (fabsf(relationToTarget.y) < threshold) {
        if (relationToTarget.x > 0) {
            c->direction = CharacterDirectionRight;
        } else {
            c->direction = CharacterDirectionLeft;
        }
    } else {
        if (relationToTarget.y > 0) {
            c->direction = CharacterDirectionDown;
        } else {
            c->direction = CharacterDirectionUp;
        }
    }
}

void charager_get_dialog(const char *characterID, char **dialogArray, const u64 arrayLength) {
    panicIf(arrayLength == 0, "dialog array cannot be zero");
}

CharacterDirection character_direction_from_str(const char *directionStr) {
    if (streq(directionStr, "right")) {
        return CharacterDirectionRight;
    }
    if (streq(directionStr, "left")) {
        return CharacterDirectionLeft;
    }
    if (streq(directionStr, "up")) {
        return CharacterDirectionUp;
    }
    if (streq(directionStr, "down")) {
        return CharacterDirectionDown;
    }
    return CharacterDirectionNone;
}

const char *character_direction_string(const CharacterDirection d) {
    static const char *CharacterDirectionStrings[CharacterDirectionMax] = {
        "CharacterDirectionNone",
        "CharacterDirectionDown",
        "CharacterDirectionLeft",
        "CharacterDirectionRight",
        "CharacterDirectionUp",
    };

    panicIf(d >= CharacterDirectionMax, "invalid CharacterDirection provided");
    return CharacterDirectionStrings[d];
}

const char *character_state_string(const CharacterState d) {
    static const char *CharacterStateStrings[CharacterStateMax] = {
        "CharacterStateIdle",
        "CharacterStateWalking",
    };

    panicIf(d >= CharacterStateMax, "invalid CharacterState provided");
    return CharacterStateStrings[d];
}

void character_set_center_at(Character *c, const Vector2 center) {
    c->frame.x = center.x - c->frame.width / 2;
    c->frame.y = center.y - c->frame.height / 2;
}
