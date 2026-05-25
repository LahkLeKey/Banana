#include "interlock.h"

#if defined(_OPENMP) && defined(__has_include)
#if __has_include(<omp.h>)
#define BANANA_ENGINE_USE_OMP_LOCKS 1
#endif
#endif

#ifdef BANANA_ENGINE_USE_OMP_LOCKS
#include <omp.h>
#else
#include <pthread.h>
#endif
#include <stdlib.h>

typedef struct RuntimeOmpLock
{
#ifdef BANANA_ENGINE_USE_OMP_LOCKS
    omp_lock_t lock;
#else
    pthread_mutex_t lock;
#endif
} RuntimeOmpLock;

int runtime_interlock_init(RuntimeInterlockContext *ctx)
{
    RuntimeOmpLock *lock = NULL;

    if (!ctx)
        return 0;

    lock = (RuntimeOmpLock *)malloc(sizeof(RuntimeOmpLock));
    if (!lock)
        return 0;

#ifdef BANANA_ENGINE_USE_OMP_LOCKS
    omp_init_lock(&lock->lock);
#else
    if (pthread_mutex_init(&lock->lock, NULL) != 0)
    {
        free(lock);
        return 0;
    }
#endif
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
#ifdef BANANA_ENGINE_USE_OMP_LOCKS
        omp_destroy_lock(&lock->lock);
#else
        pthread_mutex_destroy(&lock->lock);
#endif
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

#ifdef BANANA_ENGINE_USE_OMP_LOCKS
    if (!omp_test_lock(&lock->lock))
    {
        omp_set_lock(&lock->lock);
        ctx->contention_hits += 1;
    }
#else
    if (pthread_mutex_trylock(&lock->lock) != 0)
    {
        pthread_mutex_lock(&lock->lock);
        ctx->contention_hits += 1;
    }
#endif
}

void runtime_interlock_leave(RuntimeInterlockContext *ctx)
{
    RuntimeOmpLock *lock = NULL;

    if (!ctx || !ctx->initialized)
        return;

    lock = (RuntimeOmpLock *)ctx->native_lock;
    if (!lock)
        return;

#ifdef BANANA_ENGINE_USE_OMP_LOCKS
    omp_unset_lock(&lock->lock);
#else
    pthread_mutex_unlock(&lock->lock);
#endif
}

int runtime_interlock_contention_hits(const RuntimeInterlockContext *ctx)
{
    if (!ctx)
        return 0;

    return ctx->contention_hits;
}
