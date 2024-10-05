//
// Created by Hector Mejia on 6/1/24.
//

#include <math.h>
#include "sprites.h"

void animated_textures_sprite_update(AnimatedTexturesSprite *sprite, f32 dt) {
	sprite->frameTimer += sprite->animationSpeed * dt;
	sprite->currentFrame = (i32)fmodf(sprite->frameTimer, (f32)sprite->framesLen);
}

void animated_tiled_sprite_update(AnimatedTiledSprite *sprite, f32 dt) {
	sprite->frameTimer += sprite->animationSpeed * dt;
	if (sprite->loop) {
		sprite->currentFrame = (i32)fmodf(sprite->frameTimer, (f32)sprite->framesLen);
	} else if (sprite->done) {
		sprite->currentFrame = sprite->framesLen-1;
	} else if (sprite->currentFrame < sprite->framesLen) {
		sprite->currentFrame = (i32)sprite->frameTimer;
		sprite->done = sprite->currentFrame >= sprite->framesLen;
	}
}

Rectangle animated_tiled_sprite_current_frame(const AnimatedTiledSprite *sprite) {
	return sprite->sourceFrames[sprite->currentFrame];
}
