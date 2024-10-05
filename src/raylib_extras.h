//
// Created by Hector Mejia on 7/22/24.
//

#ifndef RAYLIB_EXTRAS_H
#define RAYLIB_EXTRAS_H
#include <raylib.h>

#include "common.h"
#include "assets.h"

// Images
Rectangle rectangle_from_image(Image img);

// Textures
Rectangle rectangle_from_texture(Texture2D text);

// Rectangles
Rectangle rectangle_inflate(Rectangle rect, f32 widthAmount, f32 heightAmount);
Rectangle rectangle_deflate(Rectangle rect, f32 widthAmount, f32 heightAmount);
Vector2 rectangle_center(Rectangle rect);
Vector2 rectangle_mid_left(Rectangle rect);
Vector2 rectangle_mid_right(Rectangle rect);
Vector2 rectangle_top_right(Rectangle rect);
Vector2 rectangle_bottom_left(Rectangle rect);
Vector2 rectangle_location(Rectangle rect);
Rectangle rectangle_at(Rectangle rect, Vector2 pos);
Rectangle rectangle_with_center_at(Rectangle rect, Vector2 pos);
Rectangle rectangle_with_mid_right_at(Rectangle rect, Vector2 pos);
Rectangle rectangle_with_mid_left_at(Rectangle rect, Vector2 pos);
Rectangle rectangle_move_by(Rectangle rect, Vector2 direction);

// Vectors

// Text
Rectangle text_rectangle_at(const char* text, GameFont font, Vector2 pos);
Rectangle text_rectangle_centered_at(const char* text, GameFont font, Vector2 pos);
Rectangle text_rectangle_mid_left_at(const char *text, GameFont font, Vector2 pos);

#endif //RAYLIB_EXTRAS_H
