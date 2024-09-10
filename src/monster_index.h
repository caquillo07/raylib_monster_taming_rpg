//
// Created by Hector Mejia on 8/30/24.
//

#ifndef RAYLIB_POKEMON_CLONE_MONSTER_INDEX_H
#define RAYLIB_POKEMON_CLONE_MONSTER_INDEX_H

#include "common.h"
#include "raylib.h"
#include "sprites.h"

typedef struct MonsterIndex {
	Rectangle frame;
	struct {
		i32 currentIndex;
		i32 selectedIndex;
		i32 partyLength;
		AnimatedTiledSprite animatedMonster;
	} state;
} MonsterIndex;

void monster_index_state_init();
void monster_index_handle_input();
void monster_index_update(f32 dt);
void monster_index_draw();

#endif //RAYLIB_POKEMON_CLONE_MONSTER_INDEX_H
