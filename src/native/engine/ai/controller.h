#ifndef BANANA_ENGINE_CONTROLLER_H
#define BANANA_ENGINE_CONTROLLER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    typedef uint32_t ControllerInstanceId;

    /* Forward declaration for self-reference in callbacks. */
    typedef struct ControllerInstance ControllerInstance;

    typedef struct ControllerInstance
    {
        ControllerInstanceId id;
        char type_name[64];
        float position[3]; /* mirrored from world entity */
        void *state;       /* controller-specific heap state */

        /* Callbacks — all required. */
        void (*update)(ControllerInstance *self, float dt);
        void (*on_signal)(ControllerInstance *self, const char *signal, const void *data);
        void (*destroy)(ControllerInstance *self);
    } ControllerInstance;

    /* Registry: register a controller type factory, or look up and instantiate. */
    typedef ControllerInstance *(*ControllerFactory)(float x, float y, float z);
    void controller_register(const char *type_name, ControllerFactory factory);

    ControllerInstance *controller_create(const char *type_name, float x, float y, float z);

    void controller_update(ControllerInstance *c, float dt);
    void controller_signal(ControllerInstance *c, const char *signal, const void *data);
    void controller_destroy(ControllerInstance *c);

    /* ── C ABI exports ────────────────────────────────────────────────────────── */
    uint32_t engine_controller_create(const char *type_name, float x, float y, float z);
    void engine_controller_update(uint32_t id, float dt);
    void engine_controller_signal(uint32_t id, const char *signal_name);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_CONTROLLER_H */
