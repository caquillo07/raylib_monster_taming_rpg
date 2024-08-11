//
// Created by Hector Mejia on 5/30/24.
//

#include "memory.h"

static void get_memory_unit_for_size(char buf[], u64 size);
static char get_memory_unit_for_size1(u64 size);
static f32 normalize_memory_size(u64 size);
static void print_memory_action(char action[], memory_tag tag, u64 size);

// todo(hector) - get a call stack going
struct memory_stats {
    u64 currentlyAllocated;
    u64 taggedAllocations[MemoryTagMaxTags];
    u64 totalAllocated;
    u64 totalAllocations;
    u64 totalFrees;
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
    "FILE       ",
    "JSON       ",
};

typedef struct memory_system_state {
    struct memory_stats stats;
} memory_system_state;

// Pointer to system state.
static memory_system_state state = {};

void initialize_memory() {
}

void shutdown_memory() {
}

void *mallocate(const u64 size, const memory_tag tag) {
    if (tag == MemoryTagUnknown || tag == MemoryTagUntraceable) {
        slogw("mallocate called using %s. Re-class this allocation.", memory_tag_strings[tag]);
    }

    state.stats.totalAllocated += size;
    state.stats.currentlyAllocated += size;
    state.stats.totalAllocations++;
    state.stats.taggedAllocations[tag] += size;

    // TODO: Memory alignment?
    // void *block = malloc(size);
    print_memory_action("mallocate", tag, size);

    void *block = calloc(1, size);
    return block;
}

// TODO - add a size header to this damn allocations so i can track them in free
void mfree(void *block, const u64 size, const memory_tag tag) {
    if (tag == MemoryTagUnknown || tag == MemoryTagUntraceable) {
        slogw("mfree called using %s. Re-class this allocation.", memory_tag_strings[tag]);
    }

    state.stats.currentlyAllocated -= size;
    state.stats.taggedAllocations[tag] -= size;
    state.stats.totalFrees++;

    print_memory_action("mfree", tag, size);
    // TODO: Memory alignment
    free(block);
}

void *mzero_memory(void *block, const u64 size) {
    return memset(block, 0, size);
}

void *mcopy_memory(void *dest, const void *source, const u64 size) {
    return memcpy(dest, source, size);
}

void *mset_memory(void *dest, const i32 value, const u64 size) {
    return memset(dest, value, size);
}

char *get_memory_usage_str() {
#define bufferSize 8000
    char buffer[bufferSize] = "System memory use (tagged):\n";
    u64 offset = strlen(buffer);
    u64 allocatedMemory = 0;
    for (u32 i = 0; i < MemoryTagMaxTags; ++i) {
        const u64 taggedSize = state.stats.taggedAllocations[i];
        char unit[4] = {};
        get_memory_unit_for_size(unit, taggedSize);
        const float amount = normalize_memory_size(taggedSize);

        const i32 length = snprintf(
            buffer + offset,
            bufferSize,
            "\t%s: %.2f%s\n",
            memory_tag_strings[i],
            amount,
            unit
        );
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
        normalize_memory_size(allocatedMemory),
        get_memory_unit_for_size1(allocatedMemory),
        allocatedMemory > 0 ? "\t\t<==============" : "",
        normalize_memory_size(state.stats.currentlyAllocated),
        get_memory_unit_for_size1(state.stats.currentlyAllocated),
        normalize_memory_size(state.stats.totalAllocated),
        get_memory_unit_for_size1(state.stats.totalAllocated),
        state.stats.totalAllocations,
        state.stats.totalFrees
    );

    // char *out_string = strdup(buffer);
    // return out_string;
    return strdup(buffer);
}


void get_memory_unit_for_size(char buf[], const u64 size) {
    if (size >= GiB) {
        strcpy(buf, "GiB");
    } else if (size >= MiB) {
        strcpy(buf, "MiB");
    } else if (size >= KiB) {
        strcpy(buf, "KiB");
    } else {
        strcpy(buf, "B\0\0");
    }
    buf[3] = '\0';
}

// todo need to get rid of this one
char get_memory_unit_for_size1(const u64 size) {
    if (size >= GiB) {
        return 'G';
    }
    if (size >= MiB) {
        return 'M';
    }
    if (size >= KiB) {
        return 'K';
    }
    return 'B';
}

f32 normalize_memory_size(const u64 size) {
    if (size >= GiB) {
        return (f32) size / (f32) GiB;
    }
    if (size >= MiB) {
        return (f32) size / (f32) MiB;
    }
    if (size >= KiB) {
        return (f32) size / (f32) KiB;
    }
    return (f32) size;
}

void print_memory_action(char action[], const memory_tag tag, const u64 size) {
    char unit[8] = {};
    get_memory_unit_for_size(unit, size);
    const float amount = normalize_memory_size(size);
    slog_debug("%s: %s %.2f%s", action, memory_tag_strings[tag], amount, unit);
}
