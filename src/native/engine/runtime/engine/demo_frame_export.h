#ifndef BANANA_ENGINE_RUNTIME_DEMO_FRAME_EXPORT_H
#define BANANA_ENGINE_RUNTIME_DEMO_FRAME_EXPORT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DEMO_FRAME_EXPORT_MAX_PATH 512
#define DEMO_FRAME_EXPORT_MAX_LABEL 64
#define DEMO_FRAME_EXPORT_MAX_SCENARIOS 32

    typedef struct DemoFrameExportConfig
    {
        int enabled;
        int capture_interval_ticks;
        char format[8];
        char output_dir[DEMO_FRAME_EXPORT_MAX_PATH];
        char suite_label[DEMO_FRAME_EXPORT_MAX_LABEL];
        char run_label[DEMO_FRAME_EXPORT_MAX_LABEL];
    } DemoFrameExportConfig;

    typedef struct DemoFrameExportState
    {
        int enabled;
        int capture_interval_ticks;
        char format[8];
        char output_dir[DEMO_FRAME_EXPORT_MAX_PATH];
        char suite_label[DEMO_FRAME_EXPORT_MAX_LABEL];
        char run_label[DEMO_FRAME_EXPORT_MAX_LABEL];
        char run_id[DEMO_FRAME_EXPORT_MAX_LABEL];
        char created_at[32];
        char run_dir[DEMO_FRAME_EXPORT_MAX_PATH];
        void *frames;
        int frame_count;
        int frame_capacity;
        void *failures;
        int failure_count;
        int failure_capacity;
        char scenario_labels[DEMO_FRAME_EXPORT_MAX_SCENARIOS][DEMO_FRAME_EXPORT_MAX_LABEL];
        int scenario_count;
        unsigned char *scratch_bgra;
        size_t scratch_bgra_capacity;
    } DemoFrameExportState;

    void demo_frame_export_config_init(DemoFrameExportConfig *config);
    void demo_frame_export_config_apply_env(DemoFrameExportConfig *config,
                                            int default_interval_ticks,
                                            const char *default_output_dir);

    void demo_frame_export_state_init(DemoFrameExportState *state);

    int demo_frame_export_begin(DemoFrameExportState *state,
                                const DemoFrameExportConfig *config);
    int demo_frame_export_should_capture(const DemoFrameExportState *state, int tick);
    int demo_frame_export_capture_engine_frame(DemoFrameExportState *state,
                                                int64_t timestamp_ms);
    int demo_frame_export_capture_rgba(DemoFrameExportState *state,
                                       const unsigned char *rgba,
                                       int width,
                                       int height,
                                       int64_t timestamp_ms,
                                       const char *render_backend,
                                       int ui_overlay);
    int demo_frame_export_finalize(DemoFrameExportState *state);
    int demo_frame_export_has_failures(const DemoFrameExportState *state);
    int demo_frame_export_is_enabled(const DemoFrameExportState *state);

#ifdef __cplusplus
}
#endif

#endif
