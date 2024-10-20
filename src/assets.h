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

typedef struct GameFont {
	Font rFont;
	f32 size;
} GameFont;

typedef struct Assets {
	struct {
		i32 len;
		Texture2D *texturesList;
	} waterTextures;

	Texture2D grassTexture;
	Texture2D iceGrassTexture;
	Texture2D sandTexture;
	Texture2D characterShadowTexture;
	Texture2D exclamationMarkTexture;

	struct {
		GameFont dialog;
		GameFont regular;
		GameFont small;
		GameFont bold;
	} fonts;

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
		Texture2D explosion;
		Texture2D fire;
		Texture2D green;
		Texture2D ice;
		Texture2D scratch;
		Texture2D splash;
	} attackTextures;

	struct {
		Texture2D forrest;
		Texture2D ice;
		Texture2D sand;
	} battleBackgrounds;

	struct {
		TileMap coastLine;
		TileMap player;
		TileMap blondCharacter;
		TileMap fireBossCharacter;
		TileMap grassBossCharacter;
		TileMap hatGirlCharacter;
		TileMap purpleGirlCharacter;
		TileMap strawCharacter;
		TileMap waterBossCharacter;
		TileMap youngGirlCharacter;
		TileMap youngGuyCharacter;
	} tileMaps;

	TileMap monsterTileMaps[MonsterIDCount];
	TileMap monsterAttackTileMaps[MonsterAbilityAnimationIDCount];
	struct {
		Shader textureOutline;
		Shader grayscale;
	} shaders;

	struct {
		Sound explosion;
		Sound evolution;
		Sound fire;
		Sound green;
		Sound ice;
		Sound notice;
		Sound scratch;
		Sound splash;
	} sounds;

	struct {
		Music battle;
		Music overWorld;
	} music;
} Assets;

extern Assets assets;

void load_assets();
void unload_assets();
void load_shaders();
void unload_shaders();

Rectangle tile_map_get_frame_at(TileMap tm, i32 col, i32 row);

#endif //RAYLIB_POKEMON_CLONE_ASSETS_H
