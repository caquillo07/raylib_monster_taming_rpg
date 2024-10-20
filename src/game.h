//
// Created by Hector Mejia on 5/4/24.
//

#ifndef RAYLIB_POKEMON_CLONE_GAME_H
#define RAYLIB_POKEMON_CLONE_GAME_H

#include "maps_manager.h"
#include "player.h"
#include "monsters.h"
#include "monster_index.h"
#include "monster_battle.h"

typedef struct GameMetrics {
	f64 timeInInput;
	f64 timeInUpdate;
	f64 timeInDraw;
	i64 totalSprites;
	i64 drawnSprites;
} GameMetrics;

typedef struct DialogBubble {
	i64 index;
	bool visible;
	char characterID[MAX_CHARACTER_ID_LENGTH];
	Vector2 characterCenter;
} DialogBubble;

typedef enum TransitionMode {
	TransitionModeNone = 0,
	TransitionModeFadeOut,
	TransitionModeFadeIn,

	TransitionModeCount,
} TransitionMode;

typedef enum GameModeState {
	GameModeNone = 0,
	GameModeLoading,
	GameModePlaying,
	GameModeMonsterIndex,
	GameModeBattle,

	GameModeCount,
} GameModeState;

typedef struct Game {
	bool isDebug;
	GameModeState gameModeState;

	Map *currentMap;
	Player player;
	Monster playerMonsters[MAX_PARTY_MONSTERS_LEN];
	Camera2D camera;
	Rectangle cameraBoundingBox;
	GameMetrics gameMetrics;
	DialogBubble dialogBubble;

	// transitions
	struct {
		TransitionSprite *target;
		TransitionMode mode;
		f32 progress;
		f32 speed;
	} transition;

	// monsters index
	MonsterIndex monsterIndex;
	BattleStage battleStage;

	struct {
		CharacterData *characterData;
		MonsterData *monsterData;
		MonsterAbilityData *attackData;
	} data;
	bool gameOver;

	struct {
		Timer monsterEncounterTimer;
	} timers;
} Game;

extern Game game;

void game_init();
void game_handle_input();
void game_shutdown();
void game_update(f32 deltaTime);
void game_draw();
void game_start_battle(BattleType battleType, BattleStageBackground bg, Monster *monsters, usize monstersLen);

// general stuff
i32 player_party_length();

#endif //RAYLIB_POKEMON_CLONE_GAME_H
