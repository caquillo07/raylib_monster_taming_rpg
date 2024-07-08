//
// Created by Hector Mejia on 3/15/24.
//

#ifndef POKEMON_GAME_COMMON_H
#define POKEMON_GAME_COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <slog.h>
#include <stdnoreturn.h>
#include "raylib.h"

// todo: make this env driven
#define DEBUG 1

#define min(_a, _b) ({ __typeof__(_a) __a = (_a), __b = (_b); __a < __b ? __a : __b; })
#define max(_a, _b) ({ __typeof__(_a) __a = (_a), __b = (_b); __a > __b ? __a : __b; })

#define nil NULL
typedef float f32;
typedef double f64;
typedef uint8_t byte;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;
typedef ssize_t isize;

#define ScreenWidth 1280
#define ScreenHeight 720
#define PixelWindowHeight (ScreenHeight * 2)
#define TILE_SIZE 64
#define BATTLE_OUTLINE_WIDTH 4

typedef struct Size {
    f32 width;
    f32 height;
} Size;

#define panic(...) slogf(__VA_ARGS__); exit(EXIT_FAILURE);
#define panicIf(condition, ...) if (condition) { panic(__VA_ARGS__); }
#define panicIfNil(ptr, ...) if (ptr == nil) { panic( __VA_ARGS__); }

// cant forget the null character at the end of the string .-.
#define streq(str, val) (strncmp(str, val, strlen(val)+1) == 0)

void initLogger();

Size size_from_rectangle(Rectangle rect);

#endif //POKEMON_GAME_COMMON_H
