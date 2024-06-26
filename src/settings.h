//
// Created by Hector Mejia on 6/1/24.
//

#ifndef RAYLIB_POKEMON_CLONE_SETTINGS_H
#define RAYLIB_POKEMON_CLONE_SETTINGS_H

#include "common.h"

typedef struct GameSettings {
    f32 playerSpeed;
    f32 playerAnimationSpeed;
    f32 waterAnimationSpeed;
    f32 coastLineAnimationSpeed;
} GameSettings;

extern GameSettings settings;

#endif //RAYLIB_POKEMON_CLONE_SETTINGS_H
