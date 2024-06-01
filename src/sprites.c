//
// Created by Hector Mejia on 6/1/24.
//

#include <math.h>
#include "sprites.h"

void animate_sprite(AnimatedSprite *sprite, f32 dt) {
    sprite->frameTimer += sprite->animationSpeed * dt;
    sprite->currentFrame = (i32)fmodf(sprite->frameTimer, (f32)sprite->framesLen) ;
}
