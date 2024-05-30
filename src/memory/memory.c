//
// Created by Hector Mejia on 5/30/24.
//

#include "memory.h"

struct memory_stats {
    u64 total_allocated;
    u64 tagged_allocations[MemoryTagMaxTags];
};

static const char *memory_tag_strings[MemoryTagMaxTags] = {
    "UNKNOWN    ",
    "UNTRACEABLE",
    "ARRAY      ",
    "DYN_ARRAY  ",
    "STRING     ",
    "TEXTURE    ",
    "GAME       ",
    "MAP        ",
    "ENTITY     ",
    "RESOURCE   ",
};

typedef struct memory_system_state {
    struct memory_stats stats;
} memory_system_state;

// Pointer to system state.
static memory_system_state state;

void initialize_memory() {
    memset(&state, 0, sizeof(state));
}

void shutdown_memory() {
}

void *mallocate(u64 size, memory_tag tag) {
    if (tag == MemoryTagUnknown || tag == MemoryTagUntraceable) {
        slogw("mallocate called using %s. Re-class this allocation.", memory_tag_strings[tag]);
    }

    state.stats.total_allocated += size;
    state.stats.tagged_allocations[tag] += size;

    // TODO: Memory alignment?
    void *block = malloc(size);
    memset(block, 0, size);
    return block;
}

void mfree(void *block, u64 size, memory_tag tag) {
    if (tag == MemoryTagUnknown || tag == MemoryTagUntraceable) {
        slogw("mfree called using %s. Re-class this allocation.", memory_tag_strings[tag]);
    }

    state.stats.total_allocated -= size;
    state.stats.tagged_allocations[tag] -= size;

    // TODO: Memory alignment
    free(block);
}

void *mzero_memory(void *block, u64 size) {
    return memset(block, 0, size);
}

void *mcopy_memory(void *dest, const void *source, u64 size) {
    return memcpy(dest, source, size);
}

void *mset_memory(void *dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

char *get_memory_usage_str() {
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

#define bufferSize 8000
    char buffer[bufferSize] = "System memory use (tagged):\n";
    u64 offset = strlen(buffer);
    for (u32 i = 0; i < MemoryTagMaxTags; ++i) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if (state.stats.tagged_allocations[i] >= gib) {
            unit[0] = 'G';
            amount = (f32) state.stats.tagged_allocations[i] / (f32) gib;
        } else if (state.stats.tagged_allocations[i] >= mib) {
            unit[0] = 'M';
            amount = (f32) state.stats.tagged_allocations[i] / (f32) mib;
        } else if (state.stats.tagged_allocations[i] >= kib) {
            unit[0] = 'K';
            amount = (f32) state.stats.tagged_allocations[i] / (f32) kib;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (f32) state.stats.tagged_allocations[i];
        }

        i32 length = snprintf(buffer + offset, bufferSize, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }
    char *out_string = strdup(buffer);
    return out_string;
}

