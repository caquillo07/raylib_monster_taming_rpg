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
    MapIDHospital = 1,
    MapIDMax
} MapID;

typedef struct MapInfo {
    MapID id;
    char *name;
    char *mapFilePath;
    u32 startingPositionObjectID;
} MapInfo;

typedef struct Sprite {
    i32 imageID;
    i32 depth;
    f32 x, y;
    f32 scaleX, scaleY;
    i32 textureID;
} Sprite;

typedef struct Map {
    MapID id;
    tmx_map *tiledMap;
    tmx_layer *terrainLayer;
    tmx_layer *terrainTopLayer;
    tmx_layer *entitiesLayer;
    tmx_layer *objectsLayer;

    Vector2 playerStartingPosition;
} Map;

static MapInfo mapAtlas[MapIDMax] = {
    {
        .id = MapIDWorld,
        .name = "world_map",
        .mapFilePath = map_path(world.tmx),
        .startingPositionObjectID = 12,
    },
    {
        .id = MapIDHospital,
        .name = "hospital_map",
        .mapFilePath = map_path(hospital.tmx),
        .startingPositionObjectID = 12,
    }
};

void maps_manager_init();
Map *load_map(MapID mapID);

void map_free(Map *map);
void map_draw(Map *map);

#endif //RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H
