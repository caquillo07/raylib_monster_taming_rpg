//
// Created by Hector Mejia on 5/30/24.
//

#include <dirent.h>
#include "assets_manager.h"
#include "common.h"
#include "array/array.h"

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
