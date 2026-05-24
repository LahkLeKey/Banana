#include "backend.h"
#include "backend_dx12.h"

BananaRenderBackend banana_render_backend_requested(void)
{
#ifdef __EMSCRIPTEN__
    return BANANA_RENDER_BACKEND_WASM;
#elif defined(BANANA_ENGINE_RENDER_BACKEND_DX12)
    return BANANA_RENDER_BACKEND_DX12;
#elif defined(BANANA_ENGINE_RENDER_BACKEND_GLFW)
    return BANANA_RENDER_BACKEND_GLFW;
#else
    return BANANA_RENDER_BACKEND_HEADLESS;
#endif
}

BananaRenderBackend banana_render_backend_active(void)
{
#ifdef __EMSCRIPTEN__
    return BANANA_RENDER_BACKEND_WASM;
#elif defined(BANANA_ENGINE_RENDER_BACKEND_DX12)
    if (banana_dx12_backend_is_available())
    {
        return BANANA_RENDER_BACKEND_DX12;
    }
    return BANANA_RENDER_BACKEND_HEADLESS;
#elif defined(BANANA_ENGINE_HAS_GLFW)
    return BANANA_RENDER_BACKEND_GLFW;
#else
    return BANANA_RENDER_BACKEND_HEADLESS;
#endif
}

const char *banana_render_backend_name(BananaRenderBackend backend)
{
    switch (backend)
    {
    case BANANA_RENDER_BACKEND_GLFW:
        return "glfw";
    case BANANA_RENDER_BACKEND_DX12:
        return "dx12";
    case BANANA_RENDER_BACKEND_WASM:
        return "wasm";
    case BANANA_RENDER_BACKEND_HEADLESS:
        return "headless";
    default:
        return "unknown";
    }
}

const char *banana_render_backend_status(void)
{
#ifdef __EMSCRIPTEN__
    return "wasm-webgl";
#elif defined(BANANA_ENGINE_RENDER_BACKEND_DX12)
    return banana_dx12_backend_status();
#elif defined(BANANA_ENGINE_HAS_GLFW)
    return "glfw-opengl";
#else
    return "headless-stub";
#endif
}
