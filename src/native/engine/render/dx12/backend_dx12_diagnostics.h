#ifndef BANANA_ENGINE_RENDER_BACKEND_DX12_DIAGNOSTICS_H
#define BANANA_ENGINE_RENDER_BACKEND_DX12_DIAGNOSTICS_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum BananaDx12DiagnosticsProbeState
    {
        BANANA_DX12_DIAGNOSTICS_PROBE_UNKNOWN = 0,
        BANANA_DX12_DIAGNOSTICS_PROBE_AVAILABLE = 1,
        BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE = 2,
    } BananaDx12DiagnosticsProbeState;

    typedef struct BananaDx12DiagnosticsSnapshot
    {
        const char *status;
        int active;
        int width;
        int height;
        unsigned long long frame_counter;
        unsigned long long frames_presented;
        unsigned int scene_draw_calls_frame;
        unsigned long long scene_draw_calls_total;
        unsigned int present_interval;
        unsigned long last_present_result;
        int ui_overlay_width;
        int ui_overlay_height;
        int ui_overlay_dirty;
        unsigned int ui_overlay_rows_last;
        unsigned int ui_overlay_quads_last;
        unsigned int ui_overlay_vertices_last;
    } BananaDx12DiagnosticsSnapshot;

    BananaDx12DiagnosticsProbeState banana_dx12_diagnostics_probe_state(void);
    void banana_dx12_diagnostics_set_probe_state(BananaDx12DiagnosticsProbeState state,
                                                 const char *status);
    const char *banana_dx12_diagnostics_probe_status(void);

    const char *banana_dx12_diagnostics_runtime_telemetry(const BananaDx12DiagnosticsSnapshot *snapshot);
    const char *banana_dx12_diagnostics_runtime_unavailable_telemetry(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_RENDER_BACKEND_DX12_DIAGNOSTICS_H */