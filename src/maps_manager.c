//
// Created by Hector Mejia on 5/10/24.
//

#include <assert.h>
#include "maps_manager.h"
#include "common.h"
#include "array/array.h"

#include <tmx.h>

#define LINE_THICKNESS 2.5
static bool loaded = false;

// private funcs
static void *texture_loader_callback(const char *path);
static void texture_free_callback(void *ptr);
static Color int_to_color(u32 color);
static void draw_all_layers(tmx_map *tmap, tmx_layer *layers);
static void draw_objects(tmx_object_group *objectGroup);
static void draw_image(tmx_image *image);
static void draw_layer(tmx_map *tmap, tmx_layer *layer);
static void draw_tile(void *image, Rectangle sourceRec, Vector2 destination, f32 opacity, i32 flags);

static void draw_polygon(double x, double y, double **points, int pointsCount, Color color);

static void draw_polyline(double x, double y, double **points, int pointsLen, Color color);

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

    tmx_map *tmap = tmx_load(mapInfo.mapFilePath);
    panicIf(tmap == nil, tmx_strerr());

    Map *map = calloc(1, sizeof(*map));
    panicIf(map == nil, "failed to alloc map");

    map->tiledMap = tmap;
    return map;
}

void map_draw(Map *map) {
    ClearBackground(int_to_color(map->tiledMap->backgroundcolor));
    draw_all_layers(map->tiledMap, map->tiledMap->ly_head);
}

void map_free(Map *map) {
    // LibTMX
    tmx_map_free(map->tiledMap);

    free(map);
    map = nil;
}

static void draw_all_layers(tmx_map *tmap, tmx_layer *layers) {
    while (layers) {
        if (layers->visible) {
            if (layers->type == L_GROUP) {
                draw_all_layers(tmap, layers->content.group_head); // recursive call
            } else if (layers->type == L_OBJGR) {
                draw_objects(layers->content.objgr); // Function to be implemented
            } else if (layers->type == L_IMAGE) {
                draw_image(layers->content.image); // Function to be implemented
            } else if (layers->type == L_LAYER) {
                draw_layer(tmap, layers); // Function to be implemented
            }
        }
        layers = layers->next;
    }
}

static void draw_layer(tmx_map *tmap, tmx_layer *layer) {
    if (layer == nil || layer->name == nil) {
        printf("no layer found\n");
        return;
    }
    for (u64 i = 0; i < tmap->height; i++) {
        for (u64 j = 0; j < tmap->width; j++) {

            i32 gid = (layer->content.gids[(i * tmap->width) + j]) & TMX_FLIP_BITS_REMOVAL;
            if (tmap->tiles[gid] != NULL) {

                tmx_tileset *tileSet = tmap->tiles[gid]->tileset;
                void *image = tileSet->image->resource_image;

                if (tmap->tiles[gid]->image) {
                    image = tmap->tiles[gid]->image->resource_image;
                }

                i32 flags = (layer->content.gids[(i * tmap->width) + j]) & ~TMX_FLIP_BITS_REMOVAL;
                Rectangle sourceRec = {
                    .x = tmap->tiles[gid]->ul_x,
                    .y = tmap->tiles[gid]->ul_y,
                    .width = tileSet->tile_width,
                    .height = tileSet->tile_height,
                };
                Vector2 destination = {
                    .x = j * tileSet->tile_width,
                    .y = i * tileSet->tile_height,
                };
                draw_tile(
                    image,
                    sourceRec,
                    destination,
                    layer->opacity,
                    flags
                );
            }
        }
    }
}

static void draw_tile(void *image, Rectangle sourceRec, Vector2 destination, f32 opacity, i32 flags) {
    (void) flags;
    i8 c = (i8) opacity * 255;
    Color tint = {c, c, c, c};
    DrawTextureRec(*(Texture2D *) image, sourceRec, destination, tint);
}

static void draw_image(tmx_image *image) {
    if (image == nil || image->source == nil) {
        printf("no image found\n");
        return;
    }
    printf("drawing image: %s\n", image->source);
    Texture2D *texture = (Texture2D *) image->resource_image;
    DrawTexture(*texture, 0, 0, WHITE);
    panic("not implemented");
}

static void draw_objects(tmx_object_group *objectGroup) {
    if (objectGroup == nil || objectGroup->head == nil) {
        printf("no objects found\n");
        return;
    }

    Color color = int_to_color(objectGroup->color);
    tmx_object *head = objectGroup->head;
    while (head) {
        if (head->visible) {
            switch (head->obj_type) {
                case OT_NONE:
                    panic("draw_object->type == OT_NONE not implemented");
                    break;
                case OT_SQUARE:
                    DrawRectangleLinesEx(
                        (Rectangle) {head->x, head->y, head->width, head->height},
                        LINE_THICKNESS,
                        color
                    );
                    break;
                case OT_POLYGON:
                    draw_polygon(
                        head->x,
                        head->y,
                        head->content.shape->points,
                        head->content.shape->points_len,
                        color
                    );
                    break;
                case OT_POLYLINE:
                    draw_polyline(
                        head->x,
                        head->y,
                        head->content.shape->points,
                        head->content.shape->points_len,
                        color
                    );
                    break;
                case OT_ELLIPSE:
                    DrawEllipseLines(
                        head->x + head->width / 2.0,
                        head->y + head->height / 2.0,
                        head->width / 2.0,
                        head->height / 2.0, color
                    );
                    break;
                case OT_TILE:
                    panic("draw_object->type == OT_TILE not implemented");
                    break;
                case OT_TEXT:
                    panic("draw_object->type == OT_TEXT not implemented");
                    break;
                case OT_POINT:
                    panic("draw_object->type == OT_POINT not implemented");
                    break;
            }
        }
        head = head->next;
    }
}

static void draw_polyline(double offsetX, double offsetY, double **points, int pointsCount, Color color) {
    printf("draw_polyline\n");
    panic("not implemented");

    for (i32 i = 1; i < pointsCount; i++) {
        DrawLineEx(
            (Vector2) {offsetX + points[i - 1][0], offsetY + points[i - 1][1]},
            (Vector2) {offsetX + points[i][0], offsetY + points[i][1]},
            LINE_THICKNESS,
            color
        );
    }

}

static void draw_polygon(double offsetX, double offsetY, double **points, int pointsCount, Color color) {
    printf("draw_polygon\n");
    panic("not implemented");

    draw_polyline(offsetX, offsetY, points, pointsCount, color);
    if (pointsCount > 2) {
        DrawLineEx(
            (Vector2) {offsetX + points[0][0], offsetY + points[0][1]},
            (Vector2) {offsetX + points[pointsCount - 1][0], offsetY + points[pointsCount - 1][1]},
            LINE_THICKNESS,
            color
        );
    }
}


static void *texture_loader_callback(const char *path) {
    Texture2D *text = calloc(1, sizeof(*text));
    *text = LoadTexture(path);
    printf("loaded %s with ID %d\n", path, text->id);
    return text;
}

static void texture_free_callback(void *ptr) {
    printf("unloading texture #%d\n", ((Texture2D *) ptr)->id);
    UnloadTexture(*(Texture2D *) ptr);
    free(ptr);
}

