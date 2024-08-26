//
// Created by Hector Mejia on 8/23/24.
//

#ifndef MONSTERS_H
#define MONSTERS_H

#include "common.h"

// this seems like a nice way to do this faster
// https://cboard.cprogramming.com/c-programming/27503-enum-question-post186539.html#post186539


typedef enum MonsterType {
    MonsterTypeNone = 0,
    MonsterTypePlant = 1,
    MonsterTypeWater = 2,
    MonsterTypeFire = 3,

    MonsterTypeCount,
} MonsterType;

typedef enum MonsterAbility {
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
} MonsterAbility;

typedef enum MonsterName {
    MonsterNameNone = 0,
    MonsterNamePlumette = 1,
    MonsterNameIvieron = 2,
    MonsterNamePluma = 3,
    MonsterNameSparchu = 4,
    MonsterNameCindrill = 5,
    MonsterNameCharmadillo = 6,
    MonsterNameFinsta = 7,
    MonsterNameGulfin = 8,
    MonsterNameFiniette = 9,
    MonsterNameAtrox = 10,
    MonsterNamePouch = 11,
    MonsterNameDraem = 12,
    MonsterNameLarvea = 13,
    MonsterNameCleaf = 14,
    MonsterNameJacana = 15,
    MonsterNameFriolera = 16,

    MonsterNameCount,
} MonsterID;

typedef struct MonsterStats {
    MonsterType element;
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
    MonsterStats stats;

    struct {
        u8 level;
        MonsterAbility ability;
    } abilities[MAX_MONSTER_ABILITIES_LEN];
    u8 abilitiesLen;

    struct {
        MonsterID monster;
        u8 level;
    } evolution;
} MonsterData;

typedef struct Monster {

} Monster;

MonsterType monster_type_from_str(const char *name);
MonsterAbility monster_ability_from_str(const char *name);
MonsterID monster_name_from_str(const char *name);

#endif //MONSTERS_H
