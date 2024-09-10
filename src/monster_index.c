//
// Created by Hector Mejia on 8/30/24.
//

#include "monster_index.h"
#include "game.h"
#include "colors.h"
#include <raylib.h>
#include <math.h>

void monster_index_state_init() {
	const MonsterIndex state = {
		.frame = {
			.height = (f32)GetScreenHeight(),
			.width = (f32)GetScreenWidth(),
			.x = 0,
			.y = 0,
		},
		.state = {
			.currentIndex = 0,
			.selectedIndex = -1,
			.partyLength = 0,
			.animatedMonster = {
				.entity = {
					.layer = WorldLayerTop,
				},
			}
		},
	};
	game.monsterIndex = state;
}

void monster_index_handle_input() {
	if (game.gameModeState != GameModeMonsterIndex) {
		return;
	}
	if (IsKeyPressed(KEY_ENTER)) {
		game.gameModeState = GameModePlaying;
		game.monsterIndex.state.currentIndex = 0;
		game.monsterIndex.state.selectedIndex = -1;
		return;
	}
	if (IsKeyPressed(KEY_UP)) {
		game.monsterIndex.state.currentIndex -= 1;
		if (game.monsterIndex.state.currentIndex < 0) {
			game.monsterIndex.state.currentIndex = game.monsterIndex.state.partyLength - 1;
		}
	}
	if (IsKeyPressed(KEY_DOWN)) {
		game.monsterIndex.state.currentIndex = (game.monsterIndex.state.currentIndex + 1) %
											   game.monsterIndex.state.partyLength;
	}
	if (IsKeyPressed(KEY_SPACE)) {
		if (game.monsterIndex.state.selectedIndex == -1) {
			game.monsterIndex.state.selectedIndex = game.monsterIndex.state.currentIndex;
		} else {
			const Monster selectedMonster = game.playerMonsters[game.monsterIndex.state.selectedIndex];
			const Monster currentMonster = game.playerMonsters[game.monsterIndex.state.currentIndex];

			game.playerMonsters[game.monsterIndex.state.selectedIndex] = currentMonster;
			game.playerMonsters[game.monsterIndex.state.currentIndex] = selectedMonster;
			game.monsterIndex.state.selectedIndex = -1;
		}
	}
}

void monster_index_update(f32 dt) {
	if (game.gameModeState != GameModeMonsterIndex) {
		return;
	}
	update_animated_tiled_sprite(&game.monsterIndex.state.animatedMonster, dt);
}

static Texture2D monster_icon_texture_for_id(MonsterID id) {
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

static const u8 visibleItems = 6;

static void draw_monsters_list(Rectangle *menuRect, const f32 listWidth, const f32 itemHeight, const f32 tableOffset) {
	for (i32 i = 0; i < game.monsterIndex.state.partyLength; i++) {
		const f32 top = roundf((*menuRect).y + (f32)i * itemHeight + tableOffset);
		Rectangle menuItemRect = {
			.x = (*menuRect).x,
			.y = top,
			.height = itemHeight,
			.width = listWidth,
		};
		Color textColor = gameColors[ColorsWhite];
		Color backgroundColor = gameColors[ColorsGray];
		if (game.monsterIndex.state.currentIndex == i) {
			textColor = gameColors[ColorsDark];
			backgroundColor = gameColors[ColorsLight];
		}
		if (game.monsterIndex.state.selectedIndex == i) {
			textColor = gameColors[ColorsGold];
			backgroundColor = gameColors[ColorsLight];
		}
		if (CheckCollisionRecs(menuItemRect, (*menuRect))) {
			const Monster monster = game.playerMonsters[i];
			if (monster.id == MonsterIDNone) { continue; } // empty slots

			// monster box
			const f32 menuItemRectMidLeft = menuItemRect.y + (menuItemRect.height / 2);
			const f32 menuItemCornerRadius = 12;
			if (CheckCollisionPointRec((Vector2){.x = (*menuRect).x, .y = (*menuRect).y + 1}, menuItemRect)) {
				// this box is at the top
				DrawCircle(
					(i32)(menuItemRect.x + menuItemCornerRadius),
					(i32)(menuItemRect.y + menuItemCornerRadius),
					menuItemCornerRadius,
					backgroundColor
				);
				DrawRectangle(
					(i32)(menuItemRect.x + menuItemCornerRadius),
					(i32)(menuItemRect.y),
					(i32)(menuItemRect.width - menuItemCornerRadius),
					(i32)(menuItemRect.height),
					backgroundColor
				);
				DrawRectangle(
					(i32)(menuItemRect.x),
					(i32)(menuItemRect.y + menuItemCornerRadius),
					(i32)(menuItemRect.width),
					(i32)(menuItemRect.height - menuItemCornerRadius),
					backgroundColor
				);
			} else if (CheckCollisionPointRec(
				// - 1 to make sure it actually hits
				(Vector2){.x = (*menuRect).x, .y = (*menuRect).y + (*menuRect).height - 1},
				menuItemRect
			)) {
				DrawCircle(
					(i32)(menuItemRect.x + menuItemCornerRadius),
					(i32)(menuItemRect.y - menuItemCornerRadius + menuItemRect.height),
					menuItemCornerRadius,
					backgroundColor
				);
				DrawRectangle(
					(i32)(menuItemRect.x),
					(i32)(menuItemRect.y),
					(i32)(menuItemRect.width),
					(i32)(menuItemRect.height - menuItemCornerRadius),
					backgroundColor
				);
				DrawRectangle(
					(i32)(menuItemRect.x + menuItemCornerRadius),
					(i32)(menuItemRect.y),
					(i32)(menuItemRect.width - menuItemCornerRadius),
					(i32)(menuItemRect.height),
					backgroundColor
				);

			} else {
				DrawRectangleRec(menuItemRect, backgroundColor);
			}

			// text
			const f32 textPadding = 90.f;
			const Vector2 textSize = MeasureTextEx(assets.regularFont, monster.name, 18, 1);
			const Vector2 pos = {
				.x = menuItemRect.x + textPadding,
				.y = menuItemRectMidLeft - (textSize.y / 2),
			};
			DrawTextEx(
				assets.regularFont,
				monster.name,
				pos,
				18,
				1,
				textColor
			);

			// monster icon
			const f32 monsterIconPadding = 45.f;
			Texture2D monsterIconTexture = monster_icon_texture_for_id(monster.id);
			const Rectangle monsterIconFrame = {
				.height = (f32)monsterIconTexture.height,
				.width = (f32)monsterIconTexture.width,
			};
			const Vector2 monsterIconPos = {
				.x = menuItemRect.x + monsterIconPadding - ((f32)monsterIconTexture.width / 2.f),
				.y = menuItemRectMidLeft - ((f32)monsterIconTexture.height / 2.f),
			};

			DrawTextureRec(monsterIconTexture, monsterIconFrame, monsterIconPos, WHITE);
		}
	}

	const i32 linesMax = min(visibleItems, game.monsterIndex.state.partyLength);
	for (i32 i = 1; i < linesMax; i++) {
		const f32 y = menuRect->y + itemHeight * (f32)i;
		Vector2 start = {.x = menuRect->x, .y = y};
		Vector2 end = {.x = menuRect->x + listWidth, .y = y};
		DrawLineV(start, end, gameColors[ColorsLightGray]);
	}
}

void monster_index_draw() {
	if (game.gameModeState != GameModeMonsterIndex) {
		return;
	}
	Color c = {0, 0, 0, 200};

	// background
	DrawRectangleRec(game.monsterIndex.frame, c);
	// draw the menu in the center of the screen
	Rectangle menuRect = {
		.height = roundf(0.8f * game.monsterIndex.frame.height),
		.width = roundf(0.6f * game.monsterIndex.frame.width),
	};
	menuRect.x = roundf((game.monsterIndex.frame.width - menuRect.width) / 2);
	menuRect.y = roundf((game.monsterIndex.frame.height - menuRect.height) / 2);

	const f32 listWidth = roundf(menuRect.width * 0.3f);
	const f32 itemHeight = roundf(menuRect.height / (f32)visibleItems);
	const i32 indexesBelowDisplayed = game.monsterIndex.state.currentIndex - visibleItems + 1;
	const f32 tableOffset = game.monsterIndex.state.currentIndex < visibleItems ?
		0 :
		-(f32)indexesBelowDisplayed * itemHeight;

	// because of rounding issues, we need to update menuRect to make sure all
	// other calculations match the height of all the monster item cells.
	menuRect.height = itemHeight * (f32)visibleItems;

	// monsters list
	draw_monsters_list(&menuRect, listWidth, itemHeight, tableOffset);

	// separator shadow
	const Rectangle shadowBorder = {
		.x = menuRect.x + listWidth - 4,
		.y = menuRect.y,
		.width = 4,
		.height = menuRect.height,
	};
	DrawRectangleRec(shadowBorder, (Color){0, 0, 0, 100});

	// the detail view
	const Color detailViewBGColor = gameColors[ColorsDark];
	const f32 detailViewCornerRadius = 12;
	// the top right circle

	// actual background
	const Rectangle detailRec = {
		.x = menuRect.x + listWidth,
		.y = menuRect.y,
		.width = menuRect.width - listWidth,
		.height = menuRect.height,
	};
	DrawCircle(
		(i32)(detailRec.x + detailRec.width - detailViewCornerRadius),
		(i32)(detailRec.y + detailViewCornerRadius),
		detailViewCornerRadius,
		detailViewBGColor
	);
	DrawCircle(
		(i32)(detailRec.x + detailRec.width - detailViewCornerRadius),
		(i32)(detailRec.y + detailRec.height - detailViewCornerRadius),
		detailViewCornerRadius,
		detailViewBGColor
	);
	DrawRectangle(
		(i32)(detailRec.x),
		(i32)(detailRec.y),
		(i32)(detailRec.width - detailViewCornerRadius),
		(i32)(detailRec.height),
		detailViewBGColor
	);
	DrawRectangle(
		(i32)(detailRec.x),
		(i32)(detailRec.y + detailViewCornerRadius),
		(i32)(detailRec.width),
		(i32)(detailRec.height - detailViewCornerRadius * 2),
		detailViewBGColor
	);

	// monster display
	const Monster currentSelectedMonster = game.playerMonsters[game.monsterIndex.state.currentIndex];
	Color monsterBGColor;
	switch (currentSelectedMonster.type) {
		case MonsterTypePlant: monsterBGColor = gameColors[ColorsPlant];
			break;
		case MonsterTypeWater: monsterBGColor = gameColors[ColorsWater];
			break;
		case MonsterTypeFire: monsterBGColor = gameColors[ColorsFire];
			break;
		case MonsterTypeCount:
		case MonsterTypeNone:
		default: panic("invalid monster type");
	}
	const Rectangle monsterDisplayRect = {
		.x = detailRec.x,
		.y = detailRec.y,
		.width = detailRec.width,
		.height = detailRec.height * 0.4f,
	};
	DrawRectangleRec(monsterDisplayRect, monsterBGColor);

	const Monster currentMonster = game.playerMonsters[game.monsterIndex.state.currentIndex];
	game.monsterIndex.state.animatedMonster.texture = assets.monsterTileMaps[currentMonster.id].texture;
	const AnimatedTiledSprite *animatedSprite = &game.monsterIndex.state.animatedMonster;
	const Rectangle animationFrame = animatedSprite->sourceFrames[animatedSprite->currentFrame];
	const Vector2 monsterPosition = (Vector2){
		.x = monsterDisplayRect.x + monsterDisplayRect.width / 2 - animationFrame.width / 2,
		.y = monsterDisplayRect.y + monsterDisplayRect.height / 2 - animationFrame.height / 2,
	};
	DrawTextureRec(
		animatedSprite->texture,
		animationFrame,
		monsterPosition,
		WHITE
	);

	// monster name and level
	const Vector2 monsterNamePos = {.x = monsterDisplayRect.x + 10, .y = monsterDisplayRect.y + 10};
	DrawTextEx(
		assets.boldFont,
		currentMonster.name,
		monsterNamePos,
		14,
		1,
		gameColors[ColorsWhite]
	);

	const i32 levelTextLen = 16;
	char levelText[levelTextLen];
	snprintf(levelText, levelTextLen, "Lv. %3d", currentMonster.level);
	f32 levelTextFontSize = 24;
	f32 levelTextSpacing = 1;

	const f32 levelTextHeight = MeasureTextEx(
		assets.regularFont,
		levelText,
		levelTextFontSize,
		levelTextSpacing
	).y;
	const Vector2 monsterLevelPos = {
		.x = monsterDisplayRect.x + 10,
		.y = monsterDisplayRect.y + monsterDisplayRect.height - levelTextHeight - 10, // 10 padding
	};
	DrawTextEx(
		assets.regularFont,
		levelText,
		monsterLevelPos,
		levelTextFontSize,
		levelTextSpacing,
		gameColors[ColorsWhite]
	);

	const char *monsterTypeText = monsterTypeStr[currentMonster.type];
	const Vector2 typeTextSize = MeasureTextEx(
		assets.regularFont,
		monsterTypeText,
		levelTextFontSize,
		levelTextSpacing
	);

	const Vector2 monsterElementPos = {
		.x = monsterDisplayRect.x + monsterDisplayRect.width - typeTextSize.x - 10,
		.y = monsterDisplayRect.y + monsterDisplayRect.height - typeTextSize.y - 10, // 10 padding
	};
	DrawTextEx(
		assets.regularFont,
		monsterTypeText,
		monsterElementPos,
		levelTextFontSize,
		levelTextSpacing,
		gameColors[ColorsWhite]
	);

	// monster stats
//	const Rectangle monsterStatsRect = {
//		.x = monsterDisplayRect.x,
//		.y = monsterDisplayRect.y + monsterDisplayRect.height,
//		.width = detailRec.width,
//		.height = detailRec.height - monsterDisplayRect.height,
//	};
//	DrawRectangleRec(monsterStatsRect, BLUE);
}