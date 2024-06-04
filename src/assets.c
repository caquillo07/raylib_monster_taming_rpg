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

// todo(hector) - heap?
Assets assets;

void load_textures() {
    assets = (Assets) {};
    assets.waterTextures.textures = import_textures_from_directory(assets_path(water));
    assets.waterTextures.len = 4;
}

void unload_textures() {
    array_free(assets.waterTextures.textures);
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

static int dir_entry_compare(const void *lhsp, const void *rhsp) {
    const struct dirent *lhs = lhsp;
    const struct dirent *rhs = rhsp;
    return strcmp(lhs->d_name, rhs->d_name);
}
