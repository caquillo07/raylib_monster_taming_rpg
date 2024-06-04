//
// Created by Hector Mejia on 5/10/24.
//

#include "maps_manager.h"
#include "common.h"
#include "memory/memory.h"
#include "array/array.h"
#include "sprites.h"
#include "assets.h"
#include "settings.h"

#include <tmx.h>


#define maps_dir "./data/maps/"
#define map_path(MAP_NAME) maps_dir#MAP_NAME

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

static bool loaded = false;

// private funcs
static void *texture_loader_callback(const char *path);
static void texture_free_callback(void *ptr);
static void *map_alloc_callback(void *ptr, size_t len);
static void map_free_callback(void *ptr);
static void print_map_total_memory();

static Color int_to_color(u32 color);

static AnimatedSprite *init_water_sprites(tmx_layer *layer);

static void draw_layer(tmx_map *tmap, tmx_layer *layer);
static void draw_objects_layer(tmx_map *tmap, tmx_layer *layer);
static void draw_water_sprites(AnimatedSprite *waterSprites);
static void draw_tile(void *image, Rectangle sourceRec, Vector2 destination, f32 opacity);

void maps_manager_init() {
    tmx_img_load_func = texture_loader_callback;
    tmx_img_free_func = texture_free_callback;

    tmx_alloc_func = map_alloc_callback;
    tmx_free_func = map_free_callback;

    loaded = true;
    atexit(print_map_total_memory);
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

    Map *map = mallocate(sizeof(*map), MemoryTagGame);
    panicIfNil(map, "failed to alloc map");

    map->id = mapID;
    map->tiledMap = tmx_load(mapInfo.mapFilePath);
    panicIfNil(map->tiledMap, tmx_strerr());


    // in a real game, this wouldn't work? The way the maps are set up, every map
    // has the same layers, so it works, in a real game, that may or may not
    // be the same. Depends on how I set it up

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

    // Water
    map->waterLayer = tmx_find_layer_by_name(map->tiledMap, "Water");
    panicIfNil(map->waterLayer, "could not find the Objects layer in the tmx map");

    map->waterSprites = init_water_sprites(map->waterLayer);

    // get player starting position
    tmx_object *housePlayerObject = tmx_find_object_by_id(map->tiledMap, mapInfo.startingPositionObjectID);
    map->playerStartingPosition = (Vector2) {
        .x = (f32) housePlayerObject->x,
        .y = (f32) housePlayerObject->y,
    };
    return map;
}

void map_free(Map *map) {
    array_free(map->waterSprites);
    map->waterSprites = nil;

    // LibTMX
    // todo - add to memory/memory.h
    tmx_map_free(map->tiledMap);

    map->terrainLayer = nil;
    map->terrainTopLayer = nil;
    map->entitiesLayer = nil;
    map->objectsLayer = nil;
    map->waterLayer = nil;
    mfree(map, sizeof(*map), MemoryTagGame);
}

static AnimatedSprite *init_water_sprites(tmx_layer *layer) {
    AnimatedSprite *animatedSprite = nil;
    tmx_object *waterTileH = layer->content.objgr->head;
    while (waterTileH) {
        if (!waterTileH->visible) {
            continue;
        }

        for (i32 x = (i32) waterTileH->x; x < waterTileH->x + waterTileH->width; x += TILE_SIZE) {
            for (i32 y = (i32) waterTileH->y; y < waterTileH->y + waterTileH->height; y += TILE_SIZE) {
                AnimatedSprite waterSprite = {
                    .id = waterTileH->id,
                    .position = {.x = (f32) x, .y = (f32) y},
                    .textures = assets.waterTextures.textures,
                    .framesLen = 4,
                    .currentFrame = 0,
                    .frameTimer = 0.f,
                    .animationSpeed = settings.waterAnimationSpeed,
                };
                array_push(animatedSprite, waterSprite);
            }
        }

        waterTileH = waterTileH->next;
    }

    return animatedSprite;
}

void map_update(Map *map, f32 dt) {
    for (i32 i = 0; i < array_length(map->waterSprites); i++) {
        animate_sprite(&map->waterSprites[i], dt);
    }
}

void map_draw(Map *map) {
    ClearBackground(int_to_color(map->tiledMap->backgroundcolor));

    // todo this is slow as all hell, but it works .-.
    //  optimize to cache all images/tiles instead of parsing the linked list each time.
    draw_layer(map->tiledMap, map->terrainLayer);
    draw_layer(map->tiledMap, map->terrainTopLayer);
    draw_objects_layer(map->tiledMap, map->objectsLayer);
    draw_water_sprites(map->waterSprites);
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

static void draw_tile(void *imageTexture2D, Rectangle sourceRec, Vector2 destination, f32 opacity) {
    u8 c = (u8) opacity * 255;
    Color tint = {c, c, c, c};
    DrawTextureRec(*(Texture2D *) imageTexture2D, sourceRec, destination, tint);
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

static void draw_water_sprites(AnimatedSprite *waterSprites) {
    if (array_length(waterSprites) == 0) { return; }

    for (i32 i = 0; i < array_length(waterSprites); i++) {
        AnimatedSprite waterSprite = waterSprites[i];

        Rectangle sourceRec = {
            .x = waterSprite.position.x,
            .y = waterSprite.position.y,
            .height = (f32) waterSprite.textures[0].height,
            .width = (f32) waterSprite.textures[0].width,
        };

        Texture2D *frameToDraw = &waterSprite.textures[waterSprite.currentFrame];
        draw_tile(
            frameToDraw,
            sourceRec,
            waterSprite.position,
            1.f
        );

        // draw debug frames
        if (!isDebug) { continue; }

        Rectangle tileFrame = {
            .x = waterSprite.position.x,
            .y = waterSprite.position.y,
            .width = sourceRec.width,
            .height = sourceRec.height,
        };
        DrawRectangleLinesEx(tileFrame, 3.f, RED);
        DrawCircleV(waterSprite.position, 5.f, RED);
    }
}

static void *texture_loader_callback(const char *path) {
    Texture2D *text = mallocate(sizeof(*text), MemoryTagTexture);
    *text = LoadTexture(path);
    slogi("loaded %s with ID %d", path, text->id);
    return text;
}

static void texture_free_callback(void *ptr) {
    Texture2D *text = (Texture2D *) ptr;
    slogi("unloading texture #%d", text->id);
    UnloadTexture(*text);
    mfree(ptr, sizeof(*text), MemoryTagTexture);
}

static u64 totalMapMemoryAllocated = 0;

void *map_alloc_callback(void *ptr, size_t len) {
    // TODO - what to do .-.
    totalMapMemoryAllocated += len;
    slogi("allocating %d bytes for map, total: %d", len, totalMapMemoryAllocated);
    return realloc(ptr, len);
}

void map_free_callback(void *ptr) {
    free(ptr);
}

void print_map_total_memory() {
    slogi("total memory allocated by TmxLib: %d bytes", totalMapMemoryAllocated);
}
