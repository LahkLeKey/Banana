#ifndef BANANA_ENGINE_RENDER_BACKEND_H
#define BANANA_ENGINE_RENDER_BACKEND_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum BananaRenderBackend
    {
        BANANA_RENDER_BACKEND_UNKNOWN = 0,
        BANANA_RENDER_BACKEND_GLFW = 1,
        BANANA_RENDER_BACKEND_DX12 = 2,
        BANANA_RENDER_BACKEND_WASM = 3,
        BANANA_RENDER_BACKEND_HEADLESS = 4,
    } BananaRenderBackend;

    BananaRenderBackend banana_render_backend_requested(void);
    BananaRenderBackend banana_render_backend_active(void);
    const char *banana_render_backend_name(BananaRenderBackend backend);
    const char *banana_render_backend_status(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_RENDER_BACKEND_H */
