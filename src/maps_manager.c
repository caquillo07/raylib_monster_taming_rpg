//
// Created by Hector Mejia on 5/10/24.
//

#include <assert.h>
#include "maps_manager.h"
#include "common.h"
#include "array/array.h"


Map *load_map(MapID mapID) {
    panicIf(mapID >= MapIDMax, "map ID provided is invalid");

    MapInfo mapInfo = mapAtlas[mapID];
    cute_tiled_map_t *tiledMap = cute_tiled_load_map_from_file(mapInfo.mapFilePath, 0);
    panicIf(tiledMap == nil, "failed to alloc tileMap");

    printf("sizeof Map %lu\n", sizeof(Map));
    Image *imagesArray = calloc(5, sizeof(*imagesArray));
    panicIf(imagesArray == nil, "failed to alloc imagesArray");

    Map *map = calloc(1, sizeof(*map));
    panicIf(map == nil, "failed to alloc map");

    map->tiledMap = tiledMap;

    cute_tiled_layer_t *layer = get_layer_by_name(tiledMap, "Terrain");
    panicIf(layer == nil, "expected to find Terrain layer in tiledMap");
    assert(layer->data_count == layer->height * layer->width);

    printf("found %s\n", layer->name.ptr);

    load_map_images(map);

    for (i32 row = 0; row < layer->height; row++) {
        for (i32 col = 0; col < layer->width; col++) {
            int mapTileY = row * layer->width;
            int mapTileX = mapTileY + col;
            i32 tileInfoFlag = layer->data[mapTileY + mapTileX];
            cute_tiled_tileset_t *tileSet = get_tile_set_for_gid(map->tiledMap->tilesets, tileInfoFlag);
            printf("using tilset %d for data %d\n", tileSet->firstgid, tileInfoFlag);

            i32 hFlip, vFlip, dFlip;
            cute_tiled_get_flags(tileInfoFlag, &hFlip, &vFlip, &dFlip);
            i32 tileID = cute_tiled_unset_flags(tileInfoFlag);
            int spriteID = tileInfoFlag - tileSet->firstgid;
            printf("spriteID: %d - tileID: %d - hFlip: %d - vFlip: %d - dFlip: %d\n", spriteID, tileID, hFlip, vFlip,
                   dFlip);

            // todo (hector) - give up, try the other TMX lib

            Sprite sprite = {
                .imageID = spriteID,
                .depth = 0,
                .x = (float) (col % map->tiledMap->width),
                .y = (float) (map->tiledMap->height - col / map->tiledMap->width),
                .scaleX = 1.0f,
                .scaleY = 1.0f,

            };

        }
        break;
    }
    cute_tiled_free_map(tiledMap);
    return map;
}

void load_map_images(Map *map) {
    cute_tiled_tileset_t *tileSet = map->tiledMap->tilesets;
    while (tileSet) {
        printf("%d - %s\n", tileSet->firstgid, tileSet->source.ptr);

        if (tileSet->image.ptr != nil) {
            Image image = LoadImage(tileSet->image.ptr);
            panicIf(image.data == nil, "failed to allocate image");
            array_push(map->images, image);
        } else if (tileSet->tiles != nil) {
            cute_tiled_tile_descriptor_t *tile = tileSet->tiles;
            while (tile) {
                Image image = LoadImage(tile->image.ptr);
                panicIf(image.data == nil, "failed to allocate image");
                array_push(map->images, image);
                tile = tile->next;
            }
        }

        tileSet = tileSet->next;
    }
}

void free_map(Map *map) {
    for (i32 i = 0; i < array_length(map->images); i++) {
        UnloadImage(map->images[i]);
    }
    map->images = nil;

    free(map);
    map = nil;
}

cute_tiled_layer_t *get_layer_by_name(cute_tiled_map_t *map, char *layerName) {
    cute_tiled_layer_t *layer = map->layers;
    while (layer) {
        if (strcmp(layer->name.ptr, layerName) == 0) {
            return layer;
        }
        layer = layer->next;
    }

    return nil;
}

cute_tiled_tileset_t *get_tile_set_for_gid(cute_tiled_tileset_t *tileSet, i32 gid) {
    cute_tiled_tileset_t *cursor = tileSet;
    while(cursor) {
        if (cursor->firstgid <= gid) {
            return cursor;
        }
        cursor = cursor->next;
    }
    return nil;
}

void tiled_demo() {

    cute_tiled_map_t *map = cute_tiled_load_map_from_file("data/maps/world.json", 0);
    // get map width and height
    int w = map->width;
    int h = map->height;
    printf("Map width: %d, height: %d\n", w, h);

    // loop over the map's layers
    cute_tiled_layer_t *layer = map->layers;
    while (layer) {
//        int *data = layer->data;
//        int data_count = layer->data_count;
        printf("%s\n", layer->name.ptr);
        printf("\tID %d\n", layer->id);
        printf("\timage %s\n", layer->image.ptr);
        printf("\tsize %dx%d\n", layer->height, layer->width);
        printf("\tvisible %d\n", layer->visible);

        // do something with the tile data
//        UserFunction_HandleTiles(data, data_count);

        layer = layer->next;
    }

    cute_tiled_free_map(map);
}
