#ifndef BANANA_ENGINE_RUNTIME_ORCHESTRATION_RUNTIME_TICK_ORCHESTRATION_H
#define BANANA_ENGINE_RUNTIME_ORCHESTRATION_RUNTIME_TICK_ORCHESTRATION_H

#include "../application_service_ports.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeTickOrchestrationContext
    {
        EngineRuntimeState *state;
        RuntimeTerrainSampleHeightFn sample_height;
        const RuntimeApplicationServicePorts *service_ports;
    } RuntimeTickOrchestrationContext;

    int runtime_tick_orchestration_rebuild_dirty_chunks(void *context, int max_chunks);

    void runtime_tick_orchestration_update_player_motion(void *context, float dt);

    void runtime_tick_orchestration_follow_player_camera(void *context);

    void runtime_tick_orchestration_apply_click_move_input(void *context,
                                                           float normalized_x,
                                                           float normalized_y);

    void runtime_tick_orchestration_gameplay(void *context);

    void runtime_tick_orchestration_render_scene(void *context);

#ifdef __cplusplus
}
#endif

#endif
