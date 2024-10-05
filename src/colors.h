//
// Created by Hector Mejia on 8/13/24.
//

#ifndef COLORS_H
#define COLORS_H

#include <raylib.h>
#include "common.h"

typedef enum colors {
    ColorsWhite,
    ColorsPureWhite,
    ColorsDark,
    ColorsLight,
    ColorsGray,
    ColorsGold,
    ColorsLightGray,
    ColorsFire,
    ColorsWater,
    ColorsPlant,
    ColorsBlack,
    ColorsRed,
    ColorsBlue,
    ColorsNormal,
    ColorsDarkWhite,

    ColorsMax,
} Colors;

static const Color gameColors[ColorsMax] = {
    [ColorsWhite] = {.r = 244, .g = 254, .b = 250, .a = 255},
    [ColorsPureWhite] = {.r = 255, .g = 255, .b = 255, .a = 255},
    [ColorsDark] = {.r = 43, .g = 41, .b = 44, .a = 255},
    [ColorsLight] = {.r = 200, .g = 200, .b = 200, .a = 255},
    [ColorsGray] = {.r = 58, .g = 55, .b = 59, .a = 255},
    [ColorsGold] = {.r = 255, .g = 215, .b = 0, .a = 255},
    [ColorsLightGray] = {.r = 75, .g = 72, .b = 77, .a = 255},
    [ColorsFire] = {.r = 248, .g = 160, .b = 96, .a = 255},
    [ColorsWater] = {.r = 80, .g = 176, .b = 216, .a = 255},
    [ColorsPlant] = {.r = 100, .g = 169, .b = 144, .a = 255},
    [ColorsBlack] = {.r = 0, .g = 0, .b = 0, .a = 255},
    [ColorsRed] = {.r = 240, .g = 49, .b = 49, .a = 255},
    [ColorsBlue] = {.r = 102, .g = 215, .b = 238, .a = 255},
    [ColorsNormal] = {.r = 255, .g = 255, .b = 255, .a = 255},
    [ColorsDarkWhite] = {.r = 240, .g = 240, .b = 240, .a = 255},
};

Color color_with_alpha(Color c, i32 a);

#endif //COLORS_H
