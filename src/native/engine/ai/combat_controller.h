#ifndef BANANA_ENGINE_COMBAT_CONTROLLER_H
#define BANANA_ENGINE_COMBAT_CONTROLLER_H

#include "controller.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Register the "combat" controller type with the controller registry. */
    void combat_controller_register(void);

    /* Debug helper for tests/instrumentation. Returns NULL when controller is not combat. */
    const char *combat_controller_debug_mode(const ControllerInstance *controller);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_COMBAT_CONTROLLER_H */