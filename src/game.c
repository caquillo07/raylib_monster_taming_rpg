//
// Created by Hector Mejia on 5/4/24.
//

#include "game.h"

#include <tmx_utils.h>

#include "common.h"
#include "raylib.h"
#include "maps_manager.h"
#include "assets.h"
#include "colors.h"
#include "game_data.h"
#include "settings.h"
#include "array/array.h"
#include "monster_battle.h"

//
static void setup_game(MapID mapID);
static void update_camera();
static void game_draw_dialog_box();
static void game_draw_debug_camera();
static void game_draw_debug_screen();
static void do_map_transition_check();
static void handle_screen_transition(f32 dt);
static void game_draw_fade_transition();
static void game_load_map(MapID mapID);
static void game_start_battle(BattleType battleType);

MapID startingMap = MapIDWorld;

static bool frameStepMode = false;
static bool shouldRenderFrame = true;

Game game;

void game_init() {
	game = (Game){};

	maps_manager_init();
	load_assets();
	load_shaders();
	game_data_init();

	setup_game(startingMap);
}

void game_shutdown() {
	map_free(game.currentMap);
	game.currentMap = nil;

	player_free(&game.player);

	unload_assets();
	unload_shaders();
	game_data_free();
}

static void do_game_handle_input() {
	if (game.isDebug && IsKeyPressed(KEY_F1)) {
		frameStepMode = !frameStepMode;
		shouldRenderFrame = false;
		return;
	}

	if (game.isDebug && frameStepMode && IsKeyPressed(KEY_SPACE)) {
		shouldRenderFrame = true;
		return;
	}

	if (IsKeyPressed(KEY_F2)) {
		game.isDebug = !game.isDebug;
		frameStepMode = false;
		shouldRenderFrame = true;
		return;
	}

	if (frameStepMode && !shouldRenderFrame) {
		return;
	}

	switch (game.gameModeState) {
		case GameModeNone:
		case GameModeLoading: break;
		case GameModePlaying: {
			// todo if any of the events happens above, we never update the player input.
			//  we should instead hold a list of events on this frame and check them on each system
			//  instead. add the key press to the global event
			//  https://github.com/raysan5/raylib/blob/52f2a10db610d0e9f619fd7c521db08a876547d0/src/rcore.c#L297
			player_input(&game.player);
			if (IsKeyPressed(KEY_ENTER)) {
				game.gameModeState = GameModeMonsterIndex;
				game.monsterIndex.state.partyLength = player_party_length();

				const i32 currentIndex = game.monsterIndex.state.currentIndex;
				const Monster currenMonster = game.playerMonsters[currentIndex];
				game.monsterIndex.state.animatedMonster = monster_get_idle_animated_sprite_for_id(currenMonster.id);
				game.player.characterComponent.velocity = (Vector2){0, 0};
			}
			break;
		}
		case GameModeMonsterIndex: {
			monster_index_handle_input();
			break;
		}
		case GameModeBattle: {
			monster_battle_input();
			break;
		}
		case GameModeCount:
		default: panic("invalid game mode state: %d", game.gameModeState);
	}
}

void game_handle_input() {
	static i32 screenHeight = 0;
	static i32 screenWidth = 0;
	if (GetScreenHeight() != screenHeight || GetScreenWidth() != screenWidth) {
		screenHeight = GetScreenHeight();
		screenWidth = GetScreenWidth();
		printf("screen size changed to %dx%d\n", screenWidth, screenHeight);
	}
	const clock_t now = clock();
	do_game_handle_input();
	game.gameMetrics.timeInInput = ((double)(clock() - now)) / (CLOCKS_PER_SEC / 1000);
}

static void do_game_update(const f32 deltaTime) {
	if (frameStepMode && !shouldRenderFrame) {
		return;
	}
	do_map_transition_check();
	map_update(game.currentMap, deltaTime);
	player_update(&game.player, deltaTime);
	monster_index_update(deltaTime);
	monster_battle_update(deltaTime);

	update_camera();
	handle_screen_transition(deltaTime);
}

void game_update(const f32 deltaTime) {
	const clock_t now = clock();
	do_game_update(deltaTime);
	game.gameMetrics.timeInUpdate = ((double)(clock() - now)) / (CLOCKS_PER_SEC / 1000);
}

void game_draw() {
	const clock_t now = clock();
	BeginDrawing();
	{
		ClearBackground(DARKGRAY);
		BeginMode2D(game.camera);
		{
			map_draw(game.currentMap);
			// player_draw(game.player);
			game_draw_dialog_box();
			game_draw_debug_camera();
		}
		EndMode2D();

		game_draw_fade_transition();
		monster_index_draw();
		monster_battle_draw();

		// last thing we draw
		game_draw_debug_screen();
	}
	EndDrawing();
	if (frameStepMode) {
		shouldRenderFrame = false;
	}
	game.gameMetrics.timeInDraw = ((double)(clock() - now)) / (CLOCKS_PER_SEC / 1000);
	game.gameMetrics.drawnSprites = 0;
}

i32 player_party_length() {
	i32 count = 0;
	for (i32 i = 0; i < (i32)static_array_len(game.playerMonsters); i++) {
		if (game.playerMonsters[i].id == MonsterIDNone) {
			break;
		}
		count++;
	}
	return count;
}

static void game_draw_debug_screen() {
	if (!game.isDebug) { return; }

	DrawFPS(10, 10);

	const i32 fontSize = 20;
	const size_t textBufSize = 1024;
	char mousePosText[textBufSize];
	const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), game.camera);
	snprintf(mousePosText, textBufSize, "Mouse %dx%d", (i32)mousePos.x, (i32)mousePos.y);
	DrawText(mousePosText, 12, 30, fontSize, DARKGREEN);

	char gameMetricsText[textBufSize * 3];
	snprintf(
		gameMetricsText,
		textBufSize * 3,
		"Time in input: %0.4f\n"
		"Time in update: %0.4f\n"
		"Time in draw: %0.4f\n"
		"Sprites Drawn: %lld/%lld",
		game.gameMetrics.timeInInput,
		// todo make this static variables inside the functions instead.
		game.gameMetrics.timeInUpdate,
		game.gameMetrics.timeInDraw,
		game.gameMetrics.drawnSprites,
		game.gameMetrics.totalSprites
	);
	const Vector2 textSize = MeasureTextEx(GetFontDefault(), gameMetricsText, (f32)fontSize, 1);
	DrawText(gameMetricsText, 12, (i32)(30.f + textSize.y), 20, DARKBLUE);
}

static void do_map_transition_check() {
	array_range(game.currentMap->transitionBoxes, i) {
		TransitionSprite *transitionSprite = &game.currentMap->transitionBoxes[i];
		const bool collides = CheckCollisionRecs(transitionSprite->box, game.player.characterComponent.hitBox);
		if (collides && game.transition.target == nil) {
			game.transition.target = transitionSprite;
			game.transition.mode = TransitionModeFadeOut;
			game.transition.progress = 0.f;
			game.transition.speed = settings.fadeTransitionSpeed;
		}
	}
}

static void handle_screen_transition(const f32 dt) {
	switch (game.transition.mode) {
		case TransitionModeFadeOut: {
			panicIfNil(game.transition.target);
			player_block(&game.player);
			game.transition.progress += game.transition.speed * dt;
			if (game.transition.progress >= 255) {
				game.transition.mode = TransitionModeFadeIn;
				const MapID nextMapID = map_id_for_name(game.transition.target->destination);
				game_load_map(nextMapID);
			}
			break;
		}
		case TransitionModeFadeIn: {
			panicIfNil(game.transition.target);
			game.transition.progress -= game.transition.speed * dt;
			if (game.transition.progress <= 0) {
				game.transition.mode = TransitionModeNone;
				game.transition.target = nil;
				player_unblock(&game.player);
			}
			break;
		}

		case TransitionModeNone: break;
		default: panic("invalid tint mode while fading screen");
	}
}

static void game_draw_debug_camera() {
	if (!game.isDebug) { return; }

	// Draw camera anchor
	DrawCircleV(game.camera.target, 5.f, BLUE);
}

static void game_draw_fade_transition() {
	if (game.transition.target == nil) {
		return;
	}
	const Rectangle screen = {
		.x = 0,
		.y = 0,
		.height = (f32)GetScreenHeight(),
		.width = (f32)GetScreenWidth(),
	};
	const f32 a = clamp(game.transition.progress, 0, 255);
	printfln("progress: %.2f", a);
	const Color c = {0, 0, 0, (u8)a};
	DrawRectangleRec(screen, c);
}

void game_load_map(const MapID mapID) {
	map_free(game.currentMap);
	game.currentMap = load_map(mapID);
	character_set_center_at(&game.player.characterComponent, game.currentMap->playerStartingPosition);
}

static void game_draw_dialog_box() {
	if (!game.dialogBubble.visible) {
		return;
	}
	const CharacterData *data = game_data_for_character_id(game.dialogBubble.characterID);
	panicIf(!data->defeated && game.dialogBubble.index >= MAX_REGULAR_DIALOG_ENTRIES);
	panicIf(data->defeated && game.dialogBubble.index >= MAX_DEFEATED_DIALOG_ENTRIES);

	const char *msg = data->defeated ?
		data->dialog.defeated[game.dialogBubble.index] :
		data->dialog.regular[game.dialogBubble.index];

	const f32 fontSize = 33.f;
	const f32 fontSpacing = 4.f;
	const Vector2 textSize = MeasureTextEx(assets.fonts.dialog.rFont, msg, fontSize, fontSpacing);
	const f32 textPadding = 20.f;
	const f32 minWidth = 30.f;
	const f32 bubbleWidth = textPadding * 2 + textSize.x;
	const f32 bubbleHeight = textPadding * 2 + textSize.y;

	const f32 radius = 0.3f;
	const f32 padding = 70.f;
	const Rectangle frame = {
		.x = game.dialogBubble.characterCenter.x - (bubbleWidth / 2),
		.y = game.dialogBubble.characterCenter.y - bubbleHeight - padding,
		.height = max(bubbleHeight, minWidth),
		.width = bubbleWidth,
	};

	DrawRectangleRounded(frame, radius, 0, gameColors[ColorsWhite]);
	DrawTextEx(
		assets.fonts.dialog.rFont,
		msg,
		(Vector2){frame.x + textPadding, frame.y + textPadding},
		fontSize,
		fontSpacing,
		gameColors[ColorsBlack]
	);
}

static void setup_game(const MapID mapID) {
	Map *map = load_map(mapID);
	game.gameModeState = GameModeLoading;
	game.currentMap = map;
	game.player = player_new(map->playerStartingPosition);
	game.playerMonsters[0] = monster_new(MonsterIDCharmadillo, 30);
	game.playerMonsters[1] = monster_new(MonsterIDFriolera, 29);
	game.playerMonsters[2] = monster_new(MonsterIDLarvea, 3);
	game.playerMonsters[3] = monster_new(MonsterIDAtrox, 24);
	game.playerMonsters[4] = monster_new(MonsterIDSparchu, 24);
	game.playerMonsters[5] = monster_new(MonsterIDGulfin, 24);
	game.playerMonsters[6] = monster_new(MonsterIDJacana, 2);
	game.playerMonsters[7] = monster_new(MonsterIDPouch, 3);


	// camera
	game.camera = (Camera2D){0};
	update_camera();

	// dialog
	const DialogBubble dialog = {
		.visible = false,
	};
	game.dialogBubble = dialog;

	// index
	monster_index_state_init();

	// start the game;
	game.gameModeState = GameModePlaying;

	// todo - temp
	game_start_battle(BattleTypeWildEncounter);
}

static void update_camera() {
	game.camera.target = player_get_center(&game.player);
	game.camera.offset = (Vector2){
		.x = ((f32)GetScreenWidth() / 2.0f),
		.y = ((f32)GetScreenHeight() / 2.0f),
	};
	game.camera.zoom = (f32)GetScreenHeight() / PixelWindowHeight;
	game.camera.rotation = 0.0f;

	game.cameraBoundingBox = (Rectangle){
		game.camera.target.x - game.camera.offset.x / game.camera.zoom,
		game.camera.target.y - game.camera.offset.y / game.camera.zoom,
		(f32)GetScreenWidth() / game.camera.zoom,
		(f32)GetScreenHeight() / game.camera.zoom
	};
}

void game_start_battle(BattleType battleType) {
	game.battleStage = (BattleStage){
		.opponentMonsters = {
			monster_new(MonsterIDLarvea, 13),
			monster_new(MonsterIDAtrox, 14),
			monster_new(MonsterIDSparchu, 25),
			monster_new(MonsterIDGulfin, 14),
			monster_new(MonsterIDJacana, 12),
		},
		.bgTexture = assets.battleBackgrounds.forrest,
		.battleType = battleType,
	};
	monster_battle_setup();
	game.gameModeState = GameModeBattle;
}
