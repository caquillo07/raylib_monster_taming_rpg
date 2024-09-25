//
// Created by Hector Mejia on 8/23/24.
//

#include "monsters.h"
#include "game_data.h"
#include "colors.h"

const char *monsterTypeStr[MonsterTypeCount] = {
	[MonsterTypePlant] = "Plant",
	[MonsterTypeWater] = "Water",
	[MonsterTypeFire] = "Fire",
};

const char *monsterAbilityStr[MonsterAbilityCount] = {
	[MonsterAbilityNone] = "None",
	[MonsterAbilityScratch] = "Scratch",
	[MonsterAbilitySpark] = "Spark",
	[MonsterAbilityFire] = "Fire",
	[MonsterAbilityBattleCry] = "Battle Cry",
	[MonsterAbilityExplosion] = "Explosion",
	[MonsterAbilityAnnihilate] = "Annihilate",
	[MonsterAbilitySplash] = "Splash",
	[MonsterAbilityIce] = "Ice",
	[MonsterAbilityHeal] = "Heal",
	[MonsterAbilityBurn] = "Burn",
};

MonsterType monster_type_from_str(const char *name) {
	if (streq(name, "plant")) {
		return MonsterTypePlant;
	}
	if (streq(name, "fire")) {
		return MonsterTypeFire;
	}
	if (streq(name, "water")) {
		return MonsterTypeWater;
	}

	panic("unknown type \"%s\" provided", name);
}

MonsterAbility monster_ability_from_str(const char *name) {
	if (streq(name, "scratch")) {
		return MonsterAbilityScratch;
	}
	if (streq(name, "spark")) {
		return MonsterAbilitySpark;
	}
	if (streq(name, "fire")) {
		return MonsterAbilityFire;
	}
	if (streq(name, "battlecry")) {
		return MonsterAbilityBattleCry;
	}
	if (streq(name, "explosion")) {
		return MonsterAbilityExplosion;
	}
	if (streq(name, "annihilate")) {
		return MonsterAbilityAnnihilate;
	}
	if (streq(name, "splash")) {
		return MonsterAbilitySplash;
	}
	if (streq(name, "ice")) {
		return MonsterAbilityIce;
	}
	if (streq(name, "heal")) {
		return MonsterAbilityHeal;
	}
	if (streq(name, "burn")) {
		return MonsterAbilityBurn;
	}

	panic("unknown type \"%s\" provided", name);
}

// since there is a finite, not too large number of monsters,
// this if statements are more than enough. If we had 100s, as it would in a real
// game, I would come with a more resilient approach. Would likely have some sort
// of file database I can read into memory, and be editable from the outside.
MonsterID monster_name_from_str(const char *name) {
	if (streq(name, "Plumette")) {
		return MonsterIDPlumette;
	}
	if (streq(name, "Ivieron")) {
		return MonsterIDIvieron;
	}
	if (streq(name, "Pluma")) {
		return MonsterIDPluma;
	}
	if (streq(name, "Sparchu")) {
		return MonsterIDSparchu;
	}
	if (streq(name, "Cindrill")) {
		return MonsterIDCindrill;
	}
	if (streq(name, "Charmadillo")) {
		return MonsterIDCharmadillo;
	}
	if (streq(name, "Finsta")) {
		return MonsterIDFinsta;
	}
	if (streq(name, "Gulfin")) {
		return MonsterIDGulfin;
	}
	if (streq(name, "Finiette")) {
		return MonsterIDFiniette;
	}
	if (streq(name, "Atrox")) {
		return MonsterIDAtrox;
	}
	if (streq(name, "Pouch")) {
		return MonsterIDPouch;
	}
	if (streq(name, "Draem")) {
		return MonsterIDDraem;
	}
	if (streq(name, "Larvea")) {
		return MonsterIDLarvea;
	}
	if (streq(name, "Cleaf")) {
		return MonsterIDCleaf;
	}
	if (streq(name, "Jacana")) {
		return MonsterIDJacana;
	}
	if (streq(name, "Friolera")) {
		return MonsterIDFriolera;
	}

	panic("unknown name \"%s\" provided", name);
}

Monster monster_new(MonsterID id, u8 level) {
	MonsterData *data = game_data_for_monster_id(id);
	i32 levelUp = level * 150;
	i32 currentXP = (i32)random() % levelUp; // todo - remove, just some test data
	i32 currentHP = (i32)random() % (i32)(data->stats.maxHealth * (f32)level); // todo - remove, just some test data
	i32 currentEnergy = (i32)random() % (i32)(data->stats.maxEnergy * (f32)level); // todo - remove, just some test data
	Monster m = {
		.id = id,
		.level = level,
		.health = max(currentHP, 10),
		.energy = currentEnergy,
		.type = data->element,
		.stats = data->stats,
		.xp = currentXP,
		.levelUp = level * 150,
	};
	strncpy(m.name, data->name, MAX_MONSTER_NAME_LEN);
	return m;
}

Color monster_type_color(MonsterType type) {
	switch (type) {
		case MonsterTypePlant: return gameColors[ColorsPlant];
		case MonsterTypeWater: return gameColors[ColorsWater];
		case MonsterTypeFire: return gameColors[ColorsFire];
		case MonsterTypeCount:
		case MonsterTypeNone:
		default: panic("invalid monster type");
	}
}
