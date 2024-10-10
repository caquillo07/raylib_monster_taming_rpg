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
static void unload_tile_map(TileMap *tm);

Assets assets;

void load_assets() {
	assets = (Assets){};
	assets.waterTextures.texturesList = import_textures_from_directory("./graphics/tilesets/water");
	assets.waterTextures.len = 4;

	// each graphic is 192, but we want 64px chunks
	assets.tileMaps.coastLine = load_tile_map(3 * 8, 3 * 4, ("./graphics/tilesets/coast.png"));

	// player and characters
	assets.tileMaps.player = load_tile_map(4, 4, ("./graphics/characters/player.png"));
	assets.tileMaps.blondCharacter = load_tile_map(4, 4, ("./graphics/characters/blond.png"));
	assets.tileMaps.fireBossCharacter = load_tile_map(4, 4, ("./graphics/characters/fire_boss.png"));
	assets.tileMaps.grassBossCharacter = load_tile_map(4, 4, ("./graphics/characters/grass_boss.png"));
	assets.tileMaps.hatGirlCharacter = load_tile_map(4, 4, ("./graphics/characters/hat_girl.png"));
	assets.tileMaps.purpleGirlCharacter = load_tile_map(4, 4, ("./graphics/characters/purple_girl.png"));
	assets.tileMaps.strawCharacter = load_tile_map(4, 4, ("./graphics/characters/straw.png"));
	assets.tileMaps.waterBossCharacter = load_tile_map(4, 4, ("./graphics/characters/water_boss.png"));
	assets.tileMaps.youngGirlCharacter = load_tile_map(4, 4, ("./graphics/characters/young_girl.png"));
	assets.tileMaps.youngGuyCharacter = load_tile_map(4, 4, ("./graphics/characters/young_guy.png"));

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


	assets.uiIcons.arrows = LoadTexture("./graphics/ui/arrows.png");
	assets.uiIcons.cross = LoadTexture("./graphics/ui/cross.png");
	assets.uiIcons.hand = LoadTexture("./graphics/ui/hand.png");
	assets.uiIcons.notice = LoadTexture("./graphics/ui/notice.png");
	assets.uiIcons.shieldHighlight = LoadTexture("./graphics/ui/shield_highlight.png");
	assets.uiIcons.sword = LoadTexture("./graphics/ui/sword.png");
	assets.uiIcons.arrowsHighlight = LoadTexture("./graphics/ui/arrows_highlight.png");
	assets.uiIcons.defense = LoadTexture("./graphics/ui/defense.png");
	assets.uiIcons.handHighlight = LoadTexture("./graphics/ui/hand_highlight.png");
	assets.uiIcons.recovery = LoadTexture("./graphics/ui/recovery.png");
	assets.uiIcons.speed = LoadTexture("./graphics/ui/speed.png");
	assets.uiIcons.swordHighlight = LoadTexture("./graphics/ui/sword_highlight.png");
	assets.uiIcons.attack = LoadTexture("./graphics/ui/attack.png");
	assets.uiIcons.energy = LoadTexture("./graphics/ui/energy.png");
	assets.uiIcons.health = LoadTexture("./graphics/ui/health.png");
	assets.uiIcons.shield = LoadTexture("./graphics/ui/shield.png");
	assets.uiIcons.star = LoadTexture("./graphics/ui/star.png");

	assets.attackTextures.explosion = LoadTexture("./graphics/attacks/explosion.png");
	assets.attackTextures.fire = LoadTexture("./graphics/attacks/fire.png");
	assets.attackTextures.green = LoadTexture("./graphics/attacks/green.png");
	assets.attackTextures.ice = LoadTexture("./graphics/attacks/ice.png");
	assets.attackTextures.scratch = LoadTexture("./graphics/attacks/scratch.png");
	assets.attackTextures.splash = LoadTexture("./graphics/attacks/splash.png");

	assets.battleBackgrounds.forrest = LoadTexture("./graphics/backgrounds/forest.png");
	assets.battleBackgrounds.ice = LoadTexture("./graphics/backgrounds/ice.png");
	assets.battleBackgrounds.sand = LoadTexture("./graphics/backgrounds/sand.png");

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

	assets.monsterAttackTileMaps[MonsterAbilityAnimationIDExplosion] = load_tile_map(4, 1, "./graphics/attacks/explosion.png");
	assets.monsterAttackTileMaps[MonsterAbilityAnimationIDFire] = load_tile_map(4, 1, "./graphics/attacks/fire.png");
	assets.monsterAttackTileMaps[MonsterAbilityAnimationIDGreen] = load_tile_map(4, 1, "./graphics/attacks/green.png");
	assets.monsterAttackTileMaps[MonsterAbilityAnimationIDIce] = load_tile_map(4, 1, "./graphics/attacks/ice.png");
	assets.monsterAttackTileMaps[MonsterAbilityAnimationIDScratch] = load_tile_map(4, 1, "./graphics/attacks/scratch.png");
	assets.monsterAttackTileMaps[MonsterAbilityAnimationIDSplash] = load_tile_map(4, 1, "./graphics/attacks/splash.png");

	assets.grassTexture = LoadTexture("./graphics/objects/grass.png");
	assets.iceGrassTexture = LoadTexture("./graphics/objects/grass_ice.png");
	assets.sandTexture = LoadTexture("./graphics/objects/sand.png");

	assets.characterShadowTexture = LoadTexture("./graphics/other/shadow.png");
	assets.exclamationMarkTexture = LoadTexture("./graphics/ui/notice.png");

	assets.fonts.dialog.size = 30;
	assets.fonts.dialog.rFont = LoadFontEx(
		"./graphics/fonts/PixeloidSans.ttf",
		(i32)assets.fonts.dialog.size,
		nil,
		250
	);
	assets.fonts.regular.size = 18;
	assets.fonts.regular.rFont = LoadFontEx(
		"./graphics/fonts/PixeloidSans.ttf",
		(i32)assets.fonts.regular.size,
		nil,
		250
	);
	assets.fonts.small.size = 14;
	assets.fonts.small.rFont = LoadFontEx("./graphics/fonts/PixeloidSans.ttf", (i32)assets.fonts.small.size, nil, 250);
	assets.fonts.bold.size = 20;
	assets.fonts.bold.rFont = LoadFontEx("./graphics/fonts/dogicapixelbold.otf", (i32)assets.fonts.bold.size, nil, 250);
}

void unload_assets() {
	array_range(assets.waterTextures.texturesList, i) {
		UnloadTexture(assets.waterTextures.texturesList[i]);
	}
	array_free(assets.waterTextures.texturesList);

	UnloadTexture(assets.tileMaps.coastLine.texture);
	array_free(assets.tileMaps.coastLine.framesList);

	UnloadTexture(assets.tileMaps.player.texture);
	array_free(assets.tileMaps.player.framesList);

	UnloadTexture(assets.tileMaps.blondCharacter.texture);
	array_free(assets.tileMaps.blondCharacter.framesList);
	UnloadTexture(assets.tileMaps.fireBossCharacter.texture);
	array_free(assets.tileMaps.fireBossCharacter.framesList);
	UnloadTexture(assets.tileMaps.grassBossCharacter.texture);
	array_free(assets.tileMaps.grassBossCharacter.framesList);
	UnloadTexture(assets.tileMaps.hatGirlCharacter.texture);
	array_free(assets.tileMaps.hatGirlCharacter.framesList);
	UnloadTexture(assets.tileMaps.purpleGirlCharacter.texture);
	array_free(assets.tileMaps.purpleGirlCharacter.framesList);
	UnloadTexture(assets.tileMaps.strawCharacter.texture);
	array_free(assets.tileMaps.strawCharacter.framesList);
	UnloadTexture(assets.tileMaps.waterBossCharacter.texture);
	array_free(assets.tileMaps.waterBossCharacter.framesList);
	UnloadTexture(assets.tileMaps.youngGirlCharacter.texture);
	array_free(assets.tileMaps.youngGirlCharacter.framesList);
	UnloadTexture(assets.tileMaps.youngGuyCharacter.texture);
	array_free(assets.tileMaps.youngGuyCharacter.framesList);

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

	UnloadTexture(assets.uiIcons.arrows);
	UnloadTexture(assets.uiIcons.cross);
	UnloadTexture(assets.uiIcons.hand);
	UnloadTexture(assets.uiIcons.notice);
	UnloadTexture(assets.uiIcons.shieldHighlight);
	UnloadTexture(assets.uiIcons.sword);
	UnloadTexture(assets.uiIcons.arrowsHighlight);
	UnloadTexture(assets.uiIcons.defense);
	UnloadTexture(assets.uiIcons.handHighlight);
	UnloadTexture(assets.uiIcons.recovery);
	UnloadTexture(assets.uiIcons.speed);
	UnloadTexture(assets.uiIcons.swordHighlight);
	UnloadTexture(assets.uiIcons.attack);
	UnloadTexture(assets.uiIcons.energy);
	UnloadTexture(assets.uiIcons.health);
	UnloadTexture(assets.uiIcons.shield);
	UnloadTexture(assets.uiIcons.star);

	UnloadTexture(assets.attackTextures.explosion);
	UnloadTexture(assets.attackTextures.fire);
	UnloadTexture(assets.attackTextures.green);
	UnloadTexture(assets.attackTextures.ice);
	UnloadTexture(assets.attackTextures.scratch);
	UnloadTexture(assets.attackTextures.splash);

	UnloadTexture(assets.battleBackgrounds.forrest);
	UnloadTexture(assets.battleBackgrounds.ice);
	UnloadTexture(assets.battleBackgrounds.sand);

	unload_tile_map(&assets.monsterTileMaps[MonsterIDAtrox]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDCharmadillo]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDCindrill]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDCleaf]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDDraem]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDFiniette]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDFinsta]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDFriolera]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDGulfin]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDIvieron]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDJacana]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDLarvea]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDPluma]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDPlumette]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDPouch]);
	unload_tile_map(&assets.monsterTileMaps[MonsterIDSparchu]);

	unload_tile_map(&assets.monsterAttackTileMaps[MonsterAbilityAnimationIDExplosion]);
	unload_tile_map(&assets.monsterAttackTileMaps[MonsterAbilityAnimationIDFire]);
	unload_tile_map(&assets.monsterAttackTileMaps[MonsterAbilityAnimationIDGreen]);
	unload_tile_map(&assets.monsterAttackTileMaps[MonsterAbilityAnimationIDIce]);
	unload_tile_map(&assets.monsterAttackTileMaps[MonsterAbilityAnimationIDScratch]);
	unload_tile_map(&assets.monsterAttackTileMaps[MonsterAbilityAnimationIDSplash]);

	UnloadFont(assets.fonts.dialog.rFont);
	UnloadFont(assets.fonts.regular.rFont);
	UnloadFont(assets.fonts.small.rFont);
	UnloadFont(assets.fonts.bold.rFont);
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

void unload_tile_map(TileMap *tm) {
	UnloadTexture(tm->texture);
	array_free(tm->framesList);
}

// returns a copy of the frame inside the tilemap
Rectangle tile_map_get_frame_at(const TileMap tm, const i32 col, const i32 row) {
	const i32 index = (tm.columns * row) + col;
	panicIf((index > array_length(tm.framesList) - 1), "queried x,y is out of bounds");
	return tm.framesList[index];
}

void load_shaders() {
	assets.shaders.textureOutline = LoadShader(
		nil,
		"./shaders/texture_outline.frag"
	);
	assets.shaders.grayscale = LoadShader(
		nil,
		"./shaders/texture_grayscale.frag"
	);
}

void unload_shaders() {
	UnloadShader(assets.shaders.textureOutline);
}

static int dir_entry_compare(const void *lhsp, const void *rhsp) {
	const struct dirent *lhs = lhsp;
	const struct dirent *rhs = rhsp;
	return strcmp(lhs->d_name, rhs->d_name);
}
