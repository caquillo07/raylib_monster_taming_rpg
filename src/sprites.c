//
// Created by Hector Mejia on 6/1/24.
//

#include <math.h>
#include "sprites.h"

void update_animated_textures_sprite(AnimatedTexturesSprite *sprite, f32 dt) {
    sprite->frameTimer += sprite->animationSpeed * dt;
    sprite->currentFrame = (i32) fmodf(sprite->frameTimer, (f32) sprite->framesLen);
}

void update_animated_tiled_sprite(AnimatedTiledSprite *sprite, f32 dt) {
    sprite->frameTimer += sprite->animationSpeed * dt;
    sprite->currentFrame = (i32) fmodf(sprite->frameTimer, (f32) sprite->framesLen);
}
