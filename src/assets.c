//
// Created by Hector Mejia on 5/30/24.
//

#include <dirent.h>
#include "assets.h"
#include "common.h"
#include "array/array.h"

#define assets_dir "./graphics/tilesets/"
#define assets_path(assetName) assets_dir#assetName

static int dir_entry_compare(const void *lhsp, const void *rhsp);
static TileMap load_tile_map(i32 cols, i32 rows, const char *imagePath);

// todo(hector) - heap?
Assets assets;

void load_textures() {
    assets = (Assets) {};
    assets.waterTextures.texturesList = import_textures_from_directory(assets_path(water));
    assets.waterTextures.len = 4;

    // each graphic is 192, but we want 64px chunks
    assets.coastLineTileMap = load_tile_map(3*8, 3*4, assets_path(coast.png));
}

void unload_textures() {
    array_free(assets.waterTextures.texturesList);
    array_free(assets.coastLineTileMap.framesList);
}

Texture2D *import_textures_from_directory(char *dirPath) {
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
