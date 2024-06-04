//
// Created by Hector Mejia on 5/30/24.
//

#include <dirent.h>
#include "assets.h"
#include "common.h"
#include "array/array.h"

#define assets_dir "./graphics/tilesets/"
#define assets_path(assetName) assets_dir#assetName


// todo(hector) - heap?
Assets assets;

void load_textures() {
    assets = (Assets){};
    assets.waterTextures.textures = import_textures_from_directory(assets_path(water));
    assets.waterTextures.len = 4;
}

void unload_textures() {
    array_free(assets.waterTextures.textures);
}

Texture2D *import_textures_from_directory(char *dirPath) {
    DIR *dir = opendir(dirPath);
    panicIfNil(dir, "failed to open directory path for texture");

    Texture2D *dynTextures = nil;
    struct dirent* fileInfo;
    while ((fileInfo = readdir(dir)) != 0) {
        if (strncmp(".", fileInfo->d_name, 1) == 0 || strncmp("..", fileInfo->d_name, 2) == 0) {
            continue;
        }
#define textFilePathBufSize 1024
        char buff[textFilePathBufSize];
        snprintf(buff, textFilePathBufSize, "%s/%s", dirPath, fileInfo->d_name);
        const Texture2D text = LoadTexture(buff);
        array_push(dynTextures, text);
    }
    closedir(dir);
    return dynTextures;
}
