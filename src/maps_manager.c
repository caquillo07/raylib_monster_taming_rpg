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

#include <raylib.h>
#include <tmx.h>

#include "game.h"

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
static bool collidesWithCamera(Rectangle rect);

static Color int_to_color(u32 color);

static AnimatedTexturesSprite *init_water_sprites(const tmx_layer *layer);
static AnimatedTiledSprite *init_coast_line_sprites(const tmx_layer *layer);
static Sprite *init_monster_encounter_sprites(const tmx_layer *layer);

Character *init_over_world_characters(const tmx_layer *layer);
static void draw_layer(const tmx_map *tmap, const tmx_layer *layer);
static void draw_objects_layer(const tmx_map *tmap, const tmx_layer *layer);
static void draw_water_sprites(AnimatedTexturesSprite *waterSprites);
static void draw_coast_line_sprites(AnimatedTiledSprite *coastLineSprites);
static void draw_tile(void *imageTexture2D, Rectangle sourceRec, Vector2 destination, f32 opacity);

void maps_manager_init() {
    tmx_img_load_func = texture_loader_callback;
    tmx_img_free_func = texture_free_callback;

    tmx_alloc_func = map_alloc_callback;
    tmx_free_func = map_free_callback;

    loaded = true;
    atexit(print_map_total_memory);
}

static Color int_to_color(const u32 color) {
    const tmx_col_bytes res = tmx_col_to_bytes(color);
    return (Color){
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

Map *load_map(const MapID mapID) {
    panicIf(!loaded, "maps_manager was initialize, forgot to call maps_manager_init()?");
    panicIf(mapID >= MapIDMax, "map ID provided is invalid");
    const MapInfo mapInfo = mapAtlas[mapID];

    Map *map = mallocate(sizeof(*map), MemoryTagGame);
    panicIfNil(map, "failed to alloc map");

    map->id = mapID;
    map->tiledMap = tmx_load(mapInfo.mapFilePath);
    panicIfNil(map->tiledMap, "tmx_error: %s", tmx_strerr());


    // in a real game, this wouldn't work? The way the maps are set up, every map
    // has the same layers, so it works, in a real game, that may or may not
    // be the same. Depends on how I set it up

    // terrain
    map->terrainLayer = tmx_find_layer_by_name(map->tiledMap, "Terrain");
    panicIfNil(map->terrainLayer, "could not find the Terrain layer in tmx map");

    // terrain top
    map->terrainTopLayer = tmx_find_layer_by_name(map->tiledMap, "Terrain Top");
    panicIfNil(map->terrainTopLayer, "could not find the Terrain Top layer in tmx map");

    // grass for random ecounters
    map->monsterEcounterLayer = tmx_find_layer_by_name(map->tiledMap, "Monsters");
    panicIfNil(map->monsterEcounterLayer, "could not find the Monsters layer in tmx map");

    // entities
    map->entitiesLayer = tmx_find_layer_by_name(map->tiledMap, "Entities");
    panicIfNil(map->entitiesLayer, "could not find the Entities layer in tmx map");

    // objects
    map->objectsLayer = tmx_find_layer_by_name(map->tiledMap, "Objects");
    panicIfNil(map->objectsLayer, "could not find the Objects layer in the tmx map");

    // Water
    map->waterLayer = tmx_find_layer_by_name(map->tiledMap, "Water");
    panicIfNil(map->waterLayer, "could not find the Objects layer in the tmx map");

    // Coast line
    map->coastLineLayer = tmx_find_layer_by_name(map->tiledMap, "Coast");
    panicIfNil(map->waterLayer, "could not find the Coast layer in the tmx map");

    // sprites
    map->monsterEncounterSpritesList = init_monster_encounter_sprites(map->monsterEcounterLayer);
    map->waterSpritesList = init_water_sprites(map->waterLayer);
    map->coastLineSpritesList = init_coast_line_sprites(map->coastLineLayer);
    map->overWorldCharacters = init_over_world_characters(map->entitiesLayer);

    // get player starting position
    const tmx_object *housePlayerObject = tmx_find_object_by_id(map->tiledMap, mapInfo.startingPositionObjectID);
    map->playerStartingPosition = (Vector2){
        .x = (f32) housePlayerObject->x,
        .y = (f32) housePlayerObject->y,
    };
    return map;
}

void map_free(Map *map) {
    array_free(map->waterSpritesList);
    map->waterSpritesList = nil;

    array_range(map->coastLineSpritesList, i) {
        array_free(map->coastLineSpritesList[i].sourceFrames);
    }
    array_free(map->coastLineSpritesList);
    map->coastLineSpritesList = nil;

    array_range(map->overWorldCharacters, i) {
        character_free(&map->overWorldCharacters[i]);
    }
    array_free(map->overWorldCharacters);
    map->overWorldCharacters = nil;

    array_free(map->monsterEncounterSpritesList);

    // LibTMX
    // todo - add to memory/memory.h
    tmx_map_free(map->tiledMap);

    map->terrainLayer = nil;
    map->terrainTopLayer = nil;
    map->entitiesLayer = nil;
    map->objectsLayer = nil;
    map->waterLayer = nil;
    map->monsterEcounterLayer = nil;
    mfree(map, sizeof(*map), MemoryTagGame);
}

static Sprite *init_monster_encounter_sprites(const tmx_layer *layer) {
    Sprite *sprites = nil;
    const tmx_object *monsterTileH = layer->content.objgr->head;
    while (monsterTileH) {
        if (!monsterTileH->visible) {
            monsterTileH = monsterTileH->next;
            continue;
        }
        const tmx_property *biomeProp = tmx_get_property(monsterTileH->properties, "biome");
        Texture2D texture;
        WorldLayer worldLayer = WorldLayerMain;
        if (streq(biomeProp->value.string, "ice")) {
            texture = assets.iceGrassTexture;
        } else if (streq(biomeProp->value.string, "forest")) {
            texture = assets.grassTexture;
        } else if (streq(biomeProp->value.string, "sand")) {
            texture = assets.sandTexture;
            worldLayer = WorldLayerBackground;
        } else {
            panic("init_monster_encounter_sprites - biome prop '%s' not supported", biomeProp->value.string);
        }

        const f32 yPos = monsterTileH->y - monsterTileH->width;
        const Sprite s = {
            .id = texture.id,
            .texture = texture,
            .position = {
                .x = monsterTileH->x,
                .y = yPos,
            },
            .width = monsterTileH->width,
            .height = monsterTileH->height,
            .sourceFrame = {
                .height = monsterTileH->height,
                .width = monsterTileH->width,
            },
            .layer = worldLayer,
            .ySort = yPos - 40,
        };
        array_push(sprites, s);
        monsterTileH = monsterTileH->next;
    }
    return sprites;
}

static AnimatedTexturesSprite *init_water_sprites(const tmx_layer *layer) {
    AnimatedTexturesSprite *animatedSprite = nil;
    const tmx_object *waterTileH = layer->content.objgr->head;
    while (waterTileH) {
        if (!waterTileH->visible) {
            waterTileH = waterTileH->next;
            continue;
        }

        for (i32 x = (i32) waterTileH->x; x < waterTileH->x + waterTileH->width; x += TILE_SIZE) {
            for (i32 y = (i32) waterTileH->y; y < waterTileH->y + waterTileH->height; y += TILE_SIZE) {
                const AnimatedTexturesSprite waterSprite = {
                    .id = waterTileH->id,
                    .position = {.x = (f32) x, .y = (f32) y},
                    .textures = assets.waterTextures.texturesList,
                    .framesLen = 4,
                    .currentFrame = 0,
                    .frameTimer = 0.f,
                    .animationSpeed = settings.waterAnimationSpeed,
                    .layer = WorldLayerWater,
                };
                array_push(animatedSprite, waterSprite);
            }
        }

        waterTileH = waterTileH->next;
    }

    return animatedSprite;
}

Character *init_over_world_characters(const tmx_layer *layer) {
    Character *characters = nil;
    const tmx_object *characterH = layer->content.objgr->head;
    const i32 maxStringValSize = strlen("water_boss.png");
    while (characterH) {
        if (!characterH->visible || strncmp(characterH->name, "Player", strlen("Player")) == 0) {
            characterH = characterH->next;
            continue;
        }

        const tmx_property *directionProp = tmx_get_property(characterH->properties, "direction");
        //        tmx_property *posProp = tmx_get_property(characterH->properties, "pos");
        const tmx_property *graphicProp = tmx_get_property(characterH->properties, "graphic");

        TileMapID characterTiledMapID = -1;
        if (strncmp(graphicProp->value.string, "blond", maxStringValSize) == 0) {
            characterTiledMapID = TileMapIDBlondCharacter;
        } else if (strncmp(graphicProp->value.string, "fire_boss", maxStringValSize) == 0) {
            characterTiledMapID = TileMapIDFireBossCharacter;
        } else if (strncmp(graphicProp->value.string, "grass_boss", maxStringValSize) == 0) {
            characterTiledMapID = TileMapIDGrassBossCharacter;
        } else if (strncmp(graphicProp->value.string, "hat_girl", maxStringValSize) == 0) {
            characterTiledMapID = TileMapIDHatGirlCharacter;
        } else if (strncmp(graphicProp->value.string, "purple_girl", maxStringValSize) == 0) {
            characterTiledMapID = TileMapIDPurpleGirlCharacter;
        } else if (strncmp(graphicProp->value.string, "straw", maxStringValSize) == 0) {
            characterTiledMapID = TileMapIDStrawCharacter;
        } else if (strncmp(graphicProp->value.string, "water_boss", maxStringValSize) == 0) {
            characterTiledMapID = TileMapIDWaterBossCharacter;
        } else if (strncmp(graphicProp->value.string, "young_girl", maxStringValSize) == 0) {
            characterTiledMapID = TileMapIDYoungGirlCharacter;
        } else if (strncmp(graphicProp->value.string, "young_guy", maxStringValSize) == 0) {
            characterTiledMapID = TileMapIDYoungGuyCharacter;
        } else {
            panic("unexpected graphics property for entity %s", graphicProp->value.string);
        }

        CharacterDirection direction = {0};
        if (strncmp(directionProp->value.string, "down", maxStringValSize) == 0) {
            direction = CharacterDirectionDown;
        } else if (strncmp(directionProp->value.string, "up", maxStringValSize) == 0) {
            direction = CharacterDirectionUp;
        } else if (strncmp(directionProp->value.string, "right", maxStringValSize) == 0) {
            direction = CharacterDirectionRight;
        } else if (strncmp(directionProp->value.string, "left", maxStringValSize) == 0) {
            direction = CharacterDirectionLeft;
        } else {
            panic("unexpected direction property for entity: %s", directionProp->value.string);
        }

        Character character = character_new(
            (Vector2){},
            characterTiledMapID,
            direction
        );

        character_set_center_at(&character, (Vector2){.x = (f32) characterH->x, .y = (f32) characterH->y});
        array_push(characters, character);
        characterH = characterH->next;
    }

    return characters;
}

static AnimatedTiledSprite *init_coast_line_sprites(const tmx_layer *layer) {
    AnimatedTiledSprite *spritesList = nil;

    const tmx_object *coastLineH = layer->content.objgr->head;
    while (coastLineH) {
        if (!coastLineH->visible) {
            coastLineH = coastLineH->next;
            continue;
        }

        const tmx_property *sideProp = tmx_get_property(coastLineH->properties, "side");
        panicIfNil(sideProp, "expected coast line object ot have 'side' property");

        // todo - use enums? .-.
        //  this needs to be reworked
        int colBase = 0, rowBase = 0;
        if (streq(sideProp->value.string, "topleft")) {
            colBase = 0;
            rowBase = 0;
        } else if (streq(sideProp->value.string, "top")) {
            colBase = 1;
            rowBase = 0;
        } else if (streq(sideProp->value.string, "topright")) {
            colBase = 2;
            rowBase = 0;
        } else if (streq(sideProp->value.string, "left")) {
            colBase = 0;
            rowBase = 1;
        } else if (streq(sideProp->value.string, "right")) {
            colBase = 2;
            rowBase = 1;
        } else if (streq(sideProp->value.string, "bottomleft")) {
            colBase = 0;
            rowBase = 2;
        } else if (streq(sideProp->value.string, "bottom")) {
            colBase = 1;
            rowBase = 2;
        } else if (streq(sideProp->value.string, "bottomright")) {
            colBase = 2;
            rowBase = 2;
        }

        int firstFrameCol = 0, firstFrameRow = 0;
        const tmx_property *terrainProp = tmx_get_property(coastLineH->properties, "terrain");
        panicIfNil(terrainProp, "expected coast line object ot have 'terrain' property");

        if (streq(terrainProp->value.string, "grass")) {
            firstFrameCol = colBase + 0;
            firstFrameRow = rowBase + 0;
        } else if (streq(terrainProp->value.string, "grass_i")) {
            firstFrameCol = colBase + 1 * 3;
            firstFrameRow = rowBase + 0;
        } else if (streq(terrainProp->value.string, "sand_i")) {
            firstFrameCol = colBase + 2 * 3;
            firstFrameRow = rowBase + 0;
        } else if (streq(terrainProp->value.string, "sand")) {
            firstFrameCol = colBase + 3 * 3;
            firstFrameRow = rowBase + 0;
        } else if (streq(terrainProp->value.string, "rock")) {
            firstFrameCol = colBase + 4 * 3;
            firstFrameRow = rowBase + 0;
        } else if (streq(terrainProp->value.string, "rock_i")) {
            firstFrameCol = colBase + 5 * 3;
            firstFrameRow = rowBase + 0;
        } else if (streq(terrainProp->value.string, "ice")) {
            firstFrameCol = colBase + 6 * 3;
            firstFrameRow = rowBase + 0;
        } else if (streq(terrainProp->value.string, "ice_i")) {
            firstFrameCol = colBase + 7 * 3;
            firstFrameRow = rowBase + 0;
        }

        const TileMap coastLineTileMap = assets.tileMaps[TileMapIDCoastLine];
        Rectangle *sourceFrames = nil;
        array_push(sourceFrames, tile_map_get_frame_at(coastLineTileMap, firstFrameCol, firstFrameRow));
        array_push(sourceFrames, tile_map_get_frame_at(coastLineTileMap, firstFrameCol, firstFrameRow + 1 * 3));
        array_push(sourceFrames, tile_map_get_frame_at(coastLineTileMap, firstFrameCol, firstFrameRow + 2 * 3));
        array_push(sourceFrames, tile_map_get_frame_at(coastLineTileMap, firstFrameCol, firstFrameRow + 3 * 3));

        const AnimatedTiledSprite sprite = {
            .id = coastLineH->id,
            .position = {.x = (f32) coastLineH->x, .y = (f32) coastLineH->y},
            .texture = coastLineTileMap.texture,
            .framesLen = 4,
            .currentFrame = 0,
            .frameTimer = 0.f,
            .animationSpeed = settings.coastLineAnimationSpeed,
            .sourceFrames = sourceFrames,
            .ySort = WorldLayerWater
        };

        array_push(spritesList, sprite);
        coastLineH = coastLineH->next;
    }

    return spritesList;
}

void map_update(const Map *map, const f32 dt) {
    for (i32 i = 0; i < array_length(map->waterSpritesList); i++) {
        update_animated_textures_sprite(&map->waterSpritesList[i], dt);
    }

    array_range(map->coastLineSpritesList, i) {
        update_animated_tiled_sprite(&map->coastLineSpritesList[i], dt);
    }

    array_range(map->overWorldCharacters, i) {
        character_update(&map->overWorldCharacters[i], dt);
    }
}

void map_draw(const Map *map) {
    ClearBackground(int_to_color(map->tiledMap->backgroundcolor));

    draw_layer(map->tiledMap, map->terrainLayer);
    // todo(hector) - why is this again?
    // draw_layer(map->tiledMap, map->terrainTopLayer);
    draw_objects_layer(map->tiledMap, map->objectsLayer);
    draw_water_sprites(map->waterSpritesList);
    draw_coast_line_sprites(map->coastLineSpritesList);
    array_range(map->overWorldCharacters, i) {
        character_draw(&map->overWorldCharacters[i]);
    }
    array_range(map->monsterEncounterSpritesList, i) {
        const Sprite s = map->monsterEncounterSpritesList[i];
        DrawTextureRec(s.texture, s.sourceFrame, s.position, WHITE);
    }
}

static void draw_layer(const tmx_map *tmap, const tmx_layer *layer) {
    if (layer == nil || layer->name == nil) {
        printf("no layer found\n");
        return;
    }
    // todo(hector) - this shit is slow as it gets, 5ms spent on this
    //  for main terrain layer
    for (u64 i = 0; i < tmap->height; i++) {
        for (u64 j = 0; j < tmap->width; j++) {
            const u32 gid = (layer->content.gids[(i * tmap->width) + j]) & TMX_FLIP_BITS_REMOVAL;
            if (tmap->tiles[gid] != NULL) {
                const tmx_tileset *tileSet = tmap->tiles[gid]->tileset;
                void *image = tileSet->image->resource_image;

                if (tmap->tiles[gid]->image) {
                    image = tmap->tiles[gid]->image->resource_image;
                }

                // i32 flags = (layer->content.gids[(i * tmap->width) + j]) & ~TMX_FLIP_BITS_REMOVAL;
                const Rectangle sourceRec = {
                    .x = (f32) tmap->tiles[gid]->ul_x,
                    .y = (f32) tmap->tiles[gid]->ul_y,
                    .width = (f32) tileSet->tile_width,
                    .height = (f32) tileSet->tile_height,
                };
                const Vector2 destination = {
                    .x = (f32) (j * tileSet->tile_width),
                    .y = (f32) (i * tileSet->tile_height),
                };
                const Rectangle boundBox = {
                    .x = destination.x,
                    .y = destination.y,
                    .height = sourceRec.height,
                    .width = sourceRec.width,
                };

                if (!collidesWithCamera(boundBox)) {
                    continue;
                }
                draw_tile(image, sourceRec, destination, layer->opacity);
            }
        }
    }
}

static void draw_tile(void *imageTexture2D, const Rectangle sourceRec, const Vector2 destination, const f32 opacity) {
    const u8 c = (u8) opacity * 255;
    const Color tint = {c, c, c, c};
    DrawTextureRec(*(Texture2D *) imageTexture2D, sourceRec, destination, tint);
}

static void draw_object_tile(const tmx_map *tmap, const tmx_object *object) {
    if (tmap == nil || object == nil) {
        slogw("tmap or object are nil in draw_object_tile");
        return;
    }

    const i32 gid = object->content.gid;
    const tmx_tile *tile = tmap->tiles[gid];
    panicIfNil(tile, "expected to find tileSet for object");
    panicIfNil(tile->image, "object does contain image");
    panicIfNil(tile->image->resource_image, "object does contain image data");

    const Rectangle sourceRec = {
        .x = (f32) tile->ul_x,
        .y = (f32) tile->ul_y,
        .width = (f32) object->width,
        .height = (f32) object->height,
    };
    const Vector2 destination = {
        .x = (f32) object->x,
        .y = (f32) object->y - sourceRec.height,
    };

    const Rectangle boundingBox = {
        .x = destination.x,
        .y = destination.y,
        .width = sourceRec.width,
        .height = sourceRec.height,
    };
    if (!collidesWithCamera(boundingBox)) {
        return;
    }

    draw_tile(tile->image->resource_image, sourceRec, destination, 1.f); // todo opacity?

    // draw debug frames
    if (!game.isDebug) { return; }

    const Rectangle tileFrame = {
        .x = destination.x,
        .y = destination.y,
        .width = sourceRec.width,
        .height = sourceRec.height,
    };
    DrawRectangleLinesEx(tileFrame, 3.f, RED);
    DrawCircleV(destination, 5.f, RED);
}

static void draw_objects_layer(const tmx_map *tmap, const tmx_layer *layer) {
    if (layer == nil || layer->name == nil) {
        printf("no layer objects found\n");
        return;
    }

    const tmx_object *objectHead = layer->content.objgr->head;
    while (objectHead) {
        if (!objectHead->visible) {
            objectHead = objectHead->next;
            continue;
        }
        switch (objectHead->obj_type) {
            case OT_TILE:
                draw_object_tile(tmap, objectHead);
                break;
            case OT_NONE:
                panic("draw_object_tile->type == OT_NONE not implemented");
            case OT_SQUARE:
                panic("draw_object_tile->type == OT_SQUARE not implemented");
            case OT_POLYGON:
                panic("draw_object_tile->type == OT_POLYGON not implemented");
            case OT_POLYLINE:
                panic("draw_object_tile->type == OT_POLYLINE not implemented");
            case OT_ELLIPSE:
                panic("draw_object_tile->type == OT_ELLIPSE not implemented");
            case OT_TEXT:
                panic("draw_object_tile->type == OT_TEXT not implemented");
            case OT_POINT:
                panic("draw_object_tile->type == OT_POINT not implemented");
        }
        objectHead = objectHead->next;
    }
}

static bool collidesWithCamera(const Rectangle rect) {
    return CheckCollisionRecs(game.cameraBoundingBox, rect);
}

static void draw_coast_line_sprites(AnimatedTiledSprite *coastLineSprites) {
    if (array_length(coastLineSprites) == 0) { return; }

    array_range(coastLineSprites, i) {
        const AnimatedTiledSprite coastSprite = coastLineSprites[i];
        const Rectangle coastSpriteBoundingBox = {
            .x = coastSprite.position.x,
            .y = coastSprite.position.y,
            .width = coastSprite.sourceFrames[0].width,
            .height = coastSprite.sourceFrames[0].height,
        };

        if (!collidesWithCamera(coastSpriteBoundingBox)) {
            continue;
        }
        const Rectangle tileToDraw = coastSprite.sourceFrames[coastSprite.currentFrame];
        draw_tile(&assets.tileMaps[TileMapIDCoastLine].texture, tileToDraw, coastSprite.position, 1.f);

        // draw debug frames
        if (!game.isDebug) { continue; }

        DrawRectangleLinesEx(coastSpriteBoundingBox, 3.f, RED);
        DrawCircleV(coastSprite.position, 5.f, RED);
    }
}

static void draw_water_sprites(AnimatedTexturesSprite *waterSprites) {
    if (array_length(waterSprites) == 0) { return; }

    for (i32 i = 0; i < array_length(waterSprites); i++) {
        const AnimatedTexturesSprite waterSprite = waterSprites[i];
        const Rectangle sourceRec = {
            .x = waterSprite.position.x,
            .y = waterSprite.position.y,
            .height = (f32) waterSprite.textures[0].height,
            .width = (f32) waterSprite.textures[0].width,
        };
        if (!collidesWithCamera(sourceRec)) {
            continue;
        }

        Texture2D *frameToDraw = &waterSprite.textures[waterSprite.currentFrame];
        draw_tile(
            frameToDraw,
            sourceRec,
            waterSprite.position,
            1.f
        );

        // draw debug frames
        if (!game.isDebug) { continue; }

        DrawRectangleLinesEx(sourceRec, 3.f, RED);
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
    // slogi("allocating %d bytes for map, total: %d", len, totalMapMemoryAllocated);
    return realloc(ptr, len);
}

void map_free_callback(void *ptr) {
    free(ptr);
}

void print_map_total_memory() {
    slogi("total memory allocated by TmxLib: %d bytes", totalMapMemoryAllocated);
}
