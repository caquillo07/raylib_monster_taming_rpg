//
// Created by Hector Mejia on 6/13/24.
//

#include "character_entity.h"

#include <raymath.h>

#include "assets.h"
#include "game.h"
#include "game_data.h"
#include "raylib_extras.h"
#include "settings.h"
#include "array/array.h"

static void character_animate(Character *c, f32 deltaTime);
static bool line_intersects_rect(Vector2 p1, Vector2 p2, Rectangle r);
static bool character_has_line_of_sight(const Character *c, const Rectangle *rect);

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
        .radius = 0.f, // 0 means they dont fight?
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

void character_move_towards(Character *c, const Vector2 point) {
    // start to move the character towards the player
    c->velocity = Vector2Normalize(point);
}

void character_move(Character *c, const f32 deltaTime) {
    c->frame.x += c->velocity.x * c->speed * deltaTime;
    c->frame.y += c->velocity.y * c->speed * deltaTime;
}

void character_update(Character *c, const f32 deltaTime) {
    character_raycast(c);

    if (c->isPlayer) {
        // player
        if (!c->blocked) {
            character_move(c, deltaTime);
        }
    }

    if (!c->isPlayer) {
        if (!c->hasNoticed && !c->hasMoved) {
            if (timer_done(c->patrolTimer)) {
                const CharacterData *charData = game_data_for_character_id(c->id);
                const i32 nextIndex = c->currentDirectionIndex % charData->directionsLen;
                const CharacterDirection next = charData->directions[nextIndex];

                c->direction = next;
                c->currentDirectionIndex = (c->currentDirectionIndex + 1) % charData->directionsLen;
                timer_reset(&c->patrolTimer);
            }
        }
        if (c->hasNoticed && !game.player.noticed && !c->hasMoved) {
            character_move(c, deltaTime);

            if (CheckCollisionRecs(
                rectangle_inflate(c->hitBox, 10, 10),
                game.player.characterComponent.hitBox
            )) {
                const Vector2 stop = {};
                character_move_towards(c, stop); // stop the character
                c->hasMoved = true;

                character_create_dialog(c);
            }
        }
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

void character_create_dialog(const Character *character) {
    game.dialogBubble.characterCenter = character_get_center(character);
    game.dialogBubble.visible = true;
    strncpy(game.dialogBubble.characterID, character->id, MAX_CHARACTER_ID_LENGTH);
}

Vector2 character_get_center(const Character *const c) {
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


// check if there is a character in the line of sight
bool check_character_connection(const Character *const from, const Character *const to, const f32 radius) {
    const Vector2 distFromCharToPlayer = Vector2Subtract(
        character_get_center(to),
        character_get_center(from)
    );
    if (Vector2Length(distFromCharToPlayer) < radius) {
        const f32 tolerance = 30.f;
        if (from->direction == CharacterDirectionLeft &&
            // if the character is to the left of us
            distFromCharToPlayer.x < 0 &&
            // and we are in the same horizontal plane
            fabsf(distFromCharToPlayer.y) < tolerance) {
            return true;
        }

        if (from->direction == CharacterDirectionRight &&
            // if the character is to the right of us
            distFromCharToPlayer.x > 0 &&
            // and we are in the same horizontal plane
            fabsf(distFromCharToPlayer.y) < tolerance) {
            return true;
        }
        if (from->direction == CharacterDirectionUp &&
            // if the character is above us
            distFromCharToPlayer.y < 0 &&
            // and we are in the same vertical plane
            fabsf(distFromCharToPlayer.x) < tolerance) {
            return true;
        }
        if (from->direction == CharacterDirectionDown &&
            // if the character is below us
            distFromCharToPlayer.y > 0 &&
            // and we are in the same vertical plane
            fabsf(distFromCharToPlayer.x) < tolerance) {
            return true;
        }
    }

    return false;
}

void character_raycast(Character *c) {
    if (!c->canNoticePlayer) {
        return;
    }
    const bool hasConnection = check_character_connection(c, &game.player.characterComponent, c->radius);
    if (hasConnection && character_has_line_of_sight(c, &game.player.characterComponent.frame) && !c->hasMoved) {
        const Vector2 distFromCharToPlayer = Vector2Subtract(
            character_get_center(&game.player.characterComponent),
            character_get_center(c)
        );
        player_block(&game.player);

        // make the player look at the character
        character_change_direction(&game.player.characterComponent, (Vector2){c->frame.x, c->frame.y});
        if (!c->hasNoticed && !game.player.noticed) {
            player_set_noticed(&game.player);
            c->hasNoticed = true;
        }
        if (!timer_is_valid(game.player.noticedTimer) || !timer_done(game.player.noticedTimer)) {
            return;
        }
        player_unset_noticed(&game.player);

        // start to move the character twoards the player
        // roundf here ensurces the line is always straight, and not slightly tilted.
        const Vector2 dest = {
            .x = roundf(distFromCharToPlayer.x),
            .y = roundf(distFromCharToPlayer.y),
        };
        character_move_towards(c, dest);
    }
}

// not the best, but works here. If need more performance, use Cohen-Sutherland algorithm
// https://stackoverflow.com/a/23641016
static bool line_intersects_rect(const Vector2 p1, const Vector2 p2, const Rectangle r) {
    return CheckCollisionLines(p1, p2, (Vector2){r.x, r.y}, (Vector2){r.x + r.width, r.y}, nil) ||
           CheckCollisionLines(p1, p2, (Vector2){r.x + r.width, r.y}, (Vector2){r.x + r.width, r.y + r.height}, nil) ||
           CheckCollisionLines(p1, p2, (Vector2){r.x + r.width, r.y + r.height}, (Vector2){r.x, r.y + r.height}, nil) ||
           CheckCollisionLines(p1, p2, (Vector2){r.x, r.y + r.height}, (Vector2){r.x, r.y}, nil) ||
           (CheckCollisionPointRec(p1, r) && CheckCollisionPointRec(p2, r));
}

static bool character_has_line_of_sight(const Character *const c, const Rectangle *const rect) {
    const Vector2 characterCenter = character_get_center(c);
    const Vector2 rectPos = {.x = rect->x, .y = rect->y};
    const f32 distance = Vector2Distance(characterCenter, rectPos);
    if (distance < c->radius) {
        array_range(game.currentMap->collisionBoxes, i) {
            const Rectangle collisionRect = game.currentMap->collisionBoxes[i];
            if (line_intersects_rect(characterCenter, rectPos, collisionRect)) {
                return false;
            }
        }
    }
    return true;
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
