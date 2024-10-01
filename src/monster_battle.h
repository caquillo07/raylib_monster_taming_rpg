//
// Created by Hector Mejia on 9/30/24.
//

#pragma once

#include <raylib.h>
#include "common.h"
#include "monsters.h"
#include "sprites.h"

#define MAX_MONSTERS_PER_SIDE_LEN 3
typedef struct BattleStage {
	Texture2D bgTexture;
	Monster opponentMonsters[MAX_PARTY_MONSTERS_LEN];

	AnimatedTiledSprite playerMonsterSprites[MAX_MONSTERS_PER_SIDE_LEN];
	AnimatedTiledSprite opponentMonsterSprites[MAX_MONSTERS_PER_SIDE_LEN];
} BattleStage;


// todo - needed?
typedef enum BattleBackgroundID {
	BattleBackgroundIDNone,
	BattleBackgroundIDForrest,
	BattleBackgroundIDIce,
	BattleBackgroundIDSand,

	BattleBackgroundIDCount,
} BattleBackgroundID;

void monster_battle_setup();
void monster_battle_input();
void monster_battle_update(f32 dt);
void monster_battle_draw();
