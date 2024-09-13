//
// Created by Hector Mejia on 9/13/24.
//

#include "ui.h"

void ui_draw_progress_bar(Rectangle rect, f32 value, f32 maxValue, Color color, Color bgColor, f32 radius) {
    f32 ratio = rect.width / maxValue;
    Rectangle progressRect = rect;
    progressRect.width = value * ratio;
    DrawRectangleRounded(rect, radius, 4, bgColor);
    DrawRectangleRounded(progressRect, radius, 4, color);
}
