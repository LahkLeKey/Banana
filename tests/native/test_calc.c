// Native unit tests (spec 014) — minimal smoke for banana_calculate_banana.
#include "../../src/native/wrapper/banana_wrapper.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    int out = 0;
    int rc = banana_calculate_banana(3, 2, &out);
    assert(rc == 0);
    assert(out == 6);
    printf("[native-unit] banana_calculate_banana ok\n");
    return 0;
}
