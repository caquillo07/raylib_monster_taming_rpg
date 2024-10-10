//
// Created by Hector Mejia on 8/23/24.
//

#include "monsters.h"
#include "game_data.h"
#include "colors.h"
#include "settings.h"

const i32 MonsterMaxInitiative = 100;

const char *monsterTypeStr[MonsterTypeCount] = {
	[MonsterTypePlant] = "Plant",
	[MonsterTypeWater] = "Water",
	[MonsterTypeFire] = "Fire",
	[MonsterTypeNormal] = "Normal",
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

MonsterAbilityAnimationID monster_ability_animation_from_str(const char *name) {
	if (streq(name, "explosion")) {
		return MonsterAbilityAnimationIDExplosion;
	}
	if (streq(name, "fire")) {
		return MonsterAbilityAnimationIDFire;
	}
	if (streq(name, "green")) {
		return MonsterAbilityAnimationIDGreen;
	}
	if (streq(name, "ice")) {
		return MonsterAbilityAnimationIDIce;
	}
	if (streq(name, "scratch")) {
		return MonsterAbilityAnimationIDScratch;
	}
	if (streq(name, "splash")) {
		return MonsterAbilityAnimationIDSplash;
	}

	panic("unknown type \"%s\" provided", name);
}

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
	if (streq(name, "normal")) {
		return MonsterTypeNormal;
	}

	panic("unknown type \"%s\" provided", name);
}

MonsterAbilityID monster_ability_from_str(const char *name) {
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


MonsterAbilityTarget monster_target_from_str(const char *name) {
	if (streq(name, "opponent")) {
		return MonsterAbilityTargetOpponent;
	}
	if (streq(name, "player")) {
		return MonsterAbilityTargetPlayer;
	}
	panic("unknown name \"%s\" provided", name)
}

Monster monster_new(MonsterID id, u8 level) {
	const MonsterData *data = game_data_for_monster_id(id);
	// we make a copy because we don't want to update the underlying data.
	MonsterStats stats = data->stats;
	stats.maxHealth *= (f32)level;
	stats.maxEnergy *= (f32)level;
	stats.attack *= (f32)level;
	stats.defense *= (f32)level;
	stats.recovery *= (f32)level;
	stats.speed *= (f32)level;

	i32 levelUp = level * 150;
	i32 currentXP = rand() % levelUp; // todo - remove, just some test data
	i32 currentHP = rand() % (i32)(stats.maxHealth); // todo - remove, just some test data
	i32 currentEnergy = rand() % (i32)(stats.maxEnergy); // todo - remove, just some test data
	Monster m = {
		.id = id,
		.state = MonsterStateActive,
		.level = level,
		.health = max(currentHP, 10),
		.energy = currentEnergy,
		.type = data->element,
		.stats = stats,
		.xp = currentXP,
		.levelUp = level * 150,
		.initiative = 0.f,
		.paused = false, // used in battles
	};
	strncpy(m.name, data->name, MAX_MONSTER_NAME_LEN);
	return m;
}

Color monster_type_color(MonsterType type) {
	switch (type) {
		case MonsterTypePlant: return gameColors[ColorsPlant];
		case MonsterTypeWater: return gameColors[ColorsWater];
		case MonsterTypeFire: return gameColors[ColorsFire];
		case MonsterTypeNormal: return gameColors[ColorsNormal];
		case MonsterTypeCount:
		case MonsterTypeNone:
		default: panic("invalid monster type %d", type);
	}
}

// this is hacky, but i don't care. We know that every animation has 4 frames,
// I don't feel like allocating a new buffer every time the menu gets opened,
// and I don't feel like refactoring the animation system right now.
// In a real game, this wouldn't fly, but for this demo its okay.
// todo - fix this?
static AnimatedTiledSprite monster_get_animated_sprite_for_id(MonsterID monsterID, i32 animationFramesRow, bool loop) {
	// turns out every sprite atlas has sprites of the same size,
	// so lets be lazy and do it once...
	const TileMap monsterTileSet = assets.monsterTileMaps[monsterID];
	AnimatedTiledSprite sprite = {
		.entity = {
			.id = monsterTileSet.texture.id,
			.layer = WorldLayerTop,
		},
		.texture = monsterTileSet.texture,
		.loop = loop,
		.framesLen = 4,
		.frameTimer = 0,
		.animationSpeed = settings.monsterAnimationSpeed,
	};
	for (i32 i = 0; i < AnimatedSpriteAnimationFramesLen; i++) {
		sprite.sourceFrames[i] = tile_map_get_frame_at(monsterTileSet, i, animationFramesRow);
	}
	return sprite;
}

#define idleAnimationFramesRow 0
#define attackAnimationFramesRow 1

AnimatedTiledSprite monster_get_idle_animated_sprite_for_id(MonsterID monsterID) {
	return monster_get_animated_sprite_for_id(monsterID, idleAnimationFramesRow, true);
}

AnimatedTiledSprite monster_get_attack_animated_sprite_for_id(MonsterID monsterID) {
	return monster_get_animated_sprite_for_id(monsterID, attackAnimationFramesRow, false);
}

Texture2D monster_icon_texture_for_id(MonsterID id) {
	panicIf(id == MonsterIDNone || id >= MonsterIDCount);
	switch (id) {
		case MonsterIDPlumette: return assets.monsterIcons.plumette;
		case MonsterIDIvieron: return assets.monsterIcons.ivieron;
		case MonsterIDPluma: return assets.monsterIcons.pluma;
		case MonsterIDSparchu: return assets.monsterIcons.sparchu;
		case MonsterIDCindrill: return assets.monsterIcons.cindrill;
		case MonsterIDCharmadillo: return assets.monsterIcons.charmadillo;
		case MonsterIDFinsta: return assets.monsterIcons.finsta;
		case MonsterIDGulfin: return assets.monsterIcons.gulfin;
		case MonsterIDFiniette: return assets.monsterIcons.finiette;
		case MonsterIDAtrox: return assets.monsterIcons.atrox;
		case MonsterIDPouch: return assets.monsterIcons.pouch;
		case MonsterIDDraem: return assets.monsterIcons.draem;
		case MonsterIDLarvea: return assets.monsterIcons.larvea;
		case MonsterIDCleaf: return assets.monsterIcons.cleaf;
		case MonsterIDJacana: return assets.monsterIcons.jacana;
		case MonsterIDFriolera: return assets.monsterIcons.friolera;
		case MonsterIDCount:
		case MonsterIDNone:
		default: panic("invalid monsterID");
	}
}
