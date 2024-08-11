//
// Created by Hector Mejia on 5/30/24.
//

#include <dirent.h>
#include "assets.h"

#include <assert.h>

#include "common.h"
#include "array/array.h"

static Texture2D *import_textures_from_directory(char *dir);
static int dir_entry_compare(const void *lhsp, const void *rhsp);
static TileMap load_tile_map(i32 cols, i32 rows, const char *imagePath);

Assets assets;

void load_assets() {
    assets = (Assets) {};
    assets.waterTextures.texturesList = import_textures_from_directory("./graphics/tilesets/water");
    assets.waterTextures.len = 4;

    // each graphic is 192, but we want 64px chunks
    assets.tileMaps[TileMapIDCoastLine] = load_tile_map(3 * 8, 3 * 4, ("./graphics/tilesets/coast.png"));

    // player and characters
    assets.tileMaps[TileMapIDPlayer] = load_tile_map(4, 4, ("./graphics/characters/player.png"));
    assets.tileMaps[TileMapIDBlondCharacter] = load_tile_map(4, 4, ("./graphics/characters/blond.png"));
    assets.tileMaps[TileMapIDFireBossCharacter] = load_tile_map(4, 4, ("./graphics/characters/fire_boss.png"));
    assets.tileMaps[TileMapIDGrassBossCharacter] = load_tile_map(4, 4, ("./graphics/characters/grass_boss.png"));
    assets.tileMaps[TileMapIDHatGirlCharacter] = load_tile_map(4, 4, ("./graphics/characters/hat_girl.png"));
    assets.tileMaps[TileMapIDPurpleGirlCharacter] = load_tile_map(4, 4, ("./graphics/characters/purple_girl.png"));
    assets.tileMaps[TileMapIDStrawCharacter] = load_tile_map(4, 4, ("./graphics/characters/straw.png"));
    assets.tileMaps[TileMapIDWaterBossCharacter] = load_tile_map(4, 4, ("./graphics/characters/water_boss.png"));
    assets.tileMaps[TileMapIDYoungGirlCharacter] = load_tile_map(4, 4, ("./graphics/characters/young_girl.png"));
    assets.tileMaps[TileMapIDYoungGuyCharacter] = load_tile_map(4, 4, ("./graphics/characters/young_guy.png"));

    assets.grassTexture = LoadTexture("./graphics/objects/grass.png");
    assets.iceGrassTexture = LoadTexture("./graphics/objects/grass_ice.png");
    assets.sandTexture = LoadTexture("./graphics/objects/sand.png");

    assets.characterShadowTexture = LoadTexture("./graphics/other/shadow.png");

    assets.dialogFont = LoadFontEx("./graphics/fonts/PixeloidSans.ttf", 30, nil, 250);
}

void unload_assets() {
    array_range(assets.waterTextures.texturesList, i) {
        UnloadTexture(assets.waterTextures.texturesList[i]);
    }
    array_free(assets.waterTextures.texturesList);

    UnloadTexture(assets.tileMaps[TileMapIDCoastLine].texture);
    array_free(assets.tileMaps[TileMapIDCoastLine].framesList);

    UnloadTexture(assets.tileMaps[TileMapIDPlayer].texture);
    array_free(assets.tileMaps[TileMapIDPlayer].framesList);

    UnloadTexture(assets.tileMaps[TileMapIDBlondCharacter].texture);
    array_free(assets.tileMaps[TileMapIDBlondCharacter].framesList);
    UnloadTexture(assets.tileMaps[TileMapIDFireBossCharacter].texture);
    array_free(assets.tileMaps[TileMapIDFireBossCharacter].framesList);
    UnloadTexture(assets.tileMaps[TileMapIDGrassBossCharacter].texture);
    array_free(assets.tileMaps[TileMapIDGrassBossCharacter].framesList);
    UnloadTexture(assets.tileMaps[TileMapIDHatGirlCharacter].texture);
    array_free(assets.tileMaps[TileMapIDHatGirlCharacter].framesList);
    UnloadTexture(assets.tileMaps[TileMapIDPurpleGirlCharacter].texture);
    array_free(assets.tileMaps[TileMapIDPurpleGirlCharacter].framesList);
    UnloadTexture(assets.tileMaps[TileMapIDStrawCharacter].texture);
    array_free(assets.tileMaps[TileMapIDStrawCharacter].framesList);
    UnloadTexture(assets.tileMaps[TileMapIDWaterBossCharacter].texture);
    array_free(assets.tileMaps[TileMapIDWaterBossCharacter].framesList);
    UnloadTexture(assets.tileMaps[TileMapIDYoungGirlCharacter].texture);
    array_free(assets.tileMaps[TileMapIDYoungGirlCharacter].framesList);
    UnloadTexture(assets.tileMaps[TileMapIDYoungGuyCharacter].texture);
    array_free(assets.tileMaps[TileMapIDYoungGuyCharacter].framesList);

    UnloadTexture(assets.grassTexture);
    UnloadTexture(assets.iceGrassTexture);
    UnloadTexture(assets.sandTexture);

    UnloadTexture(assets.characterShadowTexture);

    UnloadFont(assets.dialogFont);
}

static Texture2D *import_textures_from_directory(char *dirPath) {
    DIR *dir = opendir(dirPath);
    panicIfNil(dir, "failed to open directory %s for texture", dirPath);

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
    const Texture2D texture = LoadTexture(imagePath);
    panicIf(!IsTextureReady(texture), "failed to load texture");

    Rectangle *framesList = nil;
    const i32 cellWidth = texture.width / cols;
    const i32 cellHeight = texture.height / rows;
    for (i32 row = 0; row < rows; row++) {
        for (i32 col = 0; col < cols; col++) {
            const Rectangle frame = {
                .x = (f32) (col * cellWidth),
                .y = (f32) (row * cellHeight),
                .width = (f32) cellWidth,
                .height = (f32) cellHeight,
            };
            array_push(framesList, frame);
        }
    }
    const TileMap tileMap = {
        .framesList = framesList,
        .rows = rows,
        .columns = cols,
        .texture = texture,
    };
    return tileMap;
}

// returns a copy of the frame inside the tilemap
Rectangle tile_map_get_frame_at(const TileMap tm, const i32 col, const i32 row) {
    const i32 index = (tm.columns * row) + col;
    panicIf((index > array_length(tm.framesList) - 1), "queried x,y is out of bounds");
    return tm.framesList[index];
}

static int dir_entry_compare(const void *lhsp, const void *rhsp) {
    const struct dirent *lhs = lhsp;
    const struct dirent *rhs = rhsp;
    return strcmp(lhs->d_name, rhs->d_name);
}
