#ifndef BANANA_ENGINE_WILDLIFE_CONTROLLER_H
#define BANANA_ENGINE_WILDLIFE_CONTROLLER_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* Register the "wildlife" controller type with the controller registry.
     * Must be called once before any engine_controller_attach("wildlife", ...) call. */
    void wildlife_controller_register(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_WILDLIFE_CONTROLLER_H */
