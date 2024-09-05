//
// Created by Hector Mejia on 8/30/24.
//

#include "monster_index.h"
#include "game.h"
#include "array/array.h"
#include "colors.h"
#include <ranlib.h>

void monster_index_state_init() {
    MonsterIndexState state = {
        .frame = {
            .height = (f32) GetScreenHeight(),
            .width = (f32) GetScreenWidth(),
            .x = 0,
            .y = 0,
        },
        .currentIndex = 0,
    };
    game.monsterIndex = state;
}

void monster_index_handle_input() {
    if (game.gameModeState != GameModeMonsterIndex) {
        return;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        game.gameModeState = GameModePlaying;
        game.monsterIndex.currentIndex = 0;
        return;
    }
    if (IsKeyPressed(KEY_UP)) {
        game.monsterIndex.currentIndex -= 1;
        if (game.monsterIndex.currentIndex < 0) {
            game.monsterIndex.currentIndex = (i32) static_array_len(game.playerMonsters) - 1;
        }
    }
    if (IsKeyPressed(KEY_DOWN)) {
        game.monsterIndex.currentIndex =
            (game.monsterIndex.currentIndex + 1) % (i32) static_array_len(game.playerMonsters);
    }
}

void monster_index_update(f32 dt) {
    if (game.gameModeState != GameModeMonsterIndex) {
        return;
    }
}

static Texture2D monster_icon_texture_for_id(MonsterID id) {
    panicIf(id == MonsterNameNone || id >= MonsterNameCount);
    switch (id) {
        case MonsterNamePlumette:
            return assets.monsters.plumette;
        case MonsterNameIvieron:
            return assets.monsters.ivieron;
        case MonsterNamePluma:
            return assets.monsters.pluma;
        case MonsterNameSparchu:
            return assets.monsters.sparchu;
        case MonsterNameCindrill:
            return assets.monsters.cindrill;
        case MonsterNameCharmadillo:
            return assets.monsters.charmadillo;
        case MonsterNameFinsta:
            return assets.monsters.finsta;
        case MonsterNameGulfin:
            return assets.monsters.gulfin;
        case MonsterNameFiniette:
            return assets.monsters.finiette;
        case MonsterNameAtrox:
            return assets.monsters.atrox;
        case MonsterNamePouch:
            return assets.monsters.pouch;
        case MonsterNameDraem:
            return assets.monsters.draem;
        case MonsterNameLarvea:
            return assets.monsters.larvea;
        case MonsterNameCleaf:
            return assets.monsters.cleaf;
        case MonsterNameJacana:
            return assets.monsters.jacana;
        case MonsterNameFriolera:
            return assets.monsters.friolera;
        case MonsterNameCount:
        case MonsterNameNone:
        default:
        panic("invalid monsterID");
    }
}

static const u8 visibleItems = 6;

void monster_index_draw() {
    if (game.gameModeState != GameModeMonsterIndex) {
        return;
    }
    Color c = {0, 0, 0, 200};

    // background
    DrawRectangleRec(game.monsterIndex.frame, c);
    // draw the menu in the center of the screen
    Rectangle menuRect = {
        .height = 0.8f * game.monsterIndex.frame.height,
        .width = 0.6f * game.monsterIndex.frame.width,
    };
    menuRect.x = (game.monsterIndex.frame.width - menuRect.width) / 2;
    menuRect.y = (game.monsterIndex.frame.height - menuRect.height) / 2;
    DrawRectangleRec(menuRect, BLACK);

    const f32 listWidth = menuRect.width * 0.3f;
    const f32 itemHeight = menuRect.height / (f32) visibleItems;
    const i32 indexesBelowDisplayed = game.monsterIndex.currentIndex - visibleItems + 1;
    const f32 tableOffset = game.monsterIndex.currentIndex < visibleItems ? 0 : -(f32)indexesBelowDisplayed * itemHeight;

    for (i32 i = 0; i < (i32) static_array_len(game.playerMonsters); i++) {
        const f32 top = menuRect.y + (f32) i * itemHeight + tableOffset;
        Rectangle menuItemRect = {
            .x = menuRect.x,
            .y = top,
            .height = itemHeight,
            .width = listWidth,
        };
        Color textColor = gameColors[ColorsWhite];
        Color backgroundColor = gameColors[ColorsGray];
        if (game.monsterIndex.currentIndex == i) {
            textColor = gameColors[ColorsDark];
            backgroundColor = gameColors[ColorsLight];
        }
        if (CheckCollisionRecs(menuItemRect, menuRect)) {
            const Monster monster = game.playerMonsters[i];
            const f32 menuItemRectMidLeft = menuItemRect.y + (menuItemRect.height / 2);
            DrawRectangleRec(menuItemRect, backgroundColor);

            // text
            const f32 textPadding = 90.f;
            const Vector2 textSize = MeasureTextEx(assets.regularFont, monster.name, 18, 1);
            const Vector2 pos = {
                .x = menuItemRect.x + textPadding,
                .y = menuItemRectMidLeft - (textSize.y / 2),
            };
            DrawTextEx(assets.regularFont, monster.name, pos, 18, 1, textColor);

            // monster icon
            const f32 monsterIconPadding = 45.f;
            Texture2D monsterIconTexture = monster_icon_texture_for_id(monster.id);
            const Rectangle monsterIconFrame = {
                .height = (f32) monsterIconTexture.height,
                .width = (f32) monsterIconTexture.width,
            };
            const Vector2 monsterIconPos = {
                .x = menuItemRect.x + monsterIconPadding - ((f32) monsterIconTexture.width / 2.f),
                .y = menuItemRectMidLeft - ((f32) monsterIconTexture.height / 2.f),
            };

            DrawTextureRec(monsterIconTexture, monsterIconFrame, monsterIconPos, WHITE);
        }
    }
}
