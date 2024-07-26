//
// Created by Hector Mejia on 7/22/24.
//

#ifndef RAYLIB_EXTRAS_H
#define RAYLIB_EXTRAS_H
#include <raylib.h>

#include "common.h"

Rectangle rectangle_inflate(Rectangle rect, f32 widthAmount, f32 heightAmount);
Rectangle rectangle_deflate(Rectangle rect, f32 widthAmount, f32 heightAmount);

#endif //RAYLIB_EXTRAS_H
