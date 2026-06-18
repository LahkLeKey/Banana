#ifndef BANANA_NATIVE_RUNTIME_TEST_SUPPORT_H
#define BANANA_NATIVE_RUNTIME_TEST_SUPPORT_H

#include <stdio.h>

static inline int banana_test_fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

#endif
