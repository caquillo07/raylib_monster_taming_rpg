//
// Created by Hector Mejia on 6/1/24.
//

#ifndef RAYLIB_POKEMON_CLONE_SPRITES_H
#define RAYLIB_POKEMON_CLONE_SPRITES_H

#include <stddef.h>
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
	Rectangle hitBox;
	bool collideable;
} Entity;

typedef struct StaticSprite {
	Entity entity; // cannot be moved from 1st position

	Texture2D texture;
	f32 width;
	f32 height;
	Rectangle sourceFrame;
} StaticSprite;

static_assert(offsetof(StaticSprite, entity) == 0, "entity must be the first member of StaticSprite");


#define AnimatedSpriteAnimationFramesLen 4
typedef struct AnimatedTiledSprite {
	Entity entity; // cannot be moved from 1st position

	bool loop;
	bool done;
	Texture2D texture;
	i32 framesLen;
	i32 currentFrame;
	f32 frameTimer;
	f32 animationSpeed;
	Rectangle sourceFrames[AnimatedSpriteAnimationFramesLen];
} AnimatedTiledSprite;

static_assert(offsetof(AnimatedTiledSprite, entity) == 0, "entity must be the first member of AnimatedTiledSprite");

typedef struct AnimatedTexturesSprite {
	Entity entity; // cannot be moved from 1st position

	Texture2D *textures;
	i32 framesLen;
	i32 currentFrame;
	f32 frameTimer;
	f32 animationSpeed;
} AnimatedTexturesSprite;

static_assert(
	offsetof(AnimatedTexturesSprite, entity) == 0,
	"entity must be the first member of AnimatedTexturesSprite"
);

void animated_textures_sprite_update(AnimatedTexturesSprite *sprite, f32 dt);
void animated_tiled_sprite_update(AnimatedTiledSprite *sprite, f32 dt);
Rectangle animated_tiled_sprite_current_frame(const AnimatedTiledSprite *sprite);

#endif //RAYLIB_POKEMON_CLONE_SPRITES_H
