#ifndef BANANA_ENGINE_RENDER_BACKEND_DX12_H
#define BANANA_ENGINE_RENDER_BACKEND_DX12_H

#ifdef __cplusplus
extern "C"
{
#endif

    int banana_dx12_backend_is_available(void);
    const char *banana_dx12_backend_status(void);

    const char *banana_dx12_runtime_telemetry(void);
    unsigned long long banana_dx12_runtime_frames_presented(void);
    unsigned int banana_dx12_runtime_scene_draw_calls(void);

    int banana_dx12_runtime_init(void *native_window, int width, int height);
    int banana_dx12_runtime_resize(int width, int height);
    int banana_dx12_runtime_begin_frame(float clear_r, float clear_g, float clear_b, float clear_a);
    void banana_dx12_runtime_submit_scene_draw(const float *position,
                                               const float *scale,
                                               int uses_texture);
    int banana_dx12_runtime_end_frame(void);
    void banana_dx12_runtime_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_RENDER_BACKEND_DX12_H */
