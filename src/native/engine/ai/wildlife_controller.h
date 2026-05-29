#ifndef BANANA_ENGINE_WILDLIFE_CONTROLLER_H
#define BANANA_ENGINE_WILDLIFE_CONTROLLER_H

#include "controller.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Register the "wildlife" controller type with the controller registry.
     * Must be called once before any engine_controller_attach("wildlife", ...) call. */
    void wildlife_controller_register(void);

    /* Debug helper for tests/instrumentation. Returns NULL when controller is not wildlife. */
    const char *wildlife_controller_debug_state_name(const ControllerInstance *controller);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_WILDLIFE_CONTROLLER_H */
