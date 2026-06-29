#ifndef BANANA_ENGINE_CONTROLLER_SYSTEM_H
#define BANANA_ENGINE_CONTROLLER_SYSTEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "controller.h"
#include <stddef.h>

    /* ── Controller System ───────────────────────────────────────────────────────
     * Manages a pool of active ControllerInstances.
     * Suitable for standalone use in tests or as the engine's AI subsystem.
     * ─────────────────────────────────────────────────────────────────────────── */

#define BANANA_CTRL_SYS_CAPACITY 256

    typedef void *(*ControllerSystemCallocFn)(size_t count, size_t size);

    typedef struct
    {
        ControllerInstance *instances[BANANA_CTRL_SYS_CAPACITY];
        int count;
        uint32_t next_id;
    } ControllerSystem;

    /* Create an empty controller system. */
    ControllerSystem *controller_system_create(void);

    /* Override/reset allocator hook used by controller_system_create. */
    void controller_system_set_calloc_hook(ControllerSystemCallocFn hook);
    void controller_system_reset_calloc_hook(void);

    /* Spawn a controller of given type at (x, y, z).
     * Returns the new controller's ID (0 on failure). */
    uint32_t controller_system_spawn(ControllerSystem *sys, const char *type, float x, float y,
                                     float z);

    /* Step all controllers forward by dt seconds. */
    void controller_system_update(ControllerSystem *sys, float dt);

    /* Broadcast a signal to all controllers. */
    void controller_system_signal_all(ControllerSystem *sys, const char *signal, const void *data);

    /* Broadcast to controllers whose type_name matches (NULL = all). */
    void controller_system_signal_type(ControllerSystem *sys, const char *type, const char *signal,
                                       const void *data);

    /* Destroy the system and all active controllers. */
    void controller_system_destroy(ControllerSystem *sys);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_CONTROLLER_SYSTEM_H */
