#include "runtime/controller_sync.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    if (!expect_int("parallel available", runtime_controller_sync_parallel_available(), 1))
        return 1;

    if (!expect_int("parallel contract is always on", runtime_controller_sync_parallel_available(), 1))
        return 1;

    return 0;
}
