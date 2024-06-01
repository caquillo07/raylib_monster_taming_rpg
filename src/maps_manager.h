//
// Created by Hector Mejia on 5/10/24.
//

#ifndef RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H
#define RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H

#include <stdio.h>
#include "raylib.h"
#include "common.h"
#include "tmx.h"
#include "sprites.h"


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

typedef struct Map {
    MapID id;
    tmx_map *tiledMap;
    tmx_layer *terrainLayer;
    tmx_layer *terrainTopLayer;
    tmx_layer *entitiesLayer;
    tmx_layer *objectsLayer;
    tmx_layer *waterLayer;

    AnimatedSprite *waterSprites;

    Vector2 playerStartingPosition;
} Map;


void maps_manager_init();
Map *load_map(MapID mapID);
void map_free(Map *map);

void map_update(Map *map, f32 dt);
void map_draw(Map *map);

#endif //RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H
