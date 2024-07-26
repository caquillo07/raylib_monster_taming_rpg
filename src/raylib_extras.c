//
// Created by Hector Mejia on 7/22/24.
//

#include "raylib_extras.h"

#include "common.h"

Rectangle rectangle_inflate(const Rectangle rect, const f32 widthAmount, const f32 heightAmount) {
    const f32 newHeight = rect.height + heightAmount;
    const f32 newWidth = rect.width + widthAmount;
    return (Rectangle){
        .x = rect.x + (rect.width - newWidth)/2,
        .y = rect.y + (rect.height - newHeight)/2,
        .height = newHeight,
        .width = newWidth,
    };
}

Rectangle rectangle_deflate(const Rectangle rect, const f32 widthAmount, const f32 heightAmount) {
    return rectangle_inflate(rect, -widthAmount, -heightAmount);
}
