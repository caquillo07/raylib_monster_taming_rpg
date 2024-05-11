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

// todo: make this env driven
#define DEBUG 1

#define min(_a, _b) ({ __typeof__(_a) __a = (_a), __b = (_b); __a < __b ? __a : __b; })
#define max(_a, _b) ({ __typeof__(_a) __a = (_a), __b = (_b); __a > __b ? __a : __b; })

#define nil NULL
#define bool uint8_t
#define true 1
#define false 0
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

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FPS 120
#define TILE_SIZE 64
#define ANIMATION_SPEED 6
#define BATTLE_OUTLINE_WIDTH 4

void initLogger();
void panic(const char *message);
void panicIf(bool condition, const char *message);

#endif //POKEMON_GAME_COMMON_H
