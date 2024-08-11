#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include "../memory/memory.h"

// todo(hector) - make this header into a struct?
#define ARRAY_RAW_DATA(array) ((int*)(array) - 3)
#define ARRAY_CAPACITY(array) (ARRAY_RAW_DATA(array)[0])
#define ARRAY_OCCUPIED(array) (ARRAY_RAW_DATA(array)[1])
#define ARRAY_ITEM_SIZE(array) (ARRAY_RAW_DATA(array)[2])

const u32 headerSize = (sizeof(int) * 3);

void *array_hold(void *array, int count, int item_size) {
    if (array == nil) {
        const u32 rawSize = headerSize + (item_size * count);
        int *base = mallocate(rawSize, MemoryTagDynArray);
        base[0] = count;  // capacity
        base[1] = count;  // occupied
        base[2] = item_size;  // each item size
        return base + 3;
    }
    if (ARRAY_OCCUPIED(array) + count <= ARRAY_CAPACITY(array)) {
        ARRAY_OCCUPIED(array) += count;
        return array;
    }
    const int needed_size = ARRAY_OCCUPIED(array) + count;
    const int double_curr = ARRAY_CAPACITY(array) * 2;
    const int newCapacity = needed_size > double_curr ? needed_size : double_curr;
    const int occupied = needed_size;
    const u32 oldRawSize = headerSize + (ARRAY_CAPACITY(array) * ARRAY_ITEM_SIZE(array));
    const u32 newRawSize = headerSize + (item_size * newCapacity);

    int *oldBase = ARRAY_RAW_DATA(array);
    int *newBase = mallocate(newRawSize, MemoryTagDynArray);
    mcopy_memory(newBase, oldBase, oldRawSize);
    mfree(oldBase, oldRawSize, MemoryTagDynArray);

    newBase[0] = newCapacity;
    newBase[1] = occupied;
    newBase[2] = item_size;
    return newBase + 3;
}

int array_length(void *array) {
    return (array != nil) ? ARRAY_OCCUPIED(array) : 0;
}

int array_cap(void *array) {
    return array != nil ? ARRAY_CAPACITY(array) : 0;
}

void array_free(void *array) {
    if (array != nil) {
        const u32 arraySize = headerSize + (ARRAY_ITEM_SIZE(array) * ARRAY_CAPACITY(array));
        mfree(ARRAY_RAW_DATA(array), arraySize, MemoryTagDynArray);
        return;
    }
    slogw("called array_free on null pointer");
}

void array_remove(void *array, int index, int item_size) {
    const int count = ARRAY_OCCUPIED(array);
    for (int i = index; i < count; i++) {
        char *dest = (char *) array + (i * item_size);
        const char *src = (char *) array + ((i + 1) * item_size);
        mcopy_memory(dest, src, item_size);
    }
    ARRAY_OCCUPIED(array) -= 1;
}
