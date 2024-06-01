//
// Created by Hector Mejia on 5/30/24.
//

#ifndef RAYLIB_POKEMON_CLONE_ASSETS_H
#define RAYLIB_POKEMON_CLONE_ASSETS_H

#include "raylib.h"
#include "common.h"

typedef struct Assets {
    struct {
        i32 len;
        Texture2D *textures;
    } waterTextures ;
} Assets;

extern Assets assets;

void init_textures();

Texture2D *import_textures_from_directory(char* dir);

#endif //RAYLIB_POKEMON_CLONE_ASSETS_H
