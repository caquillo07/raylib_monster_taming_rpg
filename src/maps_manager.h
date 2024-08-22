//
// Created by Hector Mejia on 5/10/24.
//

#ifndef RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H
#define RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H

#include "raylib.h"
#include "common.h"
#include "tmx.h"
#include "sprites.h"
#include "character_entity.h"


typedef enum MapID {
    MapIDWorld = 0,
    MapIDHospital = 1,
    MapIDHouse = 2,
    MapIDArena = 3,
    MapIDFire = 4,
    MapIDPlant = 5,
    MapIDWater = 6,

    MapIDMax
} MapID;

#define  MAX_MAP_NAME_LEN 32

typedef struct MapInfo {
    MapID id;
    char name[MAX_MAP_NAME_LEN];
    char *mapFilePath;
    u32 startingPositionObjectID;
} MapInfo;

#define MAX_TRASNSITION_DEST_LEN 32

typedef struct TransitionSprite {
    Rectangle box;
    char destination[MAX_TRASNSITION_DEST_LEN];
    char destinationPos[MAX_TRASNSITION_DEST_LEN];
} TransitionSprite;

typedef struct Map {
    MapID id;
    tmx_map *tiledMap;

    AnimatedTexturesSprite *waterSpritesList;
    AnimatedTiledSprite *coastLineSpritesList;
    Character *overWorldCharacters;

    StaticSprite *backgroundSprites;
    StaticSprite *mainSprites;
    StaticSprite *foregroundSprites;

    Rectangle *collisionBoxes;
    TransitionSprite *transitionBoxes;

    Vector2 playerStartingPosition;
} Map;


void maps_manager_init();
Map *load_map(MapID mapID);
void map_free(Map *map);
MapID map_id_for_name(const char *name);

void map_update(const Map *map, f32 dt);
void map_draw(const Map *map);

#endif //RAYLIB_POKEMON_CLONE_MAPS_MANAGER_H
