//
// Created by Hector Mejia on 7/22/24.
//

#include "raylib_extras.h"
#include "common.h"

#include <raymath.h>

// Images

Rectangle rectangle_from_image(Image img) {
	return (Rectangle){
		.width = (f32)img.width,
		.height = (f32)img.height,
	};
}

// Rectangles
Rectangle rectangle_inflate(const Rectangle rect, const f32 widthAmount, const f32 heightAmount) {
	const f32 newHeight = rect.height + heightAmount;
	const f32 newWidth = rect.width + widthAmount;
	return (Rectangle){
		.x = rect.x + (rect.width - newWidth) / 2,
		.y = rect.y + (rect.height - newHeight) / 2,
		.height = newHeight,
		.width = newWidth,
	};
}

Rectangle rectangle_deflate(const Rectangle rect, const f32 widthAmount, const f32 heightAmount) {
	return rectangle_inflate(rect, -widthAmount, -heightAmount);
}

Vector2 rectangle_center(Rectangle rect) {
	return (Vector2){
		.x = rect.x + (rect.width / 2),
		.y = rect.y + (rect.height / 2),
	};
}

Vector2 rectangle_mid_left(Rectangle rect) {
	return (Vector2){
		.x = rect.x,
		.y = rect.y + (rect.height / 2),
	};
}

Vector2 rectangle_mid_right(Rectangle rect) {
	return (Vector2){
		.x = rect.x + rect.width,
		.y = rect.y + (rect.height / 2),
	};
}

Vector2 rectangle_top_right(Rectangle rect) {
	return (Vector2){
		.x = rect.x + rect.width,
		.y = rect.y,
	};
}

Vector2 rectangle_location(Rectangle rect) {
	return (Vector2){
		.x = rect.x,
		.y = rect.y,
	};
}

Rectangle rectangle_with_center_at(Rectangle rect, Vector2 pos) {
	rect.x = pos.x - (rect.width / 2);
	rect.y = pos.y - (rect.height / 2);
	return rect;
}

Rectangle rectangle_with_mid_right_at(Rectangle rect, Vector2 pos) {
	rect.x = pos.x + rect.width;
	rect.y = pos.y - (rect.height / 2);
	return rect;
}

Rectangle rectangle_with_mid_left_at(Rectangle rect, Vector2 pos) {
	rect.x = pos.x;
	rect.y = pos.y - (rect.height / 2);
	return rect;
}

Rectangle rectangle_move_by(Rectangle rect, Vector2 direction) {
	const Vector2 newPos = Vector2Add((Vector2){rect.x, rect.y}, direction);
	rect.x = newPos.x;
	rect.y = newPos.y;
	return rect;
}

// Textures

Rectangle rectangle_from_texture(Texture2D text) {
	return (Rectangle){
		.width = (f32)text.width,
		.height = (f32)text.height,
	};
}

Rectangle text_rectangle_at(const char *text, GameFont font, Vector2 pos) {
	const Vector2 textSize = MeasureTextEx(font.rFont, text, font.size, 1.f);

	return (Rectangle){
		.x = pos.x, .y = pos.y,
		.width = textSize.x,
		.height = textSize.y,
	};
}

Rectangle text_rectangle_centered_at(const char *text, GameFont font, Vector2 pos) {
	Rectangle rect = text_rectangle_at(text, font, pos);
	rect.x -= rect.width / 2;
	rect.y -= rect.height / 2;
	return rect;
}

Rectangle text_rectangle_mid_left_at(const char *text, GameFont font, Vector2 pos) {
	Rectangle rect = text_rectangle_at(text, font, pos);
	rect.y -= rect.height / 2;
	return rect;
}

Rectangle rectangle_at(Rectangle rect, Vector2 pos) {
	rect.x = pos.x;
	rect.y = pos.y;
	return rect;
}

