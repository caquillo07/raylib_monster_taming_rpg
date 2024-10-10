//

#include "monster_battle.h"
#include "game.h"
#include "settings.h"
#include "colors.h"
#include "ui.h"
#include "raylib_extras.h"
#include "game_data.h"

#include <raymath.h>

static void draw_monster(const Monster *monster, const AnimatedTiledSprite *sprite, Vector2 pos, bool flipped);
static void draw_monster_stat_bar(Vector2 textPos, f32 barWidth, i32 value, i32 maxValue, Colors barColor);
static void set_monster_highlight(bool activate);
static void pause_all_monster_initiative(bool pause);
static void draw_ui();
static void draw_general_ui();
static void draw_attacks_ui();
static void draw_switch_ui();
static bool player_monster_in_stage(const Monster *monster);
static void monster_start_attack();
static void apply_start_attack();
static void player_monster_update(Monster *monster, AnimatedTiledSprite *sprite, i32 fieldIndex, f32 dt);
static void opponent_monster_update(Monster *monster, AnimatedTiledSprite *sprite, i32 fieldIndex, f32 dt);
static bool common_monster_update(Monster *monster, AnimatedTiledSprite *sprite, i32 fieldIndex, f32 dt);

typedef enum selectionMode_ {
	SelectionModeNone,
	SelectionModeGeneral,
	SelectionModeAttack,
	SelectionModeSwitch,
	SelectionModeTarget,

	SelectionModeCount,
} selectionMode_;

typedef enum uiBattleChoiceIconID_ {
	UIBattleChoiceIconIDFight,
	UIBattleChoiceIconIDDefend,
	UIBattleChoiceIconIDSwitch,
	UIBattleChoiceIconIDCatch,
	UIBattleChoiceIconIDCount,
} uiBattleChoiceIconID_;

typedef enum selectionSide_ {
	SelectionSideNone,
	SelectionSidePlayer,
	SelectionSideOpponent,

	SelectionSideCount,
} selectionSide_;

struct monsterBattleState {
	// internal state, probably should have been a struct, but then i have to type twice
	Rectangle currentMonsterRect;
	Monster emptyMonster;

	struct {
		Monster *monster;
		i32 index;
	} currentMonster;
	struct {
		Monster *monster;
		i32 index;
	} selectedTargetMonster;

	// all spites are the same, so we can just swap the textures and reset the sprite
	struct {
		AnimatedTiledSprite sprite;
	} attackAnimationSprite;

	// player state
	AnimatedTiledSprite playerMonsterSprites[MAX_MONSTERS_PER_SIDE_LEN];
	Monster *playerActiveMonsters[MAX_MONSTERS_PER_SIDE_LEN];
	Vector2 playerActiveMonsterLocations[MAX_MONSTERS_PER_SIDE_LEN];

	// opponent state
	AnimatedTiledSprite opponentMonsterSprites[MAX_MONSTERS_PER_SIDE_LEN];
	Monster *opponentActiveMonsters[MAX_MONSTERS_PER_SIDE_LEN];
	Vector2 opponentActiveMonsterLocations[MAX_MONSTERS_PER_SIDE_LEN];

	bool highlight; // use to highlight monster when damage occurs
	i32 highlightLoc;
	Timer removeHighlightTimer;
	struct {
		selectionMode_ uiSelectionMode;
		//	uiBattleChoiceIconID_ selectedIcon;
		//	i32 selectedAttackIndex;
		i32 indexes[SelectionModeCount];
	} uiBattleChoiceState;
	selectionSide_ selectedSelectionSide;
	MonsterAbilityID selectedAttackID;
};

typedef struct uiBattleChoiceState_ {
	selectionMode_ uiSelectionMode;
//	uiBattleChoiceIconID_ selectedIcon;
//	i32 selectedAttackIndex;
	i32 indexes[SelectionModeCount];
} uiBattleChoiceState_;

struct selectedMonster_ {
	Monster *monster;
	i32 index;
};

struct attackAnimationSprite_ {
	AnimatedTiledSprite sprite;
};

static Monster emptyMonster = {};
static struct monsterBattleState state = {
	.currentMonsterRect = {},
	.emptyMonster = {},
	.currentMonster = {},
	.selectedTargetMonster = {},

	// all spites are the same, so we can just swap the textures and reset the sprite
	.attackAnimationSprite = {},

	// player state
	.playerMonsterSprites = {
	},
	.playerActiveMonsters = {&emptyMonster, &emptyMonster, &emptyMonster},
	.playerActiveMonsterLocations = {
		{.x=360, .y= 260},
		{.x=190, .y= 400},
		{.x=410, .y= 520},
	},

	// opponent state
	.opponentMonsterSprites = {
	},
	.opponentActiveMonsters = {&emptyMonster, &emptyMonster, &emptyMonster},
	.opponentActiveMonsterLocations = {
		{.x = 900, .y = 260},
		{.x = 1110, .y = 390},
		{.x = 900, .y = 550},
	},

	.highlight = false, // use to highlight monster when damage occurs
	.highlightLoc = 0,
	.removeHighlightTimer = {},
	.uiBattleChoiceState = {
		.indexes = {
			[SelectionModeGeneral] = 0,
			[SelectionModeAttack] = 0,
			[SelectionModeSwitch] = 0,
			[SelectionModeTarget] = 0,
		},
		.uiSelectionMode = SelectionModeNone,
	},
	.selectedSelectionSide = SelectionSideNone,
	.selectedAttackID = {},
};

void monster_battle_setup() {
	// player side
	const f32 minAnimSpeed = 0.85f;
	const f32 maxAnimSpeed = 1.00f;
	for (i32 i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
		panicIf(game.playerMonsters[i].id == MonsterIDNone && i == 0);
		if (game.playerMonsters[i].id == MonsterIDNone) {
			continue;
		}
		state.playerActiveMonsters[i] = &game.playerMonsters[i];
		state.playerMonsterSprites[i] = monster_get_idle_animated_sprite_for_id(
			game.playerMonsters[i].id
		);
		state.playerMonsterSprites[i].animationSpeed *= rand_f32(minAnimSpeed, maxAnimSpeed);
	}

	// opponent side
	for (i32 i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
		panicIf(game.battleStage.opponentMonsters[i].id == MonsterIDNone && i == 0);
		if (game.battleStage.opponentMonsters[i].id == MonsterIDNone) {
			continue;
		}
		state.opponentActiveMonsters[i] = &game.battleStage.opponentMonsters[i];
		state.opponentMonsterSprites[i] = monster_get_idle_animated_sprite_for_id(
			game.battleStage.opponentMonsters[i].id
		);
		state.opponentMonsterSprites[i].animationSpeed *= rand_f32(minAnimSpeed, maxAnimSpeed);
	}

	// set up the shaders - this should all be in a shader container, but ¯\_(ツ)_/¯ for now

	// Get shader locations
	i32 outlineSizeLoc = GetShaderLocation(assets.shaders.textureOutline, "outlineSize");
	i32 outlineColorLoc = GetShaderLocation(assets.shaders.textureOutline, "outlineColor");
	i32 textureSizeLoc = GetShaderLocation(assets.shaders.textureOutline, "textureSize");
	state.highlightLoc = GetShaderLocation(assets.shaders.textureOutline, "highlight");

	// shader values
	const Texture2D monsterTextureSample = state.playerMonsterSprites[0].texture;
	f32 outlineSize = 4.0f;
	f32 outlineColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};     // Normalized WHITE color
	f32 textureSize[2] = {(f32)monsterTextureSample.width, (f32)monsterTextureSample.height};

	// Set shader values (they can be changed later)
	SetShaderValue(assets.shaders.textureOutline, outlineSizeLoc, &outlineSize, SHADER_UNIFORM_FLOAT);
	SetShaderValue(assets.shaders.textureOutline, outlineColorLoc, outlineColor, SHADER_UNIFORM_VEC4);
	SetShaderValue(assets.shaders.textureOutline, textureSizeLoc, textureSize, SHADER_UNIFORM_VEC2);
	SetShaderValue(assets.shaders.textureOutline, state.highlightLoc, &state.highlight, SHADER_UNIFORM_INT);
}

void monster_battle_input() {
	if (game.gameModeState != GameModeBattle) { return; }

	selectionMode_ selectedMode = state.uiBattleChoiceState.uiSelectionMode;
	if (selectedMode == SelectionModeNone ||
		state.currentMonster.monster == nil ||
		state.currentMonster.monster == &emptyMonster) {
		return;
	}

	i32 maxIndex = 0;
	if (selectedMode == SelectionModeGeneral) {
		maxIndex = game.battleStage.battleType == BattleTypeTrainer ?
			UIBattleChoiceIconIDCatch : UIBattleChoiceIconIDCount;
	} else if (selectedMode == SelectionModeAttack) {
		const MonsterData *currentMonsterData = game_data_for_monster_id(state.currentMonster.monster->id);
		for (i32 i = 0; i < currentMonsterData->abilitiesLen; i++) {
			if (state.currentMonster.monster->level < currentMonsterData->abilities[i].level) { continue; }
			maxIndex++;
		}
	} else if (selectedMode == SelectionModeSwitch) {
		// hate to duplicate this, but whatever.
		maxIndex = 0;
		const Monster *availableMonsters[MAX_PARTY_MONSTERS_LEN] = {};
		for (usize i = 0; i < static_array_len(game.playerMonsters); i++) {
			const Monster *monster = &game.playerMonsters[i];
			if (monster->id == MonsterIDNone || monster->health <= 0 || player_monster_in_stage(monster)) { continue; }
			availableMonsters[maxIndex] = monster;
			maxIndex++;
		}
	} else if (selectedMode == SelectionModeTarget) {
		Monster **targetMonsters = state.selectedSelectionSide == SelectionSideOpponent ?
			state.opponentActiveMonsters : state.playerActiveMonsters;
		i32 opponentsCount = 0;

		for (usize i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
			if (targetMonsters[i]->id != MonsterIDNone && targetMonsters[i]->health > 0) {
				opponentsCount++;
			}
		}

		maxIndex = opponentsCount;
	} else {
		panic("invalid selection mode %d", state.uiBattleChoiceState.uiSelectionMode);
	}

	if (IsKeyPressed(KEY_DOWN)) {
		state.uiBattleChoiceState.indexes[selectedMode] = (state.uiBattleChoiceState.indexes[selectedMode] + 1) %
														  maxIndex;
	}
	if (IsKeyPressed(KEY_UP)) {
		state.uiBattleChoiceState.indexes[selectedMode] -= 1;
		if (state.uiBattleChoiceState.indexes[selectedMode] < 0) {
			state.uiBattleChoiceState.indexes[selectedMode] = maxIndex - 1;
		}
	}
	if (IsKeyPressed(KEY_SPACE)) {
		if (selectedMode == SelectionModeTarget) {
			Monster **targetMonsters = state.selectedSelectionSide == SelectionSideOpponent ?
				state.opponentActiveMonsters : state.playerActiveMonsters;

			i32 displayedIndex = 0;
			for (usize i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
				if (targetMonsters[i]->id != MonsterIDNone && targetMonsters[i]->health > 0) {
					if (displayedIndex == state.uiBattleChoiceState.indexes[SelectionModeTarget]) {
						printfln("selected monster %s", targetMonsters[i]->name);
						state.selectedTargetMonster.monster = targetMonsters[i];
						state.selectedTargetMonster.index = (i32)i;
						break;
					}
					displayedIndex++;
				}
			}
			state.uiBattleChoiceState.indexes[SelectionModeTarget] = 0;

			if (state.selectedAttackID != MonsterAbilityNone) {
				monster_start_attack();
				selectedMode = SelectionModeNone;
				pause_all_monster_initiative(false);
			} else {
				// catch
			}
		}
		if (selectedMode == SelectionModeAttack) {
			panicIfNil(state.currentMonster.monster);
			state.uiBattleChoiceState.uiSelectionMode = SelectionModeTarget;
			const MonsterData *currentMonsterData = game_data_for_monster_id(state.currentMonster.monster->id);
			i32 displayedIndex = 0;
			for (i32 i = 0; i < currentMonsterData->abilitiesLen; i++) {
				if (state.currentMonster.monster->level < currentMonsterData->abilities[i].level) { continue; }
				if (displayedIndex == state.uiBattleChoiceState.indexes[SelectionModeAttack]) {
					state.selectedAttackID = currentMonsterData->abilities[i].ability;
					break;
				}
				displayedIndex++;
			}
			printfln("chosen attack %s", monsterAbilityStr[state.selectedAttackID]);
			state.selectedSelectionSide = game_data_for_monster_attack_id(state.selectedAttackID)->target ==
										  MonsterAbilityTargetPlayer ?
				SelectionSidePlayer : SelectionSideOpponent;
		}
		if (selectedMode == SelectionModeGeneral) {
			switch (state.uiBattleChoiceState.indexes[SelectionModeGeneral]) {
				case UIBattleChoiceIconIDFight: {
					state.uiBattleChoiceState.uiSelectionMode = SelectionModeAttack;
					break;
				}
				case UIBattleChoiceIconIDDefend: {
					pause_all_monster_initiative(false);
					state.currentMonster.monster = &emptyMonster;
					state.currentMonster.index = 0;
					state.uiBattleChoiceState.uiSelectionMode = SelectionModeNone;
					break;
				}
				case UIBattleChoiceIconIDSwitch: {
					state.uiBattleChoiceState.uiSelectionMode = SelectionModeSwitch;
					break;
				}
				case UIBattleChoiceIconIDCatch: {
					state.uiBattleChoiceState.uiSelectionMode = SelectionModeTarget;
					break;
				}
				case UIBattleChoiceIconIDCount:
				default: panic(
					"tried to select on invalid battle icon state %d",
					state.uiBattleChoiceState.indexes[SelectionModeGeneral]
				);
			}
		}
	}
	if (IsKeyPressed(KEY_LEFT)) {
		if (selectedMode == SelectionModeSwitch ||
			selectedMode == SelectionModeAttack ||
			selectedMode == SelectionModeTarget) {
			state.uiBattleChoiceState.uiSelectionMode = SelectionModeGeneral;
		}
	}
}

static AnimatedTiledSprite new_animation_attack(Texture2D texture) {
//	const TileMap monsterTileSet = assets.monsterTileMaps[monsterID];
//	AnimatedTiledSprite sprite = {
//		.entity = {
//			.id = monsterTileSet.texture.id,
//			.layer = WorldLayerTop,
//		},
//		.texture = monsterTileSet.texture,
//		.loop = loop,
//		.framesLen = 4,
//		.frameTimer = 0,
//		.animationSpeed = settings.monsterAnimationSpeed,
//	};
//	for (i32 i = 0; i < AnimatedSpriteAnimationFramesLen; i++) {
//		sprite.sourceFrames[i] = tile_map_get_frame_at(monsterTileSet, i, animationFramesRow);
//	}
//	return sprite;
	return (AnimatedTiledSprite){};
}

static void monster_start_attack() {
	panicIf(state.selectedAttackID == MonsterAbilityNone);

	state.currentMonster.monster->state = MonsterStateAttack;
	state.playerMonsterSprites[state.currentMonster.index] = monster_get_attack_animated_sprite_for_id(
		state.currentMonster.monster->id
	);
}

static void apply_start_attack() {
	panicIf(state.currentMonster.monster->state != MonsterStateAttack && state.selectedAttackID == MonsterAbilityNone);

	// get the attack data
	const MonsterAbilityData *abilityData = game_data_for_monster_attack_id(state.selectedAttackID);
	state.currentMonster.monster->energy -= abilityData->cost;
	state.currentMonster.monster->energy = max(0, state.currentMonster.monster->energy);

	state.selectedTargetMonster.monster->health -= (i32)(
		state.currentMonster.monster->stats.attack *
		abilityData->damageAmount
	);
	state.selectedTargetMonster.monster->health = max(0, state.selectedTargetMonster.monster->health);

	state.currentMonster.monster->state = MonsterStateActive;
	state.selectedAttackID = MonsterAbilityNone;

	// clear for next monster attack
	state.currentMonster.monster = &emptyMonster;
	state.currentMonster.index = 0;
}

static void pause_all_monster_initiative(bool pause) {
	for (i32 j = 0; j < MAX_MONSTERS_PER_SIDE_LEN; j++) {
		state.playerActiveMonsters[j]->paused = pause;
		state.opponentActiveMonsters[j]->paused = pause;
	}
}

static bool common_monster_update(Monster *monster, AnimatedTiledSprite *sprite, i32 fieldIndex, f32 dt) {
	panicIf(fieldIndex >= MAX_MONSTERS_PER_SIDE_LEN);

	// check if attack animation is playing, and it's done.
	// would be nice to have animation hooks, but we already know when an animation
	// is finished here, and that's good enough.
//	for (i32 i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
//		const i32 index = i % MAX_MONSTERS_PER_SIDE_LEN;
	if (monster->state == MonsterStateAttack && sprite->done) {
		apply_start_attack();
		*sprite = monster_get_idle_animated_sprite_for_id(monster->id);
	}
//		if (state.opponentActiveMonsters[index]->state == MonsterStateAttack && state.opponentMonsterSprites[index].done) {
//			state.opponentMonsterSprites[index] = monster_get_idle_animated_sprite_for_id(
//				state.opponentActiveMonsters[index]->id
//			);
//			apply_start_attack();
//		}
//	}

	// if any monster reached the max initiative, then we should pause the rest
//	Monster *allMonsters[MAX_MONSTERS_PER_SIDE_LEN * 2];
//	for (i32 i = 0; i < MAX_MONSTERS_PER_SIDE_LEN * 2; i++) {
//		if (i < MAX_MONSTERS_PER_SIDE_LEN) {
//			allMonsters[i] = state.playerActiveMonsters[i % MAX_MONSTERS_PER_SIDE_LEN];
//		} else {
//			allMonsters[i] = state.opponentActiveMonsters[i % MAX_MONSTERS_PER_SIDE_LEN];
//		}
//	}

	if (!monster->paused) {
		monster->initiative = min(monster->initiative + monster->stats.speed * dt, MonsterMaxInitiative);
	}

	// checking to see who is active
//	for (i32 i = 0; i < MAX_MONSTERS_PER_SIDE_LEN * 2; i++) {
	if (monster->initiative >= 100) {
		pause_all_monster_initiative(true);
		state.currentMonster.monster = monster;
		state.currentMonster.monster->initiative = 0;
		state.currentMonster.index = fieldIndex; // original index
		set_monster_highlight(true);

		// if is a player monster, index 0,1,2
		// this assumption breaks if the ordering above changes.
//			break;
		return true;
	}
//	}

	animated_tiled_sprite_update(sprite, dt);
	return false;
}

static void set_monster_highlight(bool activate) {
	state.highlight = activate;
	if (activate) {
		timer_start(&state.removeHighlightTimer, settings.monsterBattleRemoveHighlightIntervalSecs);
	}
}

static void player_monster_update(Monster *monster, AnimatedTiledSprite *sprite, i32 fieldIndex, f32 dt) {
	panicIf(fieldIndex >= MAX_MONSTERS_PER_SIDE_LEN);

	bool finishedTurn = common_monster_update(monster, sprite, fieldIndex, dt);
	if (finishedTurn) {
		state.uiBattleChoiceState.uiSelectionMode = SelectionModeGeneral;
		state.uiBattleChoiceState.indexes[SelectionModeGeneral] = UIBattleChoiceIconIDFight;
	}
}

static void opponent_monster_update(Monster *monster, AnimatedTiledSprite *sprite, i32 fieldIndex, f32 dt) {
	panicIf(fieldIndex >= MAX_MONSTERS_PER_SIDE_LEN);

	common_monster_update(monster, sprite, fieldIndex, dt);
}

void monster_battle_update(f32 dt) {
	if (game.gameModeState != GameModeBattle) { return; }

	// timers
	if (timer_is_valid(state.removeHighlightTimer) && timer_done(state.removeHighlightTimer)) {
		set_monster_highlight(false);
		timer_stop(&state.removeHighlightTimer);
	}

	// player
	for (i32 i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
		if (state.playerActiveMonsters[i]->id == MonsterIDNone) { continue; }
		player_monster_update(
			state.playerActiveMonsters[i],
			&state.playerMonsterSprites[i],
			i,
			dt
		);
	}

	// opponent
	for (i32 i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
		if (state.opponentActiveMonsters[i]->id == MonsterIDNone) { continue; }
		opponent_monster_update(
			state.opponentActiveMonsters[i],
			&state.opponentMonsterSprites[i],
			i,
			dt
		);
	}

	// update shaders data
	SetShaderValue(assets.shaders.textureOutline, state.highlightLoc, &state.highlight, SHADER_UNIFORM_INT);
}

// normally you would want to add some sort of sprite + z-value for draw order,
// but since this scene is fairly basic, and stuff won't dynamically change their
// z ordering; we will just manually draw them in the correct order.
void monster_battle_draw() {
	if (game.gameModeState != GameModeBattle) { return; }

	DrawTexture(game.battleStage.bgTexture, 0, 0, WHITE);
	DrawTexturePro(
		game.battleStage.bgTexture,
		(Rectangle){
			.x= 0,
			.y= 0,
			.height = (f32)game.battleStage.bgTexture.height,
			.width = (f32)game.battleStage.bgTexture.width,
		},
		(Rectangle){
			.x = 0,
			.y = 0,
			.height = (f32)GetScreenHeight(),
			.width = (f32)GetScreenWidth(),
		},
		(Vector2){0, 0},
		0.f,
		WHITE
	);
	// monsters
	draw_monster(
		state.playerActiveMonsters[0],
		&state.playerMonsterSprites[0],
		state.playerActiveMonsterLocations[0],
		true
	);
	draw_monster(
		state.playerActiveMonsters[1],
		&state.playerMonsterSprites[1],
		state.playerActiveMonsterLocations[1],
		true
	);
	draw_monster(
		state.playerActiveMonsters[2],
		&state.playerMonsterSprites[2],
		state.playerActiveMonsterLocations[2],
		true
	);

	draw_monster(
		state.opponentActiveMonsters[0],
		&state.opponentMonsterSprites[0],
		state.opponentActiveMonsterLocations[0],
		false
	);
	draw_monster(
		state.opponentActiveMonsters[1],
		&state.opponentMonsterSprites[1],
		state.opponentActiveMonsterLocations[1],
		false
	);
	draw_monster(
		state.opponentActiveMonsters[2],
		&state.opponentMonsterSprites[2],
		state.opponentActiveMonsterLocations[2],
		false
	);

	draw_ui();
}

static void draw_monster(const Monster *monster, const AnimatedTiledSprite *sprite, Vector2 pos, bool flipped) {
	if (monster->id == MonsterIDNone || monster->health <= 0) { return; }
	Rectangle animationFrame = animated_tiled_sprite_current_frame(sprite);
	Rectangle monsterDestRec = {
		.x = pos.x - animationFrame.width / 2,
		.y = pos.y - animationFrame.height / 2,
		.height = animationFrame.height,
		.width = animationFrame.width,
	};
	animationFrame.width *= (f32)(flipped ? -1 : 1);
	// monster name/level box
	const char *monsterName = monster->name;
	const f32 monsterNamePadding = 10.f;
	const Vector2 monsterNameSize = MeasureTextEx(
		assets.fonts.regular.rFont,
		monsterName,
		assets.fonts.regular.size,
		1.0f
	);
	Rectangle monsterNameRect = {
		.height = monsterNameSize.y + monsterNamePadding * 2,
		.width = monsterNameSize.x + monsterNamePadding * 2,
	};
	if (flipped) {
		// player
		monsterNameRect.x = (monsterDestRec.x + 16) - (monsterNameRect.width / 2);
		monsterNameRect.y = monsterDestRec.y + (monsterDestRec.height / 2) - 70;
	} else {
		monsterNameRect.x = (monsterDestRec.x + monsterDestRec.width) - 40 - (monsterNameRect.width / 2);
		monsterNameRect.y = monsterDestRec.y + (monsterDestRec.height / 2) - 70;
	}
	const Vector2 monsterNamePos = {
		.x = monsterNameRect.x + monsterNamePadding,
		.y = monsterNameRect.y + monsterNamePadding,
	};
	DrawRectangleRec(monsterNameRect, gameColors[ColorsWhite]);
	DrawTextEx(
		assets.fonts.regular.rFont,
		monsterName,
		monsterNamePos,
		assets.fonts.regular.size,
		1.0f,
		gameColors[ColorsBlack]
	);
	const f32 xpBarHeight = 2.f;
	const Rectangle xpBarRect = {
		.x = monsterNameRect.x,
		.y = monsterNameRect.y + monsterNameRect.height - xpBarHeight,
		.height = xpBarHeight,
		.width = monsterNameRect.width,
	};
	if (flipped) {
		// only draw for player
		ui_draw_progress_bar(
			xpBarRect,
			(f32)monster->xp,
			(f32)monster->levelUp,
			gameColors[ColorsBlack],
			gameColors[ColorsWhite],
			0
		);
	}

	// Monster level
	const char *monsterLevelText = TextFormat("Lv. %d", monster->level);
	const f32 monsterLevelPadding = 10.f;
	Rectangle monsterLevelRect = {
		.width = 60,
		.height = 26,
	};
	if (flipped) {
		// player
		monsterLevelRect.x = monsterNameRect.x;
		monsterLevelRect.y = monsterNameRect.y + monsterNameRect.height;
	} else {
		monsterLevelRect.x = monsterNameRect.x + monsterNameRect.width - monsterLevelRect.width;
		monsterLevelRect.y = monsterNameRect.y + monsterNameRect.height;
	}
	const Vector2 monsterLevelPos = {
		.x = monsterLevelRect.x + monsterLevelPadding,
		.y = monsterLevelRect.y + monsterLevelPadding / 2,
	};

	DrawRectangleRec(monsterLevelRect, gameColors[ColorsWhite]);
	DrawTextEx(
		assets.fonts.small.rFont,
		monsterLevelText,
		monsterLevelPos,
		assets.fonts.small.size,
		1.0f,
		gameColors[ColorsBlack]
	);

	bool isSelectedOpponent = false;
	if (state.selectedSelectionSide != SelectionSideNone &&
		state.uiBattleChoiceState.uiSelectionMode == SelectionModeTarget) {
		Monster **targetMonsters = state.selectedSelectionSide == SelectionSideOpponent ?
			state.opponentActiveMonsters : state.playerActiveMonsters;
		i32 displayedIndex = 0;
		for (usize i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
			if (targetMonsters[i]->id != MonsterIDNone && targetMonsters[i]->health > 0) {
				if (state.uiBattleChoiceState.indexes[SelectionModeTarget] == displayedIndex &&
					monster == targetMonsters[i]) {
					isSelectedOpponent = true;
					break;
				}
				displayedIndex++;
			}
		}
	}

	const bool isCurrentMonster = state.currentMonster.monster == monster &&
								  state.uiBattleChoiceState.uiSelectionMode != SelectionModeTarget;
	if (isCurrentMonster) {
		state.currentMonsterRect = monsterDestRec; // todo - this is a big no no, but i don't feel like restructuring the code...
	}
	if (isCurrentMonster || isSelectedOpponent) {
		BeginShaderMode(assets.shaders.textureOutline);
	}

	// draw the monster AFTER the name/level
	DrawTexturePro(
		sprite->texture,
		animationFrame,
		monsterDestRec,
		(Vector2){0, 0},
		0.f,
		WHITE
	);
	if (isCurrentMonster || isSelectedOpponent) {
		EndShaderMode();
	}

	// monster stats
	Rectangle monsterStatsRect = {
		.width = 150,
		.height = 48,
	};
	// we want the mid-top of the stats to be at the mid-bottom of the monster
	// sprite, plus a 20 px padding
	monsterStatsRect.x = monsterDestRec.x + (monsterDestRec.width - monsterStatsRect.width) / 2;
	monsterStatsRect.y = monsterDestRec.y + monsterDestRec.height + 20 - monsterStatsRect.height;

	DrawRectangleRec(monsterStatsRect, gameColors[ColorsWhite]);

	// health and energy
	const f32 barWidth = monsterStatsRect.width * 0.9f;
	Vector2 statPosition = {
		.x = monsterStatsRect.x + (monsterStatsRect.width * 0.05f),
		.y = monsterStatsRect.y,
	};
	draw_monster_stat_bar(
		statPosition,
		barWidth,
		(i32)monster->health,
		(i32)monster->stats.maxHealth,
		ColorsRed
	);

	statPosition.y = monsterStatsRect.y + (monsterStatsRect.height / 2);
	draw_monster_stat_bar(
		statPosition,
		barWidth,
		(i32)monster->energy,
		(i32)monster->stats.maxEnergy,
		ColorsBlue
	);
	const f32 initiativeBarHeight = 2.f;
	const Rectangle initiativeRect = {
		.x = monsterStatsRect.x,
		.y = monsterStatsRect.y + monsterStatsRect.height - initiativeBarHeight,
		.width = monsterStatsRect.width,
		.height = initiativeBarHeight,
	};
	ui_draw_progress_bar(
		initiativeRect,
		monster->initiative,
		(f32)MonsterMaxInitiative,
		gameColors[ColorsBlack],
		gameColors[ColorsWhite],
		0
	);
}

static void draw_monster_stat_bar(Vector2 textPos, f32 barWidth, i32 value, i32 maxValue, Colors barColor) {
	const char *barText = TextFormat("%d/%d", value, maxValue);
	const Vector2 barTextSize = MeasureTextEx(assets.fonts.small.rFont, barText, assets.fonts.regular.size, 1.f);
	const Rectangle barTextRect = {
		.x = textPos.x,
		.y = textPos.y,
		.width = barTextSize.x,
		.height = barTextSize.y,
	};
	DrawTextEx(
		assets.fonts.small.rFont,
		barText,
		(Vector2){barTextRect.x, barTextRect.y},
		assets.fonts.small.size,
		1.0f,
		gameColors[ColorsBlack]
	);
	const Rectangle progressBarRect = {
		.x = barTextRect.x,
		.y = barTextRect.y + barTextRect.height - 2.f,
		.width = barWidth,
		.height = 4.f,
	};
	ui_draw_progress_bar(
		progressBarRect,
		(f32)value,
		(f32)maxValue,
		gameColors[barColor],
		gameColors[ColorsBlack],
		2
	);
}

static void draw_ui() {
	if (state.currentMonster.monster == nil) {
		return;
	}

	switch (state.uiBattleChoiceState.uiSelectionMode) {
		case SelectionModeGeneral: {
			draw_general_ui();
			break;
		}
		case SelectionModeAttack: {
			draw_attacks_ui();
			break;
		}
		case SelectionModeSwitch: {
			draw_switch_ui();
			break;
		}
		case SelectionModeTarget:break;
		case SelectionModeNone:
		case SelectionModeCount:
		default: {
			break;
		}
	}
}

static void draw_battle_icon(Texture2D texture, Vector2 pos, bool grayscale) {
	if (grayscale) { BeginShaderMode(assets.shaders.grayscale); }
	DrawTextureV(texture, pos, WHITE);
	if (grayscale) { EndShaderMode(); }
}

static void draw_general_ui() {
	Vector2 fightIconOffset = {30, -60};
	Vector2 defendIconOffset = {40, -20};
	Vector2 switchIconOffset = {40, 20};
	Vector2 catchIconOffset = {30, 60};
	if (game.battleStage.battleType == BattleTypeTrainer) {
		fightIconOffset = (Vector2){30, -40};
		defendIconOffset = (Vector2){40, 0};
		switchIconOffset = (Vector2){30, 40};
		catchIconOffset = Vector2Zero();
	}

	Vector2 currentMonsterMidRightPos = rectangle_mid_right(state.currentMonsterRect);
	Rectangle fightIconRect = rectangle_with_center_at(
		rectangle_from_texture(assets.uiIcons.sword),
		Vector2Add(fightIconOffset, currentMonsterMidRightPos)
	);
	Rectangle defendIconRect = rectangle_with_center_at(
		rectangle_from_texture(assets.uiIcons.shield),
		Vector2Add(defendIconOffset, currentMonsterMidRightPos)
	);
	Rectangle switchIconRect = rectangle_with_center_at(
		rectangle_from_texture(assets.uiIcons.arrows),
		Vector2Add(switchIconOffset, currentMonsterMidRightPos)
	);
	Rectangle catchIconRect = rectangle_with_center_at(
		rectangle_from_texture(assets.uiIcons.hand),
		Vector2Add(catchIconOffset, currentMonsterMidRightPos)
	);

	uiBattleChoiceIconID_ selectedIcon = state.uiBattleChoiceState.indexes[SelectionModeGeneral];
	Texture2D fightIconTexture = selectedIcon == UIBattleChoiceIconIDFight ?
		assets.uiIcons.swordHighlight : assets.uiIcons.sword;
	Texture2D defendIconTexture = selectedIcon == UIBattleChoiceIconIDDefend ?
		assets.uiIcons.shieldHighlight : assets.uiIcons.shield;
	Texture2D switchIconTexture = selectedIcon == UIBattleChoiceIconIDSwitch ?
		assets.uiIcons.arrowsHighlight : assets.uiIcons.arrows;
	Texture2D catchIconTexture = selectedIcon == UIBattleChoiceIconIDCatch ?
		assets.uiIcons.handHighlight : assets.uiIcons.hand;

	draw_battle_icon(
		fightIconTexture,
		rectangle_location(fightIconRect),
		selectedIcon != UIBattleChoiceIconIDFight
	);
	draw_battle_icon(
		defendIconTexture,
		rectangle_location(defendIconRect),
		selectedIcon != UIBattleChoiceIconIDDefend
	);
	draw_battle_icon(
		switchIconTexture,
		rectangle_location(switchIconRect),
		selectedIcon != UIBattleChoiceIconIDSwitch
	);
	if (game.battleStage.battleType != BattleTypeTrainer) {
		draw_battle_icon(
			catchIconTexture,
			rectangle_location(catchIconRect),
			selectedIcon != UIBattleChoiceIconIDCatch
		);
	}
}

static void draw_attacks_ui() {
	if (state.uiBattleChoiceState.uiSelectionMode != SelectionModeAttack) { return; }
	panicIfNil(state.currentMonster.monster);

	const i32 selectedIndex = state.uiBattleChoiceState.indexes[SelectionModeAttack];
	const Rectangle listBgRect = rectangle_with_mid_left_at(
		(Rectangle){.width = 150, .height = 200},
		Vector2Add(rectangle_mid_right(state.currentMonsterRect), (Vector2){20, 0})
	);

	const i32 visibleAttacks = 4;
	const f32 itemHeight = listBgRect.height / (f32)visibleAttacks;
	const f32 itemRadius = 0.05f;
	const f32 tableOffset = selectedIndex < visibleAttacks ?
		0 : -((f32)(selectedIndex - visibleAttacks + 1)) * itemHeight;
	DrawRectangleRounded(listBgRect, itemRadius, 1, gameColors[ColorsWhite]);

	const MonsterData *currentMonsterData = game_data_for_monster_id(state.currentMonster.monster->id);
	for (i32 i = 0; i < currentMonsterData->abilitiesLen; i++) {
		if (state.currentMonster.monster->level < currentMonsterData->abilities[i].level) { continue; }

		// rect
		const Vector2 offset = {
			.x = 0,
			.y = (f32)i * itemHeight + tableOffset,
		};

		// text
		const MonsterAbilityData *abilityData = game_data_for_monster_attack_id(currentMonsterData->abilities[i].ability);
		const Rectangle attackTextRect = rectangle_move_by(
			(Rectangle){.width = listBgRect.width, .height = itemHeight},
			Vector2Add(rectangle_location(listBgRect), offset)
		);
		if (!CheckCollisionPointRec(rectangle_center(attackTextRect), listBgRect)) {
			continue;
		}
		const char *abilityText = monsterAbilityStr[abilityData->id];
		const Rectangle textRect = text_rectangle_centered_at(
			abilityText,
			assets.fonts.regular,
			rectangle_center(attackTextRect)
		);

		Color abilityBGColor;
		if (abilityData->element == MonsterTypeNormal) {
			abilityBGColor = gameColors[ColorsGray];
		} else {
			abilityBGColor = monster_type_color(abilityData->element);
		}

		bool isSelected = i == selectedIndex;
		if (isSelected) {
			DrawRectangleRounded(attackTextRect, itemRadius, 1, gameColors[ColorsDarkWhite]);
		}
		DrawTextEx(
			assets.fonts.regular.rFont,
			abilityText,
			rectangle_location(textRect),
			assets.fonts.regular.size,
			1.f,
			isSelected ? abilityBGColor : gameColors[ColorsLight]
		);

		const bool isReady = state.currentMonster.monster->energy >= abilityData->cost;
		DrawRectangleRec(
			rectangle_at(
				(Rectangle){.width = attackTextRect.width * .1f, .height = attackTextRect.height},
				rectangle_location(attackTextRect)
			),
			isReady ? color_with_alpha(gameColors[ColorsPlant], 100) : color_with_alpha(
				gameColors[ColorsLightGray],
				100
			)
		);
	}
}

// cool thing is if we can pass in some sort of "view" object (maybe a texture?, gotta see the performance
// implications with that) as a row, we can use this as a table view.

static void draw_switch_ui() {
	if (state.uiBattleChoiceState.uiSelectionMode != SelectionModeSwitch) { return; }

	const i32 selectedIndex = state.uiBattleChoiceState.indexes[SelectionModeSwitch];
	const Rectangle listBodyRect = rectangle_with_mid_left_at(
		(Rectangle){.width = 300, .height = 320},
		Vector2Add(rectangle_mid_right(state.currentMonsterRect), (Vector2){20, 0})
	);

	const i32 visibleRows = 4;
	const f32 itemHeight = listBodyRect.height / (f32)visibleRows;
	const f32 itemRadius = 0.05f;
	const f32 tableOffset = selectedIndex < visibleRows ?
		0 : (f32)(-(selectedIndex - visibleRows + 1)) * itemHeight;
	DrawRectangleRounded(listBodyRect, itemRadius, 1, gameColors[ColorsWhite]);

	i32 availableMonsterCount = 0;
	const Monster *availableMonsters[MAX_PARTY_MONSTERS_LEN] = {};
	for (usize i = 0; i < static_array_len(game.playerMonsters); i++) {
		const Monster *monster = &game.playerMonsters[i];
		if (monster->id == MonsterIDNone || monster->health <= 0 || player_monster_in_stage(monster)) { continue; }
		availableMonsters[availableMonsterCount] = monster;
		availableMonsterCount++;
	}

	f32 largestMonsterIconWidth = 0;
	for (i32 i = 0; i < availableMonsterCount; i++) {
		largestMonsterIconWidth = max(
			monster_icon_texture_for_id(availableMonsters[i]->id).width,
			largestMonsterIconWidth
		);
	}

	for (i32 i = 0; i < availableMonsterCount; i++) {
		const Monster *monster = availableMonsters[i];
		panicIf(monster->id == MonsterIDNone || monster->health <= 0 || player_monster_in_stage(monster));

		// rect
		const Vector2 offset = {
			.x = 0,
			.y = (f32)i * itemHeight + tableOffset,
		};

		// text
		const Rectangle monsterRowRect = rectangle_move_by(
			(Rectangle){.width = listBodyRect.width, .height = itemHeight},
			Vector2Add(rectangle_location(listBodyRect), offset)
		);
		if (!CheckCollisionPointRec(rectangle_center(monsterRowRect), listBodyRect)) {
			continue;
		}

		const Texture2D monsterIcon = monster_icon_texture_for_id(monster->id);
		const f32 monsterIconPadding = 10.f + ((largestMonsterIconWidth - (f32)monsterIcon.width) / 2);
		Rectangle monsterIconRect = rectangle_from_texture(monsterIcon);
		monsterIconRect = rectangle_with_mid_left_at(monsterIconRect, rectangle_mid_left(monsterRowRect));
		monsterIconRect = rectangle_move_by(
			monsterIconRect,
			(Vector2){
				.x = monsterIconPadding,
				.y = 0,
			}
		);
		bool isSelected = (i32)i == selectedIndex;
		if (isSelected) {
			DrawRectangleRounded(monsterRowRect, itemRadius, 1, gameColors[ColorsDarkWhite]);
		}
		DrawTexturePro(
			monsterIcon,
			rectangle_from_texture(monsterIcon),
			monsterIconRect,
			Vector2Zero(),
			0.f,
			WHITE
		);

		const char *rowText = TextFormat("%s (Lv. %d)", monster->name, monster->level);
		const Rectangle textRect = text_rectangle_at(
			rowText,
			assets.fonts.regular,
			Vector2Add(rectangle_top_right(monsterIconRect), (Vector2){monsterIconPadding, 0})
		);

		DrawTextEx(
			assets.fonts.regular.rFont,
			rowText,
			rectangle_location(textRect),
			assets.fonts.regular.size,
			1.f,
			isSelected ? gameColors[ColorsRed] : gameColors[ColorsBlack]
		);
		const Rectangle healthRect = rectangle_at(
			(Rectangle){.width = 100, .height = 4},
			Vector2Add(rectangle_bottom_left(textRect), (Vector2){0, 4})
		);
		ui_draw_progress_bar(
			healthRect,
			(f32)monster->health,
			monster->stats.maxHealth,
			gameColors[ColorsRed],
			gameColors[ColorsBlack],
			0.3f
		);
		const Rectangle energyRect = rectangle_at(
			(Rectangle){.width = 100, .height = 4},
			Vector2Add(rectangle_bottom_left(healthRect), (Vector2){0, 2})
		);
		ui_draw_progress_bar(
			energyRect,
			(f32)monster->energy,
			monster->stats.maxEnergy,
			gameColors[ColorsBlue],
			gameColors[ColorsBlack],
			0.3f
		);
	}
}

bool player_monster_in_stage(const Monster *monster) {
	for (usize i = 0; i < static_array_len(state.playerActiveMonsters); i++) {
		const Monster *m = state.playerActiveMonsters[i];
		if (monster == m) {
			return true;
		}
	}
	return false;
}

