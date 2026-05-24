#include "runtime/parallel/interlock.h"

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
    RuntimeInterlockContext interlock;
    int counter = 0;
    int i = 0;

    if (!expect_int("init", runtime_interlock_init(&interlock), 1))
        return 1;

#pragma omp parallel for schedule(static)
    for (i = 0; i < 256; i++)
    {
        runtime_interlock_enter(&interlock);
        counter += 1;
        runtime_interlock_leave(&interlock);
    }

    if (!expect_int("counter", counter, 256))
        return 1;
    if (!expect_int("contention non-negative", runtime_interlock_contention_hits(&interlock) >= 0 ? 1 : 0, 1))
        return 1;

    runtime_interlock_destroy(&interlock);
    return 0;
}
