//
// Created by Hector Mejia on 5/30/24.
//

#include "memory.h"

static char get_memory_unit_for_size(u64 size);
static f32 normalize_memory_size(u64 size);

// todo(hector) - get a call stack going
struct memory_stats {
    u64 currentlyAllocated;
    u64 taggedAllocations[MemoryTagMaxTags];
    u64 totalAllocated;
    u64 totalAllocations;
    u64 totalFrees;
};

const u64 gib = 1024 * 1024 * 1024;
const u64 mib = 1024 * 1024;
const u64 kib = 1024;

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
static memory_system_state state = {};

void initialize_memory() {}

void shutdown_memory() {}

void *mallocate(u64 size, memory_tag tag) {
    if (tag == MemoryTagUnknown || tag == MemoryTagUntraceable) {
        slogw("mallocate called using %s. Re-class this allocation.", memory_tag_strings[tag]);
    }

    state.stats.totalAllocated += size;
    state.stats.currentlyAllocated += size;
    state.stats.totalAllocations++;
    state.stats.taggedAllocations[tag] += size;

    // TODO: Memory alignment?
//    void *block = malloc(size);
//    memset(block, 0, size);
    void *block = calloc(1, size);
    return block;
}

void mfree(void *block, u64 size, memory_tag tag) {
    if (tag == MemoryTagUnknown || tag == MemoryTagUntraceable) {
        slogw("mfree called using %s. Re-class this allocation.", memory_tag_strings[tag]);
    }

    state.stats.currentlyAllocated -= size;
    state.stats.taggedAllocations[tag] -= size;
    state.stats.totalFrees++;

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
#define bufferSize 8000
    char buffer[bufferSize] = "System memory use (tagged):\n";
    u64 offset = strlen(buffer);
    u64 allocatedMemory = 0;
    for (u32 i = 0; i < MemoryTagMaxTags; ++i) {

        char unit[4] = "XiB";
        u64 taggedSize = state.stats.taggedAllocations[i];
        float amount = normalize_memory_size(taggedSize);
        unit[0] = get_memory_unit_for_size(taggedSize);
        if (unit[0] == 'B') {
            unit[1] = 0;
        }

        i32 length = snprintf(buffer + offset, bufferSize, "\t%s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
        allocatedMemory += taggedSize;
    }
    snprintf(
        buffer + offset,
        bufferSize,
        "\tTagged memory not freed: %.2f%c%s\n"
        "\tMemory currently allocated: %.2f%c\n"
        "\tTotal memory allocated: %.2f%c\n"
        "\tTotal memory allocations: %llu\n"
        "\tTotal memory frees: %llu\n"
        "",
        normalize_memory_size(allocatedMemory), get_memory_unit_for_size(allocatedMemory), allocatedMemory > 0 ? "\t\t<==============" : "",
        normalize_memory_size(state.stats.currentlyAllocated), get_memory_unit_for_size(state.stats.currentlyAllocated),
        normalize_memory_size(state.stats.totalAllocated), get_memory_unit_for_size(state.stats.totalAllocated),
        state.stats.totalAllocations,
        state.stats.totalFrees
    );

    char *out_string = strdup(buffer);
    return out_string;
}


char get_memory_unit_for_size(u64 size) {
    if (size >= gib) {
        return 'G';
    } else if (size >= mib) {
        return 'M';
    } else if (size >= kib) {
        return 'K';
    }
    return 'B';
}

f32 normalize_memory_size(u64 size) {
    if (size >= gib) {
        return (f32) size / (f32) gib;
    } else if (size >= mib) {
        return (f32) size / (f32) mib;
    } else if (size >= kib) {
        return (f32) size / (f32) kib;
    }
    return (f32) size;
}
