//
// Created by Hector Mejia on 9/30/24.
//

#pragma once

#include <raylib.h>
#include "common.h"
#include "monsters.h"
#include "sprites.h"
#include "character_entity.h"

typedef enum BattleType {
	BattleTypeNone,
	BattleTypeTrainer,
	BattleTypeWildEncounter,
} BattleType;

typedef enum {
	BattleStageBackgroundForest,
	BattleStageBackgroundSand,
	BattleStageBackgroundIce,
	BattleStageBackgroundCount,
} BattleStageBackground;

#define MAX_MONSTERS_PER_SIDE_LEN 3
typedef struct BattleStage {
	Texture2D bgTexture;
	Monster opponentMonsters[MAX_MONSTER_PER_CHARACTER];

	BattleType battleType;
} BattleStage;

void monster_battle_setup();
void monster_battle_input();
void monster_battle_update(f32 dt);
void monster_battle_draw();
