#include "banana_wrapper.h"

#include <stdlib.h>

void banana_free(void* pointer) {
    if (pointer != 0) {
        free(pointer);
    }
}