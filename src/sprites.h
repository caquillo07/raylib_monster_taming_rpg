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

typedef struct Entity {
    u32 id;
    f32 ySort;
    Vector2 position;
    WorldLayer layer;
} Entity;

typedef struct StaticSprite {
    Entity entity; // cannot be moved from 1st position

    Texture2D texture;
    f32 width;
    f32 height;
    Rectangle sourceFrame;
} StaticSprite;

typedef struct AnimatedTiledSprite {
    Entity entity; // cannot be moved from 1st position

    Texture2D texture;
    i32 framesLen;
    i32 currentFrame;
    f32 frameTimer;
    f32 animationSpeed;
    Rectangle *sourceFrames;
} AnimatedTiledSprite;

typedef struct AnimatedTexturesSprite {
    Entity entity; // cannot be moved from 1st position

    Texture2D *textures;
    i32 framesLen;
    i32 currentFrame;
    f32 frameTimer;
    f32 animationSpeed;
} AnimatedTexturesSprite;

void update_animated_textures_sprite(AnimatedTexturesSprite *sprite, f32 dt);

void update_animated_tiled_sprite(AnimatedTiledSprite *sprite, f32 dt);

#endif //RAYLIB_POKEMON_CLONE_SPRITES_H
