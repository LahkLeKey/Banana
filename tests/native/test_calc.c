// Native unit tests (spec 014) — minimal smoke for banana_calculate_banana.
#include "../../src/native/wrapper/banana_wrapper.h"
#include <stdio.h>

#define CHECK(cond, msg)                              \
    do                                                \
    {                                                 \
        if (!(cond))                                  \
        {                                             \
            fprintf(stderr, "[native-unit] %s\n", msg); \
            return 1;                                 \
        }                                             \
    } while (0)

int main(void) {
    int out = 0;
    int rc = banana_calculate_banana(3, 2, &out);
    CHECK(rc == 0, "banana_calculate_banana returned non-zero");
    CHECK(out == 6, "banana_calculate_banana produced unexpected output");
    printf("[native-unit] banana_calculate_banana ok\n");
    return 0;
}
