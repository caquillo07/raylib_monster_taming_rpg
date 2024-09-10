//
// Created by Hector Mejia on 5/30/24.
//

#include <dirent.h>
#include "assets.h"

#include <assert.h>

#include "common.h"
#include "array/array.h"

static Texture2D *import_textures_from_directory(const char *dir);
static int dir_entry_compare(const void *lhsp, const void *rhsp);
static TileMap load_tile_map(i32 cols, i32 rows, const char *imagePath);

Assets assets;

void load_assets() {
	assets = (Assets){};
	assets.waterTextures.texturesList = import_textures_from_directory("./graphics/tilesets/water");
	assets.waterTextures.len = 4;

	// each graphic is 192, but we want 64px chunks
	assets.tileMaps[TileMapIDCoastLine] = load_tile_map(3 * 8, 3 * 4, ("./graphics/tilesets/coast.png"));

	// player and characters
	assets.tileMaps[TileMapIDPlayer] = load_tile_map(4, 4, ("./graphics/characters/player.png"));
	assets.tileMaps[TileMapIDBlondCharacter] = load_tile_map(4, 4, ("./graphics/characters/blond.png"));
	assets.tileMaps[TileMapIDFireBossCharacter] = load_tile_map(4, 4, ("./graphics/characters/fire_boss.png"));
	assets.tileMaps[TileMapIDGrassBossCharacter] = load_tile_map(4, 4, ("./graphics/characters/grass_boss.png"));
	assets.tileMaps[TileMapIDHatGirlCharacter] = load_tile_map(4, 4, ("./graphics/characters/hat_girl.png"));
	assets.tileMaps[TileMapIDPurpleGirlCharacter] = load_tile_map(4, 4, ("./graphics/characters/purple_girl.png"));
	assets.tileMaps[TileMapIDStrawCharacter] = load_tile_map(4, 4, ("./graphics/characters/straw.png"));
	assets.tileMaps[TileMapIDWaterBossCharacter] = load_tile_map(4, 4, ("./graphics/characters/water_boss.png"));
	assets.tileMaps[TileMapIDYoungGirlCharacter] = load_tile_map(4, 4, ("./graphics/characters/young_girl.png"));
	assets.tileMaps[TileMapIDYoungGuyCharacter] = load_tile_map(4, 4, ("./graphics/characters/young_guy.png"));

	// todo(hector) - refactor to use lookup table instead
	assets.monsterIcons.atrox = LoadTexture("./graphics/icons/Atrox.png");
	assets.monsterIcons.charmadillo = LoadTexture("./graphics/icons/Charmadillo.png");
	assets.monsterIcons.cindrill = LoadTexture("./graphics/icons/Cindrill.png");
	assets.monsterIcons.cleaf = LoadTexture("./graphics/icons/Cleaf.png");
	assets.monsterIcons.draem = LoadTexture("./graphics/icons/Draem.png");
	assets.monsterIcons.finiette = LoadTexture("./graphics/icons/Finiette.png");
	assets.monsterIcons.finsta = LoadTexture("./graphics/icons/Finsta.png");
	assets.monsterIcons.friolera = LoadTexture("./graphics/icons/Friolera.png");
	assets.monsterIcons.gulfin = LoadTexture("./graphics/icons/Gulfin.png");
	assets.monsterIcons.ivieron = LoadTexture("./graphics/icons/Ivieron.png");
	assets.monsterIcons.jacana = LoadTexture("./graphics/icons/Jacana.png");
	assets.monsterIcons.larvea = LoadTexture("./graphics/icons/Larvea.png");
	assets.monsterIcons.pluma = LoadTexture("./graphics/icons/Pluma.png");
	assets.monsterIcons.plumette = LoadTexture("./graphics/icons/Plumette.png");
	assets.monsterIcons.pouch = LoadTexture("./graphics/icons/Pouch.png");
	assets.monsterIcons.sparchu = LoadTexture("./graphics/icons/Sparchu.png");

	assets.monsterTileMaps[MonsterIDAtrox] = load_tile_map(4, 2, "./graphics/monsters/Atrox.png");
	assets.monsterTileMaps[MonsterIDCharmadillo] = load_tile_map(4, 2, "./graphics/monsters/Charmadillo.png");
	assets.monsterTileMaps[MonsterIDCindrill] = load_tile_map(4, 2, "./graphics/monsters/Cindrill.png");
	assets.monsterTileMaps[MonsterIDCleaf] = load_tile_map(4, 2, "./graphics/monsters/Cleaf.png");
	assets.monsterTileMaps[MonsterIDDraem] = load_tile_map(4, 2, "./graphics/monsters/Draem.png");
	assets.monsterTileMaps[MonsterIDFiniette] = load_tile_map(4, 2, "./graphics/monsters/Finiette.png");
	assets.monsterTileMaps[MonsterIDFinsta] = load_tile_map(4, 2, "./graphics/monsters/Finsta.png");
	assets.monsterTileMaps[MonsterIDFriolera] = load_tile_map(4, 2, "./graphics/monsters/Friolera.png");
	assets.monsterTileMaps[MonsterIDGulfin] = load_tile_map(4, 2, "./graphics/monsters/Gulfin.png");
	assets.monsterTileMaps[MonsterIDIvieron] = load_tile_map(4, 2, "./graphics/monsters/Ivieron.png");
	assets.monsterTileMaps[MonsterIDJacana] = load_tile_map(4, 2, "./graphics/monsters/Jacana.png");
	assets.monsterTileMaps[MonsterIDLarvea] = load_tile_map(4, 2, "./graphics/monsters/Larvea.png");
	assets.monsterTileMaps[MonsterIDPluma] = load_tile_map(4, 2, "./graphics/monsters/Pluma.png");
	assets.monsterTileMaps[MonsterIDPlumette] = load_tile_map(4, 2, "./graphics/monsters/Plumette.png");
	assets.monsterTileMaps[MonsterIDPouch] = load_tile_map(4, 2, "./graphics/monsters/Pouch.png");
	assets.monsterTileMaps[MonsterIDSparchu] = load_tile_map(4, 2, "./graphics/monsters/Sparchu.png");

	assets.grassTexture = LoadTexture("./graphics/objects/grass.png");
	assets.iceGrassTexture = LoadTexture("./graphics/objects/grass_ice.png");
	assets.sandTexture = LoadTexture("./graphics/objects/sand.png");

	assets.characterShadowTexture = LoadTexture("./graphics/other/shadow.png");
	assets.exclamationMarkTexture = LoadTexture("./graphics/ui/notice.png");

	assets.dialogFont = LoadFontEx("./graphics/fonts/PixeloidSans.ttf", 30, nil, 250);
	assets.regularFont = LoadFontEx("./graphics/fonts/PixeloidSans.ttf", 18, nil, 250);
	assets.smallFont = LoadFontEx("./graphics/fonts/PixeloidSans.ttf", 14, nil, 250);
	assets.boldFont = LoadFontEx("./graphics/fonts/dogicapixelbold.otf", 20, nil, 250);
}

void unload_assets() {
	array_range(assets.waterTextures.texturesList, i) {
		UnloadTexture(assets.waterTextures.texturesList[i]);
	}
	array_free(assets.waterTextures.texturesList);

	UnloadTexture(assets.tileMaps[TileMapIDCoastLine].texture);
	array_free(assets.tileMaps[TileMapIDCoastLine].framesList);

	UnloadTexture(assets.tileMaps[TileMapIDPlayer].texture);
	array_free(assets.tileMaps[TileMapIDPlayer].framesList);

	UnloadTexture(assets.tileMaps[TileMapIDBlondCharacter].texture);
	array_free(assets.tileMaps[TileMapIDBlondCharacter].framesList);
	UnloadTexture(assets.tileMaps[TileMapIDFireBossCharacter].texture);
	array_free(assets.tileMaps[TileMapIDFireBossCharacter].framesList);
	UnloadTexture(assets.tileMaps[TileMapIDGrassBossCharacter].texture);
	array_free(assets.tileMaps[TileMapIDGrassBossCharacter].framesList);
	UnloadTexture(assets.tileMaps[TileMapIDHatGirlCharacter].texture);
	array_free(assets.tileMaps[TileMapIDHatGirlCharacter].framesList);
	UnloadTexture(assets.tileMaps[TileMapIDPurpleGirlCharacter].texture);
	array_free(assets.tileMaps[TileMapIDPurpleGirlCharacter].framesList);
	UnloadTexture(assets.tileMaps[TileMapIDStrawCharacter].texture);
	array_free(assets.tileMaps[TileMapIDStrawCharacter].framesList);
	UnloadTexture(assets.tileMaps[TileMapIDWaterBossCharacter].texture);
	array_free(assets.tileMaps[TileMapIDWaterBossCharacter].framesList);
	UnloadTexture(assets.tileMaps[TileMapIDYoungGirlCharacter].texture);
	array_free(assets.tileMaps[TileMapIDYoungGirlCharacter].framesList);
	UnloadTexture(assets.tileMaps[TileMapIDYoungGuyCharacter].texture);
	array_free(assets.tileMaps[TileMapIDYoungGuyCharacter].framesList);

	UnloadTexture(assets.grassTexture);
	UnloadTexture(assets.iceGrassTexture);
	UnloadTexture(assets.sandTexture);

	UnloadTexture(assets.characterShadowTexture);
	UnloadTexture(assets.exclamationMarkTexture);

	UnloadTexture(assets.monsterIcons.atrox);
	UnloadTexture(assets.monsterIcons.charmadillo);
	UnloadTexture(assets.monsterIcons.cindrill);
	UnloadTexture(assets.monsterIcons.cleaf);
	UnloadTexture(assets.monsterIcons.draem);
	UnloadTexture(assets.monsterIcons.finiette);
	UnloadTexture(assets.monsterIcons.finsta);
	UnloadTexture(assets.monsterIcons.friolera);
	UnloadTexture(assets.monsterIcons.gulfin);
	UnloadTexture(assets.monsterIcons.ivieron);
	UnloadTexture(assets.monsterIcons.jacana);
	UnloadTexture(assets.monsterIcons.larvea);
	UnloadTexture(assets.monsterIcons.pluma);
	UnloadTexture(assets.monsterIcons.plumette);
	UnloadTexture(assets.monsterIcons.pouch);
	UnloadTexture(assets.monsterIcons.sparchu);

	UnloadTexture(assets.monsterTileMaps[MonsterIDAtrox].texture);
	array_free(assets.monsterTileMaps[MonsterIDAtrox].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDCharmadillo].texture);
	array_free(assets.monsterTileMaps[MonsterIDCharmadillo].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDCindrill].texture);
	array_free(assets.monsterTileMaps[MonsterIDCindrill].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDCleaf].texture);
	array_free(assets.monsterTileMaps[MonsterIDCleaf].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDDraem].texture);
	array_free(assets.monsterTileMaps[MonsterIDDraem].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDFiniette].texture);
	array_free(assets.monsterTileMaps[MonsterIDFiniette].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDFinsta].texture);
	array_free(assets.monsterTileMaps[MonsterIDFinsta].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDFriolera].texture);
	array_free(assets.monsterTileMaps[MonsterIDFriolera].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDGulfin].texture);
	array_free(assets.monsterTileMaps[MonsterIDGulfin].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDIvieron].texture);
	array_free(assets.monsterTileMaps[MonsterIDIvieron].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDJacana].texture);
	array_free(assets.monsterTileMaps[MonsterIDJacana].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDLarvea].texture);
	array_free(assets.monsterTileMaps[MonsterIDLarvea].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDPluma].texture);
	array_free(assets.monsterTileMaps[MonsterIDPluma].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDPlumette].texture);
	array_free(assets.monsterTileMaps[MonsterIDPlumette].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDPouch].texture);
	array_free(assets.monsterTileMaps[MonsterIDPouch].framesList);
	UnloadTexture(assets.monsterTileMaps[MonsterIDSparchu].texture);
	array_free(assets.monsterTileMaps[MonsterIDSparchu].framesList);

	UnloadFont(assets.dialogFont);
	UnloadFont(assets.regularFont);
	UnloadFont(assets.smallFont);
	UnloadFont(assets.boldFont);
}

static Texture2D *import_textures_from_directory(const char *dirPath) {
	DIR *dir = opendir(dirPath);
	panicIfNil(dir, "failed to open directory %s for texture", dirPath);

	struct dirent *fileInfo;
	struct dirent *dynFileInfoList = nil;
	while ((fileInfo = readdir(dir)) != nil) {
		if (strncmp(".", fileInfo->d_name, 1) == 0 || strncmp("..", fileInfo->d_name, 2) == 0) {
			continue;
		}
		array_push(dynFileInfoList, *fileInfo);
	}
	closedir(dir);
	panicIfNil(dynFileInfoList, "directory for import was empty");

	// sort the directory entries
	qsort(dynFileInfoList, array_length(dynFileInfoList), sizeof(*dynFileInfoList), dir_entry_compare);

	Texture2D *dynTextures = nil;
	for (int i = 0; i < array_length(dynFileInfoList); i++) {
#define textFilePathBufSize 1024
		char buff[textFilePathBufSize];
		snprintf(buff, textFilePathBufSize, "%s/%s", dirPath, dynFileInfoList[i].d_name);
		const Texture2D text = LoadTexture(buff);
		array_push(dynTextures, text);
	}

	array_free(dynFileInfoList);
	return dynTextures;
}

TileMap load_tile_map(const i32 cols, const i32 rows, const char *imagePath) {
	const Texture2D texture = LoadTexture(imagePath);
	panicIf(!IsTextureReady(texture), "failed to load texture");

	Rectangle *framesList = nil;
	const i32 cellWidth = texture.width / cols;
	const i32 cellHeight = texture.height / rows;
	for (i32 row = 0; row < rows; row++) {
		for (i32 col = 0; col < cols; col++) {
			const Rectangle frame = {
				.x = (f32)(col * cellWidth),
				.y = (f32)(row * cellHeight),
				.width = (f32)cellWidth,
				.height = (f32)cellHeight,
			};
			array_push(framesList, frame);
		}
	}
	const TileMap tileMap = {
		.framesList = framesList,
		.rows = rows,
		.columns = cols,
		.texture = texture,
	};
	return tileMap;
}

// returns a copy of the frame inside the tilemap
Rectangle tile_map_get_frame_at(const TileMap tm, const i32 col, const i32 row) {
	const i32 index = (tm.columns * row) + col;
	panicIf((index > array_length(tm.framesList) - 1), "queried x,y is out of bounds");
	return tm.framesList[index];
}

static int dir_entry_compare(const void *lhsp, const void *rhsp) {
	const struct dirent *lhs = lhsp;
	const struct dirent *rhs = rhsp;
	return strcmp(lhs->d_name, rhs->d_name);
}
