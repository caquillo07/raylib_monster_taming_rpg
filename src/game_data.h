//
// Created by Hector Mejia on 8/10/24.
//

#ifndef GAME_DATA_H
#define GAME_DATA_H
#include "character_entity.h"

void game_data_init();
void game_data_free();

CharacterData* game_data_for_character_id(const char *characterID);

#endif //GAME_DATA_H
