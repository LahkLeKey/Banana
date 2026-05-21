#include "interlock.h"

#include <omp.h>
#include <stdlib.h>

typedef struct RuntimeOmpLock
{
    omp_lock_t lock;
} RuntimeOmpLock;

int runtime_interlock_init(RuntimeInterlockContext *ctx)
{
    RuntimeOmpLock *lock = NULL;

    if (!ctx)
        return 0;

    lock = (RuntimeOmpLock *)malloc(sizeof(RuntimeOmpLock));
    if (!lock)
        return 0;

    omp_init_lock(&lock->lock);
    ctx->native_lock = lock;
    ctx->contention_hits = 0;
    ctx->initialized = 1;
    return 1;
}

void runtime_interlock_destroy(RuntimeInterlockContext *ctx)
{
    RuntimeOmpLock *lock = NULL;

    if (!ctx || !ctx->initialized)
        return;

    lock = (RuntimeOmpLock *)ctx->native_lock;
    if (lock)
    {
        omp_destroy_lock(&lock->lock);
        free(lock);
    }

    ctx->native_lock = NULL;
    ctx->contention_hits = 0;
    ctx->initialized = 0;
}

void runtime_interlock_enter(RuntimeInterlockContext *ctx)
{
    RuntimeOmpLock *lock = NULL;

    if (!ctx || !ctx->initialized)
        return;

    lock = (RuntimeOmpLock *)ctx->native_lock;
    if (!lock)
        return;

    if (!omp_test_lock(&lock->lock))
    {
        omp_set_lock(&lock->lock);
        ctx->contention_hits += 1;
    }
}

void runtime_interlock_leave(RuntimeInterlockContext *ctx)
{
    RuntimeOmpLock *lock = NULL;

    if (!ctx || !ctx->initialized)
        return;

    lock = (RuntimeOmpLock *)ctx->native_lock;
    if (!lock)
        return;

    omp_unset_lock(&lock->lock);
}

int runtime_interlock_contention_hits(const RuntimeInterlockContext *ctx)
{
    if (!ctx)
        return 0;

    return ctx->contention_hits;
}
