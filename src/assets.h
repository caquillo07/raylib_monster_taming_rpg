//
// Created by Hector Mejia on 5/30/24.
//

#ifndef RAYLIB_POKEMON_CLONE_ASSETS_H
#define RAYLIB_POKEMON_CLONE_ASSETS_H

#include "raylib.h"
#include "common.h"

typedef struct TileMap {
    i32 columns;
    i32 rows;
    Texture2D texture;
    Rectangle *framesList;
} TileMap;

typedef struct Assets {
    struct {
        i32 len;
        Texture2D *texturesList;
    } waterTextures;

    TileMap coastLineTileMap;
    TileMap playerTileMap;
    TileMap blondCharacterTileMap;
    TileMap fireBossCharacterTileMap;
    TileMap grassBossCharacterTileMap;
    TileMap hatGirlCharacterTileMap;
    TileMap purpleGirlCharacterTileMap;
    TileMap strawCharacterTileMap;
    TileMap waterBossCharacterTileMap;
    TileMap youngGirlCharacterTileMap;
    TileMap youngGuyCharacterTileMap;
} Assets;

extern Assets assets;

void load_textures();
void unload_textures();

Rectangle tile_map_get_frame_at(TileMap tm, i32 col, i32 row);

#endif //RAYLIB_POKEMON_CLONE_ASSETS_H
