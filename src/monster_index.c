//
// Created by Hector Mejia on 8/30/24.
//

#include "monster_index.h"
#include "game.h"
#include "colors.h"
#include "ui.h"
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
	game.monsterIndex.frame.height = (f32)GetScreenHeight();
	game.monsterIndex.frame.width = (f32)GetScreenWidth();
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

// todo(hector) make text size dynamic for different screen sizes
static void draw_monsters_list(Rectangle *menuRect, const f32 listWidth, const f32 itemHeight, const f32 tableOffset) {
	for (i32 i = 0; i < max(6,game.monsterIndex.state.partyLength); i++) {
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

			if (monster.id == MonsterIDNone) { continue; } // empty slots

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
			// todo(hector) make size dynamic for different screen sizes
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
	DrawCircle(
		(i32)(detailRec.x + detailRec.width - detailViewCornerRadius),
		(i32)(detailRec.y + detailViewCornerRadius),
		detailViewCornerRadius,
		monsterBGColor
	);
	Rectangle toDrawMonsterDisplayRect = monsterDisplayRect;
	toDrawMonsterDisplayRect.width -= detailViewCornerRadius;
	DrawRectangleRec(toDrawMonsterDisplayRect, monsterBGColor);
	toDrawMonsterDisplayRect = monsterDisplayRect;
	toDrawMonsterDisplayRect.y += detailViewCornerRadius;
	toDrawMonsterDisplayRect.height -= detailViewCornerRadius;
	DrawRectangleRec(toDrawMonsterDisplayRect, monsterBGColor);

	const Monster currentMonster = game.playerMonsters[game.monsterIndex.state.currentIndex];
	game.monsterIndex.state.animatedMonster.texture = assets.monsterTileMaps[currentMonster.id].texture;
	const AnimatedTiledSprite *animatedSprite = &game.monsterIndex.state.animatedMonster;
	const Rectangle animationFrame = animatedSprite->sourceFrames[animatedSprite->currentFrame];
	const f32 monsterToRectHeight = monsterDisplayRect.height * 0.7f;
	const Vector2 monsterPosition = (Vector2){
		.x = monsterDisplayRect.x + monsterDisplayRect.width / 2 - monsterToRectHeight / 2,
		.y = monsterDisplayRect.y + monsterDisplayRect.height / 2 - monsterToRectHeight / 2,
	};
	Rectangle monsterDestRec = {
		.x = monsterPosition.x,
		.y = monsterPosition.y,
		.height = monsterToRectHeight,
		.width = monsterToRectHeight,
	};
	DrawTexturePro(
		animatedSprite->texture,
		animationFrame,
		monsterDestRec,
		(Vector2){0, 0},
		0.f,
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
		.y = monsterDisplayRect.y + monsterDisplayRect.height - levelTextHeight - 16, // 10 padding
	};
	DrawTextEx(
		assets.regularFont,
		levelText,
		monsterLevelPos,
		levelTextFontSize,
		levelTextSpacing,
		gameColors[ColorsWhite]
	);

	// xp bar
	Rectangle xpBarRect = {
		.x = monsterLevelPos.x,
		.y = monsterLevelPos.y + levelTextHeight,
		.height = 4,
		.width = 100,
	};
	ui_draw_progress_bar(
		xpBarRect,
		(f32)currentMonster.xp,
		(f32)currentMonster.levelUp,
		gameColors[ColorsWhite],
		gameColors[ColorsDark],
		1.f
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

	// main part - monster stats
	const f32 healthBarPadding = 15.f;
	const Rectangle healthBarRect = {
		.x = detailRec.x + healthBarPadding - shadowBorder.width,
		.y = monsterDisplayRect.y + monsterDisplayRect.height + healthBarPadding,
		.width = detailRec.width * 0.45f,
		.height = 30,
	};
	const f32 maxHealth = currentMonster.stats.maxHealth * (f32)currentMonster.level;
	ui_draw_progress_bar(
		healthBarRect,
		(f32)currentMonster.health,
		maxHealth,
		gameColors[ColorsRed],
		gameColors[ColorsBlack],
		0.5f
	);

	// HP Text
	const f32 hpTextFontSize = 18;
	const char *hpText = TextFormat("HP: %d/%d", (i32)currentMonster.health, (i32)maxHealth);
	const Vector2 hpTextSize = MeasureTextEx(assets.regularFont, hpText, hpTextFontSize, 1);
	const f32 hpTextPadding = healthBarRect.height / 2 - hpTextSize.y / 2;
	const Vector2 hpTextPos = {
		.x = healthBarRect.x + hpTextPadding,
		.y = healthBarRect.y + hpTextPadding,
	};
	DrawTextEx(
		assets.regularFont,
		hpText,
		hpTextPos,
		hpTextFontSize,
		1,
		gameColors[ColorsWhite]
	);

	// energy rectangle
	const f32 energyBarPadding = 15.f;
	const Rectangle energyBarRect = {
		.x = (detailRec.x + detailRec.width - shadowBorder.width) - healthBarRect.width - energyBarPadding,
		.y = monsterDisplayRect.y + monsterDisplayRect.height + energyBarPadding,
		.width = healthBarRect.width,
		.height = 30,
	};
	const f32 maxEnergy = currentMonster.stats.maxEnergy * (f32)currentMonster.level;
	ui_draw_progress_bar(
		energyBarRect,
		(f32)currentMonster.energy,
		maxEnergy,
		gameColors[ColorsBlue],
		gameColors[ColorsBlack],
		0.5f
	);

	// energy Text
	const f32 energyTextFontSize = 18;
	const char *energyText = TextFormat("EP: %d/%d", (i32)currentMonster.energy, (i32)maxEnergy);
	const Vector2 energyTextSize = MeasureTextEx(assets.regularFont, energyText, energyTextFontSize, 1);
	const f32 energyTextPadding = healthBarRect.height / 2 - energyTextSize.y / 2;
	const Vector2 energyPos = {
		.x = energyBarRect.x + energyTextPadding,
		.y = energyBarRect.y + energyTextPadding,
	};
	DrawTextEx(
		assets.regularFont,
		energyText,
		energyPos,
		energyTextFontSize,
		1,
		gameColors[ColorsWhite]
	);

	// monster info
	const f32 statsRectVerticalPadding = 30.f;
	const Rectangle statsRect = {
		.x = healthBarRect.x,
		.y = healthBarRect.y + healthBarRect.height + statsRectVerticalPadding,
		.width = healthBarRect.width,
		.height = (menuRect.y + menuRect.height) -
				  (healthBarRect.y + healthBarRect.height) -
				  statsRectVerticalPadding * 2,
	};

	const f32 statsTextFontSize = 18;
	const char *statsText = "Stats";
	const Vector2 statsTextSize = MeasureTextEx(assets.regularFont, statsText, statsTextFontSize, 1);
	const Vector2 statsPos = {
		.x = statsRect.x,
		.y = statsRect.y - statsTextSize.y,
	};
	DrawTextEx(
		assets.regularFont,
		statsText,
		statsPos,
		statsTextFontSize,
		1,
		gameColors[ColorsWhite]
	);

	// draw stats
#define MONSTER_STATS_LEN 6
	// not ideal, but meh
	const f32 statHeight = statsRect.height / MONSTER_STATS_LEN;

	// I think the way is done in the tutorial is silly because there is no such thing as
	// "max stat", so having a progress bar to indicate is sort of useless. So I chose to just
	// display the value and call it a day.
	const char *statNames[MONSTER_STATS_LEN] = {
		TextFormat("Energy: %.2f", currentMonster.stats.maxEnergy),
		TextFormat("Health: %.2f", currentMonster.stats.maxHealth),
		TextFormat("Attack: %.2f", currentMonster.stats.attack),
		TextFormat("Defense: %.2f", currentMonster.stats.defense),
		TextFormat("Recovery: %.2f", currentMonster.stats.recovery),
		TextFormat("Speed: %.2f", currentMonster.stats.speed),
	};
	const Texture2D statIcons[MONSTER_STATS_LEN] = {
		assets.uiIcons.energy,
		assets.uiIcons.health,
		assets.uiIcons.attack,
		assets.uiIcons.defense,
		assets.uiIcons.recovery,
		assets.uiIcons.speed,
	};
	const f32 singleStatFontSize = 18;
	for (usize i = 0; i < MONSTER_STATS_LEN; i++) {
		const Rectangle singleStatRect = {
			.x = statsRect.x,
			.y = statsRect.y + ((f32)i * statHeight),
			.width = statsRect.width,
			.height = statHeight,
		};

		const f32 singleStatTextPadding = 5;
		const f32 iconHeight = (f32)statIcons[i].height;
		const Rectangle iconRect = {
			.x = singleStatRect.x + singleStatTextPadding,
			.y = singleStatRect.y + (singleStatRect.height - iconHeight) / 2,
			.height = (f32)statIcons[i].height,
			.width = (f32)statIcons[i].width,
		};
		DrawTextureV(statIcons[i], (Vector2){iconRect.x, iconRect.y}, WHITE);

		const Vector2 singleStatSize = MeasureTextEx(assets.regularFont, statNames[i], singleStatFontSize, 1);
		const Vector2 singleStatTextPos = {
			.x = iconRect.x + iconRect.width + singleStatTextPadding,
			.y = singleStatRect.y + (singleStatRect.height - singleStatSize.y) / 2,
		};
		DrawTextEx(
			assets.regularFont,
			statNames[i],
			singleStatTextPos,
			energyTextFontSize,
			1,
			gameColors[ColorsWhite]
		);
	}

	// monster abilities
}
