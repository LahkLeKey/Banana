#ifndef BANANA_ENGINE_RUNTIME_INTERLOCK_H
#define BANANA_ENGINE_RUNTIME_INTERLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeInterlockContext
    {
        int initialized;
        int contention_hits;
        void *native_lock;
    } RuntimeInterlockContext;

    int runtime_interlock_init(RuntimeInterlockContext *ctx);
    void runtime_interlock_destroy(RuntimeInterlockContext *ctx);

    void runtime_interlock_enter(RuntimeInterlockContext *ctx);
    void runtime_interlock_leave(RuntimeInterlockContext *ctx);

    int runtime_interlock_contention_hits(const RuntimeInterlockContext *ctx);

#ifdef __cplusplus
}
#endif

#endif
