//
// Created by Hector Mejia on 5/30/24.
//

#include <dirent.h>
#include "assets.h"
#include "common.h"
#include "array/array.h"

#define assets_dir "./graphics/"
#define assets_path(assetName) assets_dir#assetName


static Texture2D *import_textures_from_directory(char *dir);
static int dir_entry_compare(const void *lhsp, const void *rhsp);
static TileMap load_tile_map(i32 cols, i32 rows, const char *imagePath);

// todo(hector) - heap?
Assets assets;

void load_textures() {
    assets = (Assets) {};
    assets.waterTextures.texturesList = import_textures_from_directory(assets_path(tilesets/water));
    assets.waterTextures.len = 4;

    // each graphic is 192, but we want 64px chunks
    assets.coastLineTileMap = load_tile_map(3*8, 3*4, assets_path(tilesets/coast.png));

    // player and characters
    assets.playerTileMap = load_tile_map(4, 4, assets_path(characters/player.png));
    assets.blondCharacterTileMap = load_tile_map(4, 4, assets_path(characters/blond.png));
    assets.fireBossCharacterTileMap = load_tile_map(4, 4, assets_path(characters/fire_boss.png));
    assets.grassBossCharacterTileMap = load_tile_map(4, 4, assets_path(characters/grass_boss.png));
    assets.hatGirlCharacterTileMap = load_tile_map(4, 4, assets_path(characters/hat_girl.png));
    assets.purpleGirlCharacterTileMap = load_tile_map(4, 4, assets_path(characters/purple_girl.png));
    assets.strawCharacterTileMap = load_tile_map(4, 4, assets_path(characters/straw.png));
    assets.waterBossCharacterTileMap = load_tile_map(4, 4, assets_path(characters/water_boss.png));
    assets.youngGirlCharacterTileMap = load_tile_map(4, 4, assets_path(characters/young_girl.png));
    assets.youngGuyCharacterTileMap = load_tile_map(4, 4, assets_path(characters/young_guy.png));
}

void unload_textures() {
    array_range(assets.waterTextures.texturesList, i) {
        UnloadTexture(assets.waterTextures.texturesList[i]);
    }
    array_free(assets.waterTextures.texturesList);

    UnloadTexture(assets.coastLineTileMap.texture);
    array_free(assets.coastLineTileMap.framesList);

    UnloadTexture(assets.playerTileMap.texture);
    array_free(assets.playerTileMap.framesList);

    UnloadTexture(assets.blondCharacterTileMap.texture);
    array_free(assets.blondCharacterTileMap.framesList);
    UnloadTexture(assets.fireBossCharacterTileMap.texture);
    array_free(assets.fireBossCharacterTileMap.framesList);
    UnloadTexture(assets.grassBossCharacterTileMap.texture);
    array_free(assets.grassBossCharacterTileMap.framesList);
    UnloadTexture(assets.hatGirlCharacterTileMap.texture);
    array_free(assets.hatGirlCharacterTileMap.framesList);
    UnloadTexture(assets.purpleGirlCharacterTileMap.texture);
    array_free(assets.purpleGirlCharacterTileMap.framesList);
    UnloadTexture(assets.strawCharacterTileMap.texture);
    array_free(assets.strawCharacterTileMap.framesList);
    UnloadTexture(assets.waterBossCharacterTileMap.texture);
    array_free(assets.waterBossCharacterTileMap.framesList);
    UnloadTexture(assets.youngGirlCharacterTileMap.texture);
    array_free(assets.youngGirlCharacterTileMap.framesList);
    UnloadTexture(assets.youngGuyCharacterTileMap.texture);
    array_free(assets.youngGuyCharacterTileMap.framesList);
}

static Texture2D *import_textures_from_directory(char *dirPath) {
    DIR *dir = opendir(dirPath);
    panicIfNil(dir, "failed to open directory path for texture");

    struct dirent *fileInfo;
    struct dirent *dynFileInfoList = nil;
    while ((fileInfo = readdir(dir)) != 0) {
        if (strncmp(".", fileInfo->d_name, 1) == 0 || strncmp("..", fileInfo->d_name, 2) == 0) {
            continue;
        }
        array_push(dynFileInfoList, *fileInfo);
    }
    closedir(dir);
    panicIfNil(dynFileInfoList, "directory for import was empty");

    // sort the directory entries
    qsort(dynFileInfoList, array_length(dynFileInfoList), sizeof(*dynFileInfoList), dir_entry_compare);

    Texture2D *dynTextures = nil;
    for (int i = 0; i < array_length(dynFileInfoList); i++) {
#define textFilePathBufSize 1024
        char buff[textFilePathBufSize];
        snprintf(buff, textFilePathBufSize, "%s/%s", dirPath, dynFileInfoList[i].d_name);
        const Texture2D text = LoadTexture(buff);
        array_push(dynTextures, text);
    }

    array_free(dynFileInfoList);
    return dynTextures;
}

TileMap load_tile_map(const i32 cols, const i32 rows, const char *imagePath) {
    Texture2D texture = LoadTexture(imagePath);
    panicIf(!IsTextureReady(texture), "failed to load texture");

    Rectangle *framesList = nil;
    i32 cellWidth = texture.width / cols;
    i32 cellHeight = texture.height / rows;
    for (i32 row = 0; row < rows; row++) {
        for (i32 col = 0; col < cols; col++) {
            Rectangle frame = {
                .x = (f32) (col * cellWidth),
                .y = (f32) (row * cellHeight),
                .width = (f32) cellWidth,
                .height = (f32) cellHeight,
            };
            array_push(framesList, frame);
        }
    }
    TileMap tileMap = {
        .framesList = framesList,
        .rows = rows,
        .columns = cols,
        .texture = texture,
    };
    return tileMap;
}

// returns a copy of the frame inside the tilemap
Rectangle tile_map_get_frame_at(TileMap tm, i32 col, i32 row) {
    i32 index = (tm.columns * row) + col;
    panicIf((index > array_length(tm.framesList) - 1), "queried x,y is out of bounds");
    return tm.framesList[index];
}

static int dir_entry_compare(const void *lhsp, const void *rhsp) {
    const struct dirent *lhs = lhsp;
    const struct dirent *rhs = rhsp;
    return strcmp(lhs->d_name, rhs->d_name);
}
