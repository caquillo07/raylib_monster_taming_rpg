//
// Created by Hector Mejia on 6/1/24.
//

#ifndef RAYLIB_POKEMON_CLONE_SPRITES_H
#define RAYLIB_POKEMON_CLONE_SPRITES_H

#include "raylib.h"
#include "common.h"

typedef enum {
    WorldLayerWater = 0,
    WorldLayerBackground,
    WorldLayerShadow,
    WorldLayerMain,
    WorldLayerTop,

    WorldLayerTotal,
} WorldLayer;

typedef struct Sprite {
    u32 id;
    Texture2D texture;
    Vector2 position;
    f32 width;
    f32 height;
    Rectangle sourceFrame;
    WorldLayer layer;
    f32 ySort;
} Sprite;

typedef struct AnimatedTiledSprite {
    u32 id;
    Texture2D texture;
    i32 framesLen;
    i32 currentFrame;
    f32 frameTimer;
    f32 animationSpeed;
    Vector2 position;
    Rectangle *sourceFrames;
    WorldLayer layer;
    f32 ySort;
} AnimatedTiledSprite;

typedef struct AnimatedTexturesSprite {
    u32 id;
    Texture2D *textures;
    i32 framesLen;
    i32 currentFrame;
    f32 frameTimer;
    f32 animationSpeed;
    Vector2 position;
    WorldLayer layer;
    f32 ySort;
} AnimatedTexturesSprite;

void update_animated_textures_sprite(AnimatedTexturesSprite *sprite, f32 dt);

void update_animated_tiled_sprite(AnimatedTiledSprite *sprite, f32 dt);

#endif //RAYLIB_POKEMON_CLONE_SPRITES_H
