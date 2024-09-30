//
// Created by Hector Mejia on 8/23/24.
//

#ifndef MONSTERS_H
#define MONSTERS_H

#include <raylib.h>
#include "common.h"

// this seems like a nice way to do this faster
// https://cboard.cprogramming.com/c-programming/27503-enum-question-post186539.html#post186539


typedef enum MonsterType {
	MonsterTypeNone = 0,
	MonsterTypePlant = 1,
	MonsterTypeWater = 2,
	MonsterTypeFire = 3,
	MonsterTypeNormal = 4,

	MonsterTypeCount,
} MonsterType;

typedef enum MonsterAbilityID {
	MonsterAbilityNone,
	MonsterAbilityScratch,
	MonsterAbilitySpark,
	MonsterAbilityFire,
	MonsterAbilityBattleCry,
	MonsterAbilityExplosion,
	MonsterAbilityAnnihilate,
	MonsterAbilitySplash,
	MonsterAbilityIce,
	MonsterAbilityHeal,
	MonsterAbilityBurn,

	MonsterAbilityCount,
} MonsterAbilityID;

typedef enum MonsterID {
	MonsterIDNone = 0,
	MonsterIDPlumette = 1,
	MonsterIDIvieron = 2,
	MonsterIDPluma = 3,
	MonsterIDSparchu = 4,
	MonsterIDCindrill = 5,
	MonsterIDCharmadillo = 6,
	MonsterIDFinsta = 7,
	MonsterIDGulfin = 8,
	MonsterIDFiniette = 9,
	MonsterIDAtrox = 10,
	MonsterIDPouch = 11,
	MonsterIDDraem = 12,
	MonsterIDLarvea = 13,
	MonsterIDCleaf = 14,
	MonsterIDJacana = 15,
	MonsterIDFriolera = 16,

	MonsterIDCount,
} MonsterID;

typedef struct MonsterStats {
	f32 maxHealth;
	f32 maxEnergy;
	f32 attack;
	f32 defense;
	f32 recovery;
	f32 speed;
} MonsterStats;

#define MAX_MONSTER_NAME_LEN 32
#define MAX_MONSTER_ABILITIES_LEN 5
#define MAX_MONSTER_EVOLUTIONS_LEN 3
typedef struct MonsterData {
	MonsterID id;
	char name[MAX_MONSTER_NAME_LEN];
	MonsterType element;
	MonsterStats stats;

	struct {
		u8 level;
		MonsterAbilityID ability;
	} abilities[MAX_MONSTER_ABILITIES_LEN];
	u8 abilitiesLen;

	struct {
		MonsterID monster;
		u8 level;
	} evolution;
} MonsterData;

typedef enum MonsterAbilityTarget {
	MonsterAbilityTargetPlayer,
	MonsterAbilityTargetOpponent,
} MonsterAbilityTarget;

typedef struct MonsterAbilityData {
	MonsterAbilityID id;
	MonsterAbilityTarget target;
	f32 damageAmount;
	i32 cost;
	MonsterType element;
//	animation; ???
} MonsterAbilityData;

typedef struct Monster {
	MonsterID id;
	char name[MAX_MONSTER_NAME_LEN];
	u8 level;
	MonsterType type;
	MonsterStats stats;
	i32 xp;
	i32 levelUp;
	i32 health;
	i32 energy;
} Monster;

extern const char *monsterTypeStr[MonsterTypeCount];
extern const char *monsterAbilityStr[MonsterAbilityCount];

Monster monster_new(MonsterID id, u8 level);
MonsterType monster_type_from_str(const char *name);
MonsterAbilityID monster_ability_from_str(const char *name);
MonsterID monster_name_from_str(const char *name);
MonsterAbilityTarget monster_target_from_str(const char *name);
Color monster_type_color(MonsterType type);

#endif //MONSTERS_H
