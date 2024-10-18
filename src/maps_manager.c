//
// Created by Hector Mejia on 5/10/24.
//

#include "maps_manager.h"
#include "array/array.h"
#include "assets.h"
#include "common.h"
#include "memory/memory.h"
#include "settings.h"
#include "sprites.h"

#include <raylib.h>
#include <tmx.h>

#include "game.h"
#include "game_data.h"
#include "raylib_extras.h"

#define maps_dir "./data/maps/"
#define map_path(MAP_NAME) maps_dir #MAP_NAME

static MapInfo mapAtlas[MapIDMax] = {
	{
		.id = MapIDWorld,
		.name = "world",
		.mapFilePath = map_path(world.tmx),
		.startingPositionObjectID = 12,
	},
	{
		.id = MapIDHospital,
		.name = "hospital",
		.mapFilePath = map_path(hospital.tmx),
		.startingPositionObjectID = 1,
	},
	{
		.id = MapIDHouse,
		.name = "house",
		.mapFilePath = map_path(house.tmx),
		.startingPositionObjectID = 1,
	},
	{
		.id = MapIDArena,
		.name = "arena",
		.mapFilePath = map_path(arena.tmx),
		.startingPositionObjectID = 854,
	},
	{
		.id = MapIDFire,
		.name = "fire",
		.mapFilePath = map_path(fire.tmx),
		.startingPositionObjectID = 854,
	},
	{
		.id = MapIDPlant,
		.name = "plant",
		.mapFilePath = map_path(plant.tmx),
		.startingPositionObjectID = 854,
	},
	{
		.id = MapIDWater,
		.name = "water",
		.mapFilePath = map_path(water.tmx),
		.startingPositionObjectID = 854,
	},
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
static void init_monster_encounter_sprites(Map *map, const tmx_layer *layer);
static void init_object_sprites(Map *map, const tmx_layer *layer);
static void init_terrain_sprites(Map *map, const tmx_layer *layer, bool isTopLayer);
Character *init_over_world_characters(const tmx_layer *layer);
static void init_collision_sprites(Map *map, const tmx_layer *layer);
static void init_transition_sprites(Map *map, const tmx_layer *layer);
_comptime_unused_ static Vector2 find_player_position(const tmx_layer *layer);

static void draw_static_sprite(StaticSprite sprite);
static void draw_animated_textures_sprites(AnimatedTexturesSprite *waterSprites);
static void draw_animated_tiled_sprites(AnimatedTiledSprite *coastLineSprites);
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

static int compare_entities(const void *a, const void *b) {
	const Entity entityA = *((Entity *)a);
	const Entity entityB = *((Entity *)b);

	if (entityA.ySort == entityB.ySort && entityA.layer == entityB.layer) {
		return 0;
	}

	// the layer check is a hack to make up for the inconsistent map design
	if (entityA.ySort < entityB.ySort || entityA.layer < entityB.layer) {
		return -1;
	}

	return 1;
}

Map *load_map(const MapID mapID) {
	panicIf(!loaded, "maps_manager was initialize, forgot to call maps_manager_init()?");
	panicIf(mapID >= MapIDMax, "map ID provided is invalid");
	const MapInfo mapInfo = mapAtlas[mapID];

	Map *map = mallocate(sizeof(*map), MemoryTagGame);
	panicIfNil(map, "failed to alloc map");

	map->id = mapID;
	map->backgroundSprites = nil;
	map->mainSprites = nil;
	map->foregroundSprites = nil;
	map->transitionBoxes = nil;

	map->tiledMap = tmx_load(mapInfo.mapFilePath);
	panicIfNil(map->tiledMap, "tmx_error: %s", tmx_strerr());

	// in a real game, this wouldn't work? The way the maps are set up, every map
	// has the same layers, so it works, in a real game, that may or may not
	// be the same. Depends on how I set it up

	// terrain
	const tmx_layer *terrainLayer = tmx_find_layer_by_name(map->tiledMap, "Terrain");
	panicIfNil(terrainLayer, "could not find the Terrain layer in tmx map");

	// terrain top
	const tmx_layer *terrainTopLayer = tmx_find_layer_by_name(map->tiledMap, "Terrain Top");
	panicIfNil(terrainTopLayer, "could not find the Terrain Top layer in tmx map");

	// grass for random encounters
	const tmx_layer *monsterEncounterLayer = tmx_find_layer_by_name(map->tiledMap, "Monsters");
	panicIfNil(monsterEncounterLayer, "could not find the Monsters layer in tmx map");

	// entities
	const tmx_layer *entitiesLayer = tmx_find_layer_by_name(map->tiledMap, "Entities");
	panicIfNil(entitiesLayer, "could not find the Entities layer in tmx map");

	// objects
	const tmx_layer *objectsLayer = tmx_find_layer_by_name(map->tiledMap, "Objects");
	panicIfNil(objectsLayer, "could not find the Objects layer in the tmx map");

	// collisions
	const tmx_layer *collisionsLayer = tmx_find_layer_by_name(map->tiledMap, "Collisions");
	panicIfNil(collisionsLayer, "could not find the Collissions layer in the tmx map");

	// transitions
	const tmx_layer *transitionsLayer = tmx_find_layer_by_name(map->tiledMap, "Transition");
	panicIfNil(transitionsLayer, "could not find the Transition layer in the tmx map");

	// Water
	const tmx_layer *waterLayer = tmx_find_layer_by_name(map->tiledMap, "Water");
	panicIfNil(waterLayer, "could not find the Objects layer in the tmx map");

	// Coast line
	const tmx_layer *coastLineLayer = tmx_find_layer_by_name(map->tiledMap, "Coast");
	panicIfNil(coastLineLayer, "could not find the Coast layer in the tmx map");

	// sprites
	map->waterSpritesList = init_water_sprites(waterLayer);
	map->coastLineSpritesList = init_coast_line_sprites(coastLineLayer);
	map->overWorldCharacters = init_over_world_characters(entitiesLayer);

	init_monster_encounter_sprites(map, monsterEncounterLayer);
	init_terrain_sprites(map, terrainLayer, false);
	init_terrain_sprites(map, terrainTopLayer, true);
	init_object_sprites(map, objectsLayer);
	init_collision_sprites(map, collisionsLayer);
	init_transition_sprites(map, transitionsLayer);

	game.gameMetrics.totalSprites += array_length(map->waterSpritesList) +
									 array_length(map->coastLineSpritesList) +
									 array_length(map->overWorldCharacters) +
									 array_length(map->backgroundSprites) +
									 array_length(map->mainSprites) +
									 array_length(map->foregroundSprites);

	// y-sort sprites
	// (todo) - this is potentially bad, if the sprites structs change, this would
	//  blow up and im too lazy to make generic structs
	qsort(
		map->backgroundSprites,
		array_length(map->backgroundSprites),
		sizeof(map->backgroundSprites[0]),
		compare_entities
	);
	qsort(
		map->mainSprites,
		array_length(map->mainSprites),
		sizeof(map->mainSprites[0]),
		compare_entities
	);
	qsort(
		map->foregroundSprites,
		array_length(map->foregroundSprites),
		sizeof(map->foregroundSprites[0]),
		compare_entities
	);

	// get player starting position
	const tmx_object *startingPlayerObject = tmx_find_object_by_id(map->tiledMap, mapInfo.startingPositionObjectID);
	panicIfNil(startingPlayerObject);
	map->playerStartingPosition = (Vector2){
		.x = (f32)startingPlayerObject->x,
		.y = (f32)startingPlayerObject->y,
	};
	// map->playerStartingPosition = find_player_position(entitiesLayer);
	return map;
}

void map_free(Map *map) {
	// todo(hector) - free new generic sprites array
	array_free(map->transitionBoxes);
	array_free(map->collisionBoxes);
	array_free(map->backgroundSprites);
	array_free(map->mainSprites);
	array_free(map->foregroundSprites);

	array_free(map->waterSpritesList);
	map->waterSpritesList = nil;

	array_free(map->coastLineSpritesList);
	map->coastLineSpritesList = nil;

	array_range(map->overWorldCharacters, i) {
		character_free(&map->overWorldCharacters[i]);
	}
	array_free(map->overWorldCharacters);
	map->overWorldCharacters = nil;

	// LibTMX
	// todo - add to memory/memory.h
	tmx_map_free(map->tiledMap);

	mfree(map, sizeof(*map), MemoryTagGame);
}

MapID map_id_for_name(const char *name) {
	for (u32 i = 0; i < comptime_array_len(mapAtlas); i++) {
		if (streq(name, mapAtlas[i].name)) {
			return mapAtlas[i].id;
		}
	}
	panic("invalid map requested \"%s\"", name);
	return 0;
}

static void init_monster_encounter_sprites(Map *map, const tmx_layer *layer) {
	const tmx_object *monsterTileH = layer->content.objgr->head;
	while (monsterTileH) {
		if (!monsterTileH->visible) {
			monsterTileH = monsterTileH->next;
			continue;
		}
		const tmx_property *biomeProp = tmx_get_property(monsterTileH->properties, "biome");
		Texture2D texture = {};
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
		const StaticSprite s = {
			.entity = {
				.id = texture.id,
				.layer = worldLayer,
				.ySort = yPos + (monsterTileH->height / 2) - 20,
				.position = {
					.x = monsterTileH->x,
					.y = yPos,
				},
			},
			.texture = texture,
			.width = monsterTileH->width,
			.height = monsterTileH->height,
			.sourceFrame = {
				.height = monsterTileH->height,
				.width = monsterTileH->width,
			},
			.type = StaticSpriteTypeMonsterEncounter,
		};
		if (worldLayer == WorldLayerMain) {
			array_push(map->mainSprites, s);
			// ReSharper disable once CppDFAConstantConditions
		} else if (worldLayer == WorldLayerBackground) {
			array_push(map->backgroundSprites, s);
		}
		monsterTileH = monsterTileH->next;
	}
}

static AnimatedTexturesSprite *init_water_sprites(const tmx_layer *layer) {
	AnimatedTexturesSprite *animatedSprite = nil;
	const tmx_object *waterTileH = layer->content.objgr->head;
	while (waterTileH) {
		if (!waterTileH->visible) {
			waterTileH = waterTileH->next;
			continue;
		}

		for (i32 x = (i32)waterTileH->x; x < waterTileH->x + waterTileH->width; x += TILE_SIZE) {
			for (i32 y = (i32)waterTileH->y; y < waterTileH->y + waterTileH->height; y += TILE_SIZE) {
				const AnimatedTexturesSprite waterSprite = {
					.entity = {
						.id = waterTileH->id,
						.position = {.x = (f32)x, .y = (f32)y},
						.layer = WorldLayerWater,
						.ySort = (f32)y,
					},
					.textures = assets.waterTextures.texturesList,
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

_comptime_unused_
static Vector2 find_player_position(const tmx_layer *layer) {
	const tmx_object *characterH = layer->content.objgr->head;
	while (characterH) {
		if (streq(characterH->name, "Player")) {
			return (Vector2){.x = (f32)characterH->x, .y = (f32)characterH->y};
		}
	}

	panic("did not find the player position in layer");
	return (Vector2){};
}

Character *init_over_world_characters(const tmx_layer *layer) {
	Character *characters = nil;
	const tmx_object *characterH = layer->content.objgr->head;
	while (characterH) {
		if (!characterH->visible || streq(characterH->name, "Player")) {
			characterH = characterH->next;
			continue;
		}

		const tmx_property *directionProp = tmx_get_property(characterH->properties, "direction");
		const tmx_property *characterIDProp = tmx_get_property(characterH->properties, "character_id");
		const tmx_property *graphicProp = tmx_get_property(characterH->properties, "graphic");
		const tmx_property *radiusProp = tmx_get_property(characterH->properties, "radius");

		TileMap characterTiledMapID = {};
		if (streq(graphicProp->value.string, "blond")) {
			characterTiledMapID = assets.tileMaps.blondCharacter;
		} else if (streq(graphicProp->value.string, "fire_boss")) {
			characterTiledMapID = assets.tileMaps.fireBossCharacter;
		} else if (streq(graphicProp->value.string, "grass_boss")) {
			characterTiledMapID = assets.tileMaps.grassBossCharacter;
		} else if (streq(graphicProp->value.string, "hat_girl")) {
			characterTiledMapID = assets.tileMaps.hatGirlCharacter;
		} else if (streq(graphicProp->value.string, "purple_girl")) {
			characterTiledMapID = assets.tileMaps.purpleGirlCharacter;
		} else if (streq(graphicProp->value.string, "straw")) {
			characterTiledMapID = assets.tileMaps.strawCharacter;
		} else if (streq(graphicProp->value.string, "water_boss")) {
			characterTiledMapID = assets.tileMaps.waterBossCharacter;
		} else if (streq(graphicProp->value.string, "young_girl")) {
			characterTiledMapID = assets.tileMaps.youngGirlCharacter;
		} else if (streq(graphicProp->value.string, "young_guy")) {
			characterTiledMapID = assets.tileMaps.youngGuyCharacter;
		}

		if (!IsTextureReady(characterTiledMapID.texture)) {
			panic("unexpected graphics property for entity %s", graphicProp->value.string);
		}

		CharacterDirection direction = CharacterDirectionNone;
		if (streq(directionProp->value.string, "down")) {
			direction = CharacterDirectionDown;
		} else if (streq(directionProp->value.string, "up")) {
			direction = CharacterDirectionUp;
		} else if (streq(directionProp->value.string, "right")) {
			direction = CharacterDirectionRight;
		} else if (streq(directionProp->value.string, "left")) {
			direction = CharacterDirectionLeft;
		}
		if (direction == CharacterDirectionNone) {
			panic("unexpected direction property for entity: %s", directionProp->value.string);
		}

		Character character = character_new(
			(Vector2){},
			characterTiledMapID,
			direction,
			characterIDProp->value.string
		);
		character.speed = settings.charactersSpeed;

		const CharacterData *data = game_data_for_character_id(character.id);
		character.canNoticePlayer = data->lookAround;
		// ideally we want this to be driven by the character creation data,
		// but giving a bit of randomness here is good enough for this game.
		const f32 patrolInterval = (f32)GetRandomValue(1, (int)settings.charactersPatrolIntervalSecs);
		timer_start(&character.patrolTimer, patrolInterval);

		if (radiusProp != nil) {
			const i32 radius = atoi(radiusProp->value.string);
			if (radius > 0) {
				character.radius = (f32)radius;
			}
		}

		character_set_center_at(&character, (Vector2){.x = (f32)characterH->x, .y = (f32)characterH->y});
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

		const TileMap coastLineTileMap = assets.tileMaps.coastLine;

		const AnimatedTiledSprite sprite = {
			.entity = {
				.id = coastLineH->id,
				.position = {.x = (f32)coastLineH->x, .y = (f32)coastLineH->y},
				.ySort = (f32)coastLineH->y,
				.layer = WorldLayerWater,
			},
			.loop = true,
			.texture = coastLineTileMap.texture,
			.framesLen = 4,
			.currentFrame = 0,
			.frameTimer = 0.f,
			.animationSpeed = settings.coastLineAnimationSpeed,
			.sourceFrames = {
				tile_map_get_frame_at(coastLineTileMap, firstFrameCol, firstFrameRow),
				tile_map_get_frame_at(coastLineTileMap, firstFrameCol, firstFrameRow + 1 * 3),
				tile_map_get_frame_at(coastLineTileMap, firstFrameCol, firstFrameRow + 2 * 3),
				tile_map_get_frame_at(coastLineTileMap, firstFrameCol, firstFrameRow + 3 * 3),
			}
		};

		array_push(spritesList, sprite);
		coastLineH = coastLineH->next;
	}

	return spritesList;
}

// I hate the boolean, but i don't feel like changing the way the map is setup
// for this test project
static void init_terrain_sprites(Map *map, const tmx_layer *layer, bool isTopLayer) {
	if (layer == nil || layer->name == nil) {
		printf("no layer found\n");
		return;
	}

	for (u64 i = 0; i < map->tiledMap->height; i++) {
		for (u64 j = 0; j < map->tiledMap->width; j++) {
			const u32 gid = (layer->content.gids[(i * map->tiledMap->width) + j]) & TMX_FLIP_BITS_REMOVAL;
			if (map->tiledMap->tiles[gid] == NULL) {
				continue;
			}

			const tmx_tileset *tileSet = map->tiledMap->tiles[gid]->tileset;
			void *image = tileSet->image->resource_image;

			if (map->tiledMap->tiles[gid]->image) {
				image = map->tiledMap->tiles[gid]->image->resource_image;
			}

			// i32 flags = (layer->content.gids[(i * map->tiledMap->width) + j]) & ~TMX_FLIP_BITS_REMOVAL;
			const Rectangle sourceRec = {
				.x = (f32)map->tiledMap->tiles[gid]->ul_x,
				.y = (f32)map->tiledMap->tiles[gid]->ul_y,
				.width = (f32)tileSet->tile_width,
				.height = (f32)tileSet->tile_height,
			};
			const Vector2 destination = {
				.x = (f32)(j * tileSet->tile_width),
				.y = (f32)(i * tileSet->tile_height),
			};
			const Rectangle boundingBox = {
				.x = destination.x,
				.y = destination.y,
				.height = sourceRec.height,
				.width = sourceRec.width,
			};

			const Texture2D texture = *(Texture2D *)image;
			const StaticSprite s = {
				.entity = {
					.id = texture.id,
					.position = destination,
					.layer = isTopLayer ? WorldLayerMain : WorldLayerBackground,
					.ySort = destination.y + (boundingBox.height / 2),
				},
				.texture = texture,
				.width = boundingBox.width,
				.height = boundingBox.height,
				.sourceFrame = sourceRec,
			};

			array_push(map->backgroundSprites, s);
		}
	}
}

static void init_collision_sprites(Map *map, const tmx_layer *layer) {
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

		const Rectangle boundingBox = {
			.x = (f32)objectHead->x,
			.y = (f32)objectHead->y,
			.width = (f32)objectHead->width,
			.height = (f32)objectHead->height,
		};

		array_push(map->collisionBoxes, boundingBox);

		objectHead = objectHead->next;
	}
}

void init_transition_sprites(Map *map, const tmx_layer *layer) {
	const tmx_object *objectHead = layer->content.objgr->head;
	while (objectHead) {
		if (!objectHead->visible) {
			objectHead = objectHead->next;
			continue;
		}
		const tmx_property *targetProp = tmx_get_property(objectHead->properties, "target");
		panicIfNil(targetProp, "expected coast line object ot have 'target' property");

		const tmx_property *posProp = tmx_get_property(objectHead->properties, "pos");
		panicIfNil(posProp, "expected coast line object ot have 'pos' property");

		usize len = strnlen(targetProp->value.string, MAX_TRANSITION_DEST_LEN + 1);
		panicIf(len == 0 || len >= MAX_TRANSITION_DEST_LEN + 1, "found a bad string for 'target' property");
		len = strnlen(posProp->value.string, MAX_TRANSITION_DEST_LEN + 1);
		panicIf(len == 0 || len >= MAX_TRANSITION_DEST_LEN + 1, "found a bad string for 'pos' property");

		const Rectangle boundingBox = {
			.x = (f32)objectHead->x,
			.y = (f32)objectHead->y,
			.width = (f32)objectHead->width,
			.height = (f32)objectHead->height,
		};
		TransitionSprite sprite = {
			.box = boundingBox,
			.destination = "",
			.destinationPos = "",
		};
		strncpy(
			sprite.destination,
			targetProp->value.string,
			sizeof(sprite.destination) / sizeof(sprite.destination[0])
		);
		strncpy(
			sprite.destinationPos,
			posProp->value.string,
			sizeof(sprite.destinationPos) / sizeof(sprite.destinationPos[0])
		);

		array_push(map->transitionBoxes, sprite);
		objectHead = objectHead->next;
	}
}

static void init_object_sprites(Map *map, const tmx_layer *layer) {
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

		const i32 gid = objectHead->content.gid;
		const tmx_tile *tile = map->tiledMap->tiles[gid];
		panicIfNil(tile, "expected to find tileSet for object");
		panicIfNil(tile->image, "object does contain image");
		panicIfNil(tile->image->resource_image, "object does contain image data");

		const Rectangle sourceRec = {
			.x = (f32)tile->ul_x,
			.y = (f32)tile->ul_y,
			.width = (f32)objectHead->width,
			.height = (f32)objectHead->height,
		};
		const Vector2 destination = {
			.x = (f32)objectHead->x,
			.y = (f32)objectHead->y - sourceRec.height,
		};

		const Rectangle boundingBox = {
			.x = destination.x,
			.y = destination.y,
			.width = sourceRec.width,
			.height = sourceRec.height,
		};

		WorldLayer worldLayer = WorldLayerMain;
		if (objectHead->name != nil && streq(objectHead->name, "top")) {
			worldLayer = WorldLayerTop;
		}

		const Texture2D texture = *(Texture2D *)tile->image->resource_image;
		const StaticSprite s = {
			.entity = {
				.id = texture.id,
				.position = destination,
				.layer = worldLayer,
				.ySort = destination.y + (boundingBox.height / 2),
				.hitBox = rectangle_deflate(boundingBox, boundingBox.width * 0., boundingBox.height * 0.7),
				.collideable = true,
			},
			.texture = texture,
			.width = boundingBox.width,
			.height = boundingBox.height,
			.sourceFrame = sourceRec,
		};

		if (worldLayer == WorldLayerMain) {
			array_push(map->mainSprites, s);
		} else {
			array_push(map->foregroundSprites, s);
		}

		objectHead = objectHead->next;
	}
}

void map_update(const Map *map, const f32 dt) {
	for (i32 i = 0; i < array_length(map->waterSpritesList); i++) {
		animated_textures_sprite_update(&map->waterSpritesList[i], dt);
	}

	array_range(map->coastLineSpritesList, i) {
		animated_tiled_sprite_update(&map->coastLineSpritesList[i], dt);
	}

	array_range(map->overWorldCharacters, i) {
		character_update(&map->overWorldCharacters[i], dt);
	}
}

void map_draw(const Map *map) {
	ClearBackground(int_to_color(map->tiledMap->backgroundcolor));

	// background sprites
	array_range(map->backgroundSprites, i) {
		draw_static_sprite(map->backgroundSprites[i]);
	}

	draw_animated_textures_sprites(map->waterSpritesList);
	draw_animated_tiled_sprites(map->coastLineSpritesList);

	// todo(hector) - the player and the other characters need to be in the same list as the main sprites...
	array_range(map->overWorldCharacters, i) {
		character_draw(&map->overWorldCharacters[i]);
	}

	// main sprites
	bool playerDrawn = false;
	array_range(map->mainSprites, i) {
		const StaticSprite sprite = map->mainSprites[i];
		// todo(hector) - no me gusta, but its a quick hack that doesnt require refactoring the whole map system.
		if (!playerDrawn && sprite.entity.ySort > game.player.characterComponent.animatedSprite.entity.ySort) {
			// player_draw(&game.player);
			playerDrawn = true;
		}
		draw_static_sprite(sprite);
	}
	player_draw(&game.player);

	// foreground sprites
	array_range(map->foregroundSprites, i) {
		draw_static_sprite(map->foregroundSprites[i]);
	}

	if (game.isDebug) {
		// draw collision boxes
		array_range(map->collisionBoxes, i) {
			DrawRectangleLinesEx(map->collisionBoxes[i], 5.f, BLUE);
		}
		array_range(map->transitionBoxes, i) {
			DrawRectangleLinesEx(map->transitionBoxes[i].box, 5.f, LIME);
		}
	}
	array_range(map->transitionBoxes, i) {
		DrawRectangleLinesEx(map->transitionBoxes[i].box, 5.f, LIME);
	}
}

static void draw_tile(
	void *imageTexture2D,
	const Rectangle sourceRec,
	const Vector2 destination,
	const f32 opacity
) {
	const u8 c = (u8)opacity * 255;
	const Color tint = {c, c, c, c};
	DrawTextureRec(*(Texture2D *)imageTexture2D, sourceRec, destination, tint);
}

static bool collidesWithCamera(const Rectangle rect) {
	return CheckCollisionRecs(game.cameraBoundingBox, rect);
}

static void draw_static_sprite(const StaticSprite sprite) {
	const Rectangle spriteBoundingBox = {
		.x = sprite.entity.position.x,
		.y = sprite.entity.position.y,
		.width = sprite.sourceFrame.width,
		.height = sprite.sourceFrame.height,
	};
	if (!collidesWithCamera(spriteBoundingBox)) {
		return;
	}

	game.gameMetrics.drawnSprites++;
	DrawTextureRec(sprite.texture, sprite.sourceFrame, sprite.entity.position, WHITE);

	// draw debug frames
	if (!game.isDebug) { return; }

	DrawRectangleLinesEx(spriteBoundingBox, 3.f, RED);
	DrawCircleV(sprite.entity.position, 5.f, RED);
}

static void draw_animated_tiled_sprites(AnimatedTiledSprite *coastLineSprites) {
	if (array_length(coastLineSprites) == 0) { return; }

	array_range(coastLineSprites, i) {
		const AnimatedTiledSprite coastSprite = coastLineSprites[i];
		const Rectangle coastSpriteBoundingBox = {
			.x = coastSprite.entity.position.x,
			.y = coastSprite.entity.position.y,
			.width = coastSprite.sourceFrames[0].width,
			.height = coastSprite.sourceFrames[0].height,
		};

		if (!collidesWithCamera(coastSpriteBoundingBox)) {
			continue;
		}

		game.gameMetrics.drawnSprites++;
		const Rectangle tileToDraw = coastSprite.sourceFrames[coastSprite.currentFrame];
		draw_tile(&assets.tileMaps.coastLine.texture, tileToDraw, coastSprite.entity.position, 1.f);

		// draw debug frames
		if (!game.isDebug) { continue; }

		DrawRectangleLinesEx(coastSpriteBoundingBox, 3.f, RED);
		DrawCircleV(coastSprite.entity.position, 5.f, RED);
	}
}

static void draw_animated_textures_sprites(AnimatedTexturesSprite *waterSprites) {
	if (array_length(waterSprites) == 0) { return; }

	for (i32 i = 0; i < array_length(waterSprites); i++) {
		const AnimatedTexturesSprite waterSprite = waterSprites[i];
		const Rectangle sourceRec = {
			.x = waterSprite.entity.position.x,
			.y = waterSprite.entity.position.y,
			.height = (f32)waterSprite.textures[0].height,
			.width = (f32)waterSprite.textures[0].width,
		};
		if (!collidesWithCamera(sourceRec)) {
			continue;
		}

		game.gameMetrics.drawnSprites++;
		Texture2D *frameToDraw = &waterSprite.textures[waterSprite.currentFrame];
		draw_tile(frameToDraw, sourceRec, waterSprite.entity.position, 1.f);

		// draw debug frames
		if (!game.isDebug) { continue; }

		DrawRectangleLinesEx(sourceRec, 3.f, RED);
		DrawCircleV(waterSprite.entity.position, 5.f, RED);
	}
}

static void *texture_loader_callback(const char *path) {
	Texture2D *text = mallocate(sizeof(*text), MemoryTagTexture);
	*text = LoadTexture(path);
	slogi("loaded %s with ID %d", path, text->id);
	return text;
}

static void texture_free_callback(void *ptr) {
	const Texture2D *text = (Texture2D *)ptr;
	slogi("unloading texture #%d", text->id);
	UnloadTexture(*text);
	mfree(ptr, sizeof(*text), MemoryTagTexture);
}

static u64 totalMapMemoryAllocated = 0;

void *map_alloc_callback(void *ptr, const size_t len) {
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
