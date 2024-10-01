//
// Created by Hector Mejia on 9/30/24.
//

#include "monster_battle.h"
#include "game.h"
#include "settings.h"
#include "colors.h"

static void draw_monster(const Monster *monster, const AnimatedTiledSprite *sprite, Vector2 pos, bool flipped);

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
		.x = pos.x,
		.y = pos.y,
		.height = animationFrame.height,
		.width = animationFrame.width,
	};
	animationFrame.width *= (f32)(flipped ? -1 : 1);
	DrawTexturePro(
		sprite->texture,
		animationFrame,
		monsterDestRec,
		(Vector2){0, 0},
		0.f,
		WHITE
	);

	// monster name/level box
	const char *monsterName = monster->name;
	const f32 monsterNameFontSize = 15.f;
	const f32 monsterNamePadding = 20.f;
	const Vector2 monsterNameSize = MeasureTextEx(assets.regularFont, monsterName, 15.f, 1.0f);
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
	DrawTextEx(assets.regularFont, monsterName, monsterNamePos, monsterNameFontSize, 1.0f, gameColors[ColorsBlack]);
}
