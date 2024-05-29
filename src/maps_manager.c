//
// Created by Hector Mejia on 5/10/24.
//

#include "maps_manager.h"
#include "common.h"

#include <tmx.h>

static bool loaded = false;

// private funcs
static void *texture_loader_callback(const char *path);
static void texture_free_callback(void *ptr);
static Color int_to_color(u32 color);
static void draw_layer(tmx_map *tmap, tmx_layer *layer);
static void draw_objects_layer(tmx_map *tmap, tmx_layer *layer);
static void draw_tile(void *image, Rectangle sourceRec, Vector2 destination, f32 opacity);

void maps_manager_init() {
    tmx_img_load_func = texture_loader_callback;
    tmx_img_free_func = texture_free_callback;

    loaded = true;
}

static Color int_to_color(u32 color) {
    tmx_col_bytes res = tmx_col_to_bytes(color);
    return (Color) {
        .r = res.r,
        .g = res.g,
        .b = res.b,
        .a = res.a,
    };
    // nice, get a pointer to the memory location, cast it to a Color* and then
    // deref that and get a copy of all the values. This works because they have
    // the same memory layout, but if that struct changes, this goes kaboom.
    // do it manually instead
    // return *((Color*)&res);
}

Map *load_map(MapID mapID) {
    panicIf(!loaded, "maps_manager was initialize, forgot to call maps_manager_init()?");
    panicIf(mapID >= MapIDMax, "map ID provided is invalid");
    MapInfo mapInfo = mapAtlas[mapID];

    Map *map = calloc(1, sizeof(*map));
    panicIfNil(map, "failed to alloc map");
    map->id = mapID;

    map->tiledMap = tmx_load(mapInfo.mapFilePath);
    panicIfNil(map->tiledMap, tmx_strerr());


    // in a real game, this wouldn't work? The way the maps are setup, every map
    // has the same layers so it works, in a real game, that may or may not
    // be the same. Depends how I set it up

    // terrain
    map->terrainLayer = tmx_find_layer_by_name(map->tiledMap, "Terrain");
    panicIfNil(map->terrainLayer, "could not find the Terrain layer in tmx map");

    // terrain top
    map->terrainTopLayer = tmx_find_layer_by_name(map->tiledMap, "Terrain Top");
    panicIfNil(map->terrainTopLayer, "could not find the Terrain Top layer in tmx map");

    // entities
    map->entitiesLayer = tmx_find_layer_by_name(map->tiledMap, "Entities");
    panicIfNil(map->entitiesLayer, "could not find the Entities layer in tmx map");

    // objects
    map->objectsLayer = tmx_find_layer_by_name(map->tiledMap, "Objects");
    panicIfNil(map->objectsLayer, "could not find the Objects layer in the tmx map");

    // get player starting position
    tmx_object *housePlayerObject = tmx_find_object_by_id(map->tiledMap, mapInfo.startingPositionObjectID);
    map->playerStartingPosition = (Vector2) {
        .x = (f32) housePlayerObject->x,
        .y = (f32) housePlayerObject->y,
    };
    return map;
}

void map_draw(Map *map) {
    ClearBackground(int_to_color(map->tiledMap->backgroundcolor));

    // todo this is slow as all hell, but it works .-.
    //  optimize to cache all images/tiles instead of parsing the linked list each time.
    draw_layer(map->tiledMap, map->terrainLayer);
    draw_layer(map->tiledMap, map->terrainTopLayer);
    draw_objects_layer(map->tiledMap, map->objectsLayer);
}

void map_free(Map *map) {
    // LibTMX
    map->terrainLayer = nil;
    map->terrainTopLayer = nil;
    map->entitiesLayer = nil;
    map->objectsLayer = nil;
    tmx_map_free(map->tiledMap);

    free(map);
}

static void draw_layer(tmx_map *tmap, tmx_layer *layer) {
    if (layer == nil || layer->name == nil) {
        printf("no layer found\n");
        return;
    }
    for (u64 i = 0; i < tmap->height; i++) {
        for (u64 j = 0; j < tmap->width; j++) {

            u32 gid = (layer->content.gids[(i * tmap->width) + j]) & TMX_FLIP_BITS_REMOVAL;
            if (tmap->tiles[gid] != NULL) {

                tmx_tileset *tileSet = tmap->tiles[gid]->tileset;
                void *image = tileSet->image->resource_image;

                if (tmap->tiles[gid]->image) {
                    image = tmap->tiles[gid]->image->resource_image;
                }

                // i32 flags = (layer->content.gids[(i * tmap->width) + j]) & ~TMX_FLIP_BITS_REMOVAL;
                Rectangle sourceRec = {
                    .x = (f32) tmap->tiles[gid]->ul_x,
                    .y = (f32) tmap->tiles[gid]->ul_y,
                    .width = (f32) tileSet->tile_width,
                    .height = (f32) tileSet->tile_height,
                };
                Vector2 destination = {
                    .x = (f32) (j * tileSet->tile_width),
                    .y = (f32) (i * tileSet->tile_height),
                };
                draw_tile(image, sourceRec, destination, (f32) layer->opacity);
            }
        }
    }
}

static void draw_tile(void *image, Rectangle sourceRec, Vector2 destination, f32 opacity) {
    u8 c = (u8) opacity * 255;
    Color tint = {c, c, c, c};
    DrawTextureRec(*(Texture2D *) image, sourceRec, destination, tint);
}

static void draw_object_tile(tmx_map *tmap, tmx_object *object) {
    if (tmap == nil || object == nil) {
        slogw("tmap or object are nil in draw_object_tile");
        return;
    }

    i32 gid = object->content.gid;
    tmx_tile *tile = tmap->tiles[gid];
    panicIfNil(tile, "expected to find tileSet for object");
    panicIfNil(tile->image, "object does contain image");
    panicIfNil(tile->image->resource_image, "object does contain image data");

    Rectangle sourceRec = {
        .x = (f32) tile->ul_x,
        .y = (f32) tile->ul_y,
        .width = (f32) object->width,
        .height = (f32) object->height,
    };
    Vector2 destination = {
        .x = (f32) object->x,
        .y = (f32) object->y - sourceRec.height,
    };

    draw_tile(tile->image->resource_image, sourceRec, destination, 1.f); // todo opacity?

    // draw debug frames
    if (!isDebug) { return; }

    Rectangle tileFrame = {
        .x = destination.x,
        .y = destination.y,
        .width = sourceRec.width,
        .height = sourceRec.height,
    };
    DrawRectangleLinesEx(tileFrame, 3.f, RED);
    DrawCircleV(destination, 5.f, RED);
}

static void draw_objects_layer(tmx_map *tmap, tmx_layer *layer) {
    if (layer == nil || layer->name == nil) {
        printf("no layer objects found\n");
        return;
    }

    tmx_object *objectHead = layer->content.objgr->head;
    while (objectHead) {
        if (!objectHead->visible) {
            continue;
        }
        switch (objectHead->obj_type) {
            case OT_TILE:
                draw_object_tile(tmap, objectHead);
                break;
            case OT_NONE:
                panic("draw_object_tile->type == OT_NONE not implemented");
                break;
            case OT_SQUARE:
                panic("draw_object_tile->type == OT_SQUARE not implemented");
                break;
            case OT_POLYGON:
                panic("draw_object_tile->type == OT_POLYGON not implemented");
                break;
            case OT_POLYLINE:
                panic("draw_object_tile->type == OT_POLYLINE not implemented");
                break;
            case OT_ELLIPSE:
                panic("draw_object_tile->type == OT_ELLIPSE not implemented");
                break;
            case OT_TEXT:
                panic("draw_object_tile->type == OT_TEXT not implemented");
                break;
            case OT_POINT:
                panic("draw_object_tile->type == OT_POINT not implemented");
                break;
        }
        objectHead = objectHead->next;
    }
}

static void *texture_loader_callback(const char *path) {
    Texture2D *text = calloc(1, sizeof(*text));
    *text = LoadTexture(path);
    slogi("loaded %s with ID %d", path, text->id);
    return text;
}

static void texture_free_callback(void *ptr) {
    slogi("unloading texture #%d", ((Texture2D *) ptr)->id);
    UnloadTexture(*(Texture2D *) ptr);
    free(ptr);
}
