//
// Created by Hector Mejia on 5/10/24.
//

#ifndef RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H
#define RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H

#include <stdio.h>
#include "raylib.h"
#include "cute_tiled.h"
#include "common.h"

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
    cute_tiled_map_t *tiledMap;
    Image *images;
    Sprite *sprites;
} Map;

static MapInfo mapAtlas[MapIDMax] = {
    {
        .id = MapIDWorld,
        .name = "world_map",
        .mapFilePath = map_path(world.json)
    }
};

Map *load_map(MapID mapID);
void free_map(Map * map);

void load_map_images(Map *map);
cute_tiled_layer_t *get_layer_by_name(cute_tiled_map_t *map, char* layerName);
cute_tiled_tileset_t *get_tile_set_for_gid(cute_tiled_tileset_t *tileSet, i32 gid);

#endif //RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H
