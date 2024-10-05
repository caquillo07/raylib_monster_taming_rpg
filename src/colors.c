//
// Created by Hector Mejia on 8/13/24.
//

#include "colors.h"

Color color_with_alpha(Color c, i32 a) {
	c.a = a;
	return c;
}
