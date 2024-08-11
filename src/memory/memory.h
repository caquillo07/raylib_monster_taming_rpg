//
// Created by Hector Mejia on 5/30/24.
//

#ifndef RAYLIB_POKEMON_CLONE_MEMORY_H
#define RAYLIB_POKEMON_CLONE_MEMORY_H

#include "../common.h"
// todo https://github.com/raysan5/raylib/blob/37205bba84263c5168f7aaeef4da9c43c56a87e1/src/raylib.h#L130
//  will require recompile

typedef enum memory_tag {
    // For temporary use. Should be assigned one of the below or have a new tag created.
    MemoryTagUnknown,
    MemoryTagUntraceable,
    MemoryTagArray,
    MemoryTagDynArray,
    MemoryTagString,
    MemoryTagTexture,
    MemoryTagGame,
    MemoryTagMap,
    MemoryTagEntity,
    MemoryTagResource,
    MemoryTagFile,
    MemoryTagJSON,

    MemoryTagMaxTags
} memory_tag;

#define GiB  (1024 * 1024 * 1024)
#define MiB  (1024 * 1024)
#define KiB  (1024)

void initialize_memory();
void shutdown_memory();

void *mallocate(u64 size, memory_tag tag);
void mfree(void *block, u64 size, memory_tag tag);
void *mzero_memory(void *block, u64 size);
void *mcopy_memory(void *dest, const void *source, u64 size);
void *mset_memory(void *dest, i32 value, u64 size);
char *get_memory_usage_str();

#endif //RAYLIB_POKEMON_CLONE_MEMORY_H
