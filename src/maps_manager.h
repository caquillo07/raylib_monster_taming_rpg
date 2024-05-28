//
// Created by Hector Mejia on 5/10/24.
//

#ifndef RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H
#define RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H

#include <stdio.h>
#include "raylib.h"
#include "common.h"
#include "tmx.h"

#define maps_dir "./data/maps/"
#define map_path(MAP_NAME) maps_dir#MAP_NAME

typedef enum MapID {
    MapIDWorld = 0,
    MapIDMax
} MapID;

typedef struct MapInfo {
    MapID id;
    char *name;
    char *mapFilePath;
} MapInfo;

typedef struct Sprite {
    i32 imageID;
    i32 depth;
    f32 x, y;
    f32 scaleX, scaleY;
    i32 textureID;
} Sprite;

typedef struct Map {
    tmx_map *tiledMap;
    tmx_layer *terrainLayer;
    tmx_layer *entitiesLayer;
    Vector2 playerStartingPosition;
    tmx_layer *objectsLayer;
} Map;

static MapInfo mapAtlas[MapIDMax] = {
    {
        .id = MapIDWorld,
        .name = "world_map",
        .mapFilePath = map_path(world.tmx),
    }
};

void maps_manager_init();
Map *load_map(MapID mapID);

void map_free(Map *map);
void map_draw(Map *map);

#endif //RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H
