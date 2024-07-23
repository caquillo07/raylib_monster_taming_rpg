//
// Created by Hector Mejia on 6/13/24.
//

#include "character_entity.h"
#include "assets.h"
#include "game.h"
#include "settings.h"
#include "array/array.h"

Character character_new(const Vector2 centerPosition, const TileMapID tileMapID, const CharacterDirection direction) {
    const TileMap tm = assets.tileMaps[tileMapID];

    const i32 characterTileHeight = 128;
    const i32 characterTileWidth = 128;
    const Vector2 position = {
        .x = centerPosition.x - (f32) characterTileWidth / 2,
        .y = centerPosition.y - (f32) characterTileHeight / 2,
    };
    Character character = {
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
        .bounds = {
            .height = (float)characterTileHeight,
            .width = (float)characterTileWidth,
        },
        .animatedSprite = {
            .entity = {
                .id = tm.texture.id,
                .layer = WorldLayerMain,
                .ySort = position.y + (characterTileHeight/2),
                .position = position,
            },
            .texture = tm.texture,
            .framesLen = 4,
            .frameTimer = 0,
            .animationSpeed = settings.playerAnimationSpeed, // todo
        },
    };

    character.animatedSprite.sourceFrames = array_hold(
        character.animatedSprite.sourceFrames,
        character.animatedSprite.framesLen,
        sizeof(*character.animatedSprite.sourceFrames)
    );

    return character;
}

void character_free(const Character *character) {
    array_free(character->animatedSprite.sourceFrames);
}

void character_input(Character *character) {
    panic("unimplemented");
}

void character_move(Character *character, const f32 deltaTime) {
    character->frame.x += character->velocity.x * deltaTime;
    character->frame.y += character->velocity.y * deltaTime;
}

void character_update(Character *character, const f32 deltaTime) {
    character_move(character, deltaTime);

    character->state = CharacterStateIdle;
    if (character->velocity.x != 0 || character->velocity.y != 0) {
        character->state = CharacterStateWalking;
    }

    // update animation sprite before draw func
    character->animatedSprite.framesLen = 1; // base case, idling
    if (character->state == CharacterStateWalking) {
        character->animatedSprite.framesLen = 4;
    }

    i32 row = 0;
    switch (character->direction) {
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
        character->animatedSprite.sourceFrames[i] = tile_map_get_frame_at(
            assets.tileMaps[character->tileMapID],
            i,
            row
        );
    }
    update_animated_tiled_sprite(&character->animatedSprite, deltaTime);
    character->animatedSprite.entity.ySort = character->frame.y + (character->frame.height/2);
}

void character_draw(const Character *character) {
    const Vector2 pos = {
        .x = character->frame.x,
        .y = character->frame.y,
    };
    const Rectangle frame = character->animatedSprite.sourceFrames[character->animatedSprite.currentFrame];
    const Rectangle boundingBox = {
        .x = pos.x,
        .y = pos.y,
        .height = frame.height,
        .width = frame.width,
    };

    if (!CheckCollisionRecs(game.cameraBoundingBox, boundingBox)) {
        return;
    }

    game.gameMetrics.drawnSprites++;
    DrawTextureRec(character->animatedSprite.texture, frame, pos, WHITE);

    if (!game.isDebug) { return; }

    DrawRectangleLinesEx(boundingBox, 3.f, RED);
    DrawCircleV(pos, 5.f, RED);
}

Vector2 character_get_center(const Character *character) {
    return (Vector2){
        .x = character->frame.x + character->frame.width / 2,
        .y = character->frame.y + character->frame.height / 2,
    };
}

const char *character_direction_string(const CharacterDirection d) {
    static const char *CharacterDirectionStrings[CharacterDirectionMax] = {
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

void character_set_center_at(Character *character, Vector2 center) {
    character->frame.x = center.x - character->frame.width / 2;
    character->frame.y = center.y - character->frame.height / 2;
}
