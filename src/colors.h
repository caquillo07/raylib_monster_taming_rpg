//
// Created by Hector Mejia on 8/13/24.
//

#ifndef COLORS_H
#define COLORS_H
#include <raylib.h>

typedef enum colors {
    ColorsWhite,
    ColorsPurewhite,
    ColorsDark,
    ColorsLight,
    ColorsGray,
    ColorsGold,
    ColorsLightGray,
    ColorsFir,
    ColorsWate,
    ColorsPlant,
    ColorsBlack,
    ColorsRed,
    ColorsBlue,
    ColorsNormal,
    ColorsDarkWhite,

    ColorsMax,
} Colors;

static Color gameColors[ColorsMax] = {
    {.r = 244, .g = 254, .b = 250, .a = 255},
    {.r = 255, .g = 255, .b = 255, .a = 255},
    {.r = 43, .g = 41, .b = 44, .a = 255},
    {.r = 200, .g = 200, .b = 200, .a = 255},
    {.r = 58, .g = 55, .b = 59, .a = 255},
    {.r = 255, .g = 215, .b = 0, .a = 255},
    {.r = 75, .g = 72, .b = 77, .a = 255},
    {.r = 248, .g = 160, .b = 96, .a = 255},
    {.r = 80, .g = 176, .b = 216, .a = 255},
    {.r = 100, .g = 169, .b = 144, .a = 255},
    {.r = 0, .g = 0, .b = 0, .a = 255},
    {.r = 240, .g = 49, .b = 49, .a = 255},
    {.r = 102, .g = 215, .b = 238, .a = 255},
    {.r = 255, .g = 255, .b = 255, .a = 255},
    {.r = 240, .g = 240, .b = 240, .a = 255},
};

#endif //COLORS_H
