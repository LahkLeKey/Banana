/* test_abi_version.c — ABI version contract tests (feature 072). */
#include "../../src/native/wrapper/banana_wrapper.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    int major = 0, minor = 0;
    int rc = banana_native_version(&major, &minor);
    assert(rc == 0);
    assert(major == BANANA_WRAPPER_ABI_VERSION_MAJOR);
    assert(minor == BANANA_WRAPPER_ABI_VERSION_MINOR);
    /* Enforce minimum known version so CI catches regressions */
    assert(major >= 2);
    assert(minor >= 4);
    printf("[native-unit] banana_native_version: %d.%d ok\n", major, minor);
    return 0;
}
