//
// Created by Hector Mejia on 9/30/24.
//

#include "monster_battle.h"
#include "game.h"
#include "settings.h"
#include "colors.h"
#include "ui.h"

static void draw_monster(const Monster *monster, const AnimatedTiledSprite *sprite, Vector2 pos, bool flipped);
static void draw_monster_stat_bar(Vector2 textPos, f32 barWidth, i32 value, i32 maxValue, Colors barColor);

struct monsterPosition_ {
	Vector2 top;
	Vector2 center;
	Vector2 bottom;
};

// todo - make dynamic with screen size
const struct battlePositions_ {
	struct monsterPosition_ left;
	struct monsterPosition_ right;
} battlePositions = {
	.left = {
		.top = {360, 260},
		.center = {190, 400},
		.bottom = {410, 520},
	},
	.right = {
		.top = {900, 260},
		.center = {1110, 390},
		.bottom = {900, 550},
	},
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
		game.battleStage.playerMonsterSprites[i] = monster_get_animated_sprite_for_id(
			game.playerMonsters[i].id
		);
		game.battleStage.playerMonsterSprites[i].animationSpeed *= rand_f32(minAnimSpeed, maxAnimSpeed);
	}

	// opponent side
	for (i32 i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
		panicIf(game.battleStage.opponentMonsters[i].id == MonsterIDNone && i == 0);
		if (game.battleStage.opponentMonsters[i].id == MonsterIDNone) {
			continue;
		}
		game.battleStage.opponentMonsterSprites[i] = monster_get_animated_sprite_for_id(
			game.battleStage.opponentMonsters[i].id
		);
		game.battleStage.opponentMonsterSprites[i].animationSpeed *= rand_f32(minAnimSpeed, maxAnimSpeed);
	}
}

void monster_battle_input() {
	if (game.gameModeState != GameModeBattle) { return; }
}

void monster_battle_update(f32 dt) {
	if (game.gameModeState != GameModeBattle) { return; }

	for (i32 i = 0; i < MAX_MONSTERS_PER_SIDE_LEN; i++) {
		animated_tiled_sprite_update(&game.battleStage.playerMonsterSprites[i], dt);
		animated_tiled_sprite_update(&game.battleStage.opponentMonsterSprites[i], dt);
	}
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
	draw_monster(&game.playerMonsters[0], &game.battleStage.playerMonsterSprites[0], battlePositions.left.top, true);
	draw_monster(&game.playerMonsters[1], &game.battleStage.playerMonsterSprites[1], battlePositions.left.center, true);
	draw_monster(&game.playerMonsters[2], &game.battleStage.playerMonsterSprites[2], battlePositions.left.bottom, true);

	draw_monster(
		&game.battleStage.opponentMonsters[0],
		&game.battleStage.opponentMonsterSprites[0],
		battlePositions.right.top,
		false
	);
	draw_monster(
		&game.battleStage.opponentMonsters[1],
		&game.battleStage.opponentMonsterSprites[1],
		battlePositions.right.center,
		false
	);
	draw_monster(
		&game.battleStage.opponentMonsters[2],
		&game.battleStage.opponentMonsterSprites[2],
		battlePositions.right.bottom,
		false
	);
}

static void draw_monster(const Monster *monster, const AnimatedTiledSprite *sprite, Vector2 pos, bool flipped) {
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
	const Vector2 monsterNameSize = MeasureTextEx(assets.regularFont.font, monsterName, assets.regularFont.size, 1.0f);
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
		assets.regularFont.font,
		monsterName,
		monsterNamePos,
		assets.regularFont.size,
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
		assets.smallFont.font,
		monsterLevelText,
		monsterLevelPos,
		assets.smallFont.size,
		1.0f,
		gameColors[ColorsBlack]
	);

	// draw the monster AFTER the name/level
	DrawTexturePro(
		sprite->texture,
		animationFrame,
		monsterDestRec,
		(Vector2){0, 0},
		0.f,
		WHITE
	);

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

	// health and energy (probably abstracted too soon here...?
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
		(i32)monster->health,
		(i32)monster->stats.maxHealth,
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
		(f32)monster->initiative,
		(f32)MonsterMaxInitiative,
		gameColors[ColorsBlack],
		gameColors[ColorsWhite],
		0
	);
}

static void draw_monster_stat_bar(Vector2 textPos, f32 barWidth, i32 value, i32 maxValue, Colors barColor) {
	const char *barText = TextFormat("%d/%d", value, maxValue);
	const Vector2 barTextSize = MeasureTextEx(assets.smallFont.font, barText, assets.regularFont.size, 1.f);
	const Rectangle barTextRect = {
		.x = textPos.x,
		.y = textPos.y,
		.width = barTextSize.x,
		.height = barTextSize.y,
	};
	DrawTextEx(
		assets.smallFont.font,
		barText,
		(Vector2){barTextRect.x, barTextRect.y},
		assets.smallFont.size,
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
