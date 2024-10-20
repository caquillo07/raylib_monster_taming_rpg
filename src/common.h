//
// Created by Hector Mejia on 3/15/24.
//

#ifndef POKEMON_GAME_COMMON_H
#define POKEMON_GAME_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <slog.h>
#include <assert.h>

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

#define panic(...) slogf(__VA_ARGS__); assert(0);
#define panicIf(condition, ...) if (condition) { panic(__VA_ARGS__); }
#define panicIfNil(ptr, ...) if (ptr == nil) { panic( __VA_ARGS__); }

#define printfln(str, ...) printf(str "\n", ##__VA_ARGS__)

// cant forget the null character at the end of the string .-.
#define streq(str, val) (strncmp(str, val, strlen(val)+1) == 0)

/* Compile time macros
 *	- Anything that starts with `comptime` is an expression
 *	- Anything in the for of `_comptime_<something>_` is an attribute
 */
#define comptime_array_len(arr) ((arr) != nil ? sizeof(arr)/sizeof((arr)[0]) : 0)
#define comptime_member_size(type, member) (sizeof( ((type *)0)->member ))
#define _comptime_unused_ __attribute__((unused))

void initLogger();
f32 clamp(f32 n, f32 mi, f32 ma);
f32 rand_f32(f32 min, f32 max);

#endif //POKEMON_GAME_COMMON_H
