//
// Created by Hector Mejia on 5/30/24.
//

#ifndef RAYLIB_POKEMON_CLONE_ASSETS_H
#define RAYLIB_POKEMON_CLONE_ASSETS_H

#include "raylib.h"
#include "common.h"
#include "monsters.h"

typedef struct TileMap {
	i32 columns;
	i32 rows;
	Texture2D texture;
	Rectangle *framesList;
} TileMap;

typedef enum TileMapID {
	TileMapIDCoastLine = 0,
	TileMapIDPlayer,
	TileMapIDBlondCharacter,
	TileMapIDFireBossCharacter,
	TileMapIDGrassBossCharacter,
	TileMapIDHatGirlCharacter,
	TileMapIDPurpleGirlCharacter,
	TileMapIDStrawCharacter,
	TileMapIDWaterBossCharacter,
	TileMapIDYoungGirlCharacter,
	TileMapIDYoungGuyCharacter,

	TileMapIDMax,
} TileMapID;

typedef struct GameFont {
	Font font;
	f32 size;
} GameFont;

typedef struct Assets {
	struct {
		i32 len;
		Texture2D *texturesList;
	} waterTextures;

	TileMap tileMaps[TileMapIDMax];
	Texture2D grassTexture;
	Texture2D iceGrassTexture;
	Texture2D sandTexture;
	Texture2D characterShadowTexture;
	Texture2D exclamationMarkTexture;

	GameFont dialogFont;
	GameFont regularFont;
	GameFont smallFont;
	GameFont boldFont;

	struct {
		Texture2D atrox;
		Texture2D charmadillo;
		Texture2D cindrill;
		Texture2D cleaf;
		Texture2D draem;
		Texture2D finiette;
		Texture2D finsta;
		Texture2D friolera;
		Texture2D gulfin;
		Texture2D ivieron;
		Texture2D jacana;
		Texture2D larvea;
		Texture2D pluma;
		Texture2D plumette;
		Texture2D pouch;
		Texture2D sparchu;
	} monsterIcons;

	struct {
		Texture2D arrows;
		Texture2D cross;
		Texture2D hand;
		Texture2D notice;
		Texture2D shieldHighlight;
		Texture2D sword;
		Texture2D arrowsHighlight;
		Texture2D defense;
		Texture2D handHighlight;
		Texture2D recovery;
		Texture2D speed;
		Texture2D swordHighlight;
		Texture2D attack;
		Texture2D energy;
		Texture2D health;
		Texture2D shield;
		Texture2D star;
	} uiIcons;

	struct {
		Texture2D forrest;
		Texture2D ice;
		Texture2D sand;
	} battleBackgrounds;

	TileMap monsterTileMaps[MonsterIDCount];
} Assets;

extern Assets assets;

void load_assets();
void unload_assets();

Rectangle tile_map_get_frame_at(TileMap tm, i32 col, i32 row);

#endif //RAYLIB_POKEMON_CLONE_ASSETS_H
