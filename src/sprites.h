//
// Created by Hector Mejia on 6/1/24.
//

#ifndef RAYLIB_POKEMON_CLONE_SPRITES_H
#define RAYLIB_POKEMON_CLONE_SPRITES_H

#include "raylib.h"
#include "common.h"

typedef struct AnimatedSprite {
    Texture2D *textures;
    u32 id;
    i32 framesLen;
    i32 currentFrame;
    f32 frameTimer;
    f32 animationSpeed;
    Vector2 position;
} AnimatedSprite;

void animate_sprite(AnimatedSprite *sprite, f32 dt);


#endif //RAYLIB_POKEMON_CLONE_SPRITES_H
