#ifndef BANANA_ENGINE_RUNTIME_ORCHESTRATION_RUNTIME_TICK_ORCHESTRATION_H
#define BANANA_ENGINE_RUNTIME_ORCHESTRATION_RUNTIME_TICK_ORCHESTRATION_H

#include "../../engine/application/service/application_service_ports.h"
#include "../../engine/tick/engine_tick.h"

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

    int runtime_tick_orchestration_execute(Window *window,
                                           Renderer *renderer,
                                           PhysicsWorld *physics_world,
                                           World *world,
                                           int *viewport_width,
                                           int *viewport_height,
                                           RuntimeTerrainRebuildFn terrain_rebuild,
                                           ControllerInstance **controllers,
                                           int controller_count,
                                           float dt,
                                           void *context,
                                           RuntimeTickUpdateFn update_player_motion,
                                           RuntimeTickVoidFn follow_player_camera,
                                           RuntimeTickClickInputFn apply_click_input,
                                           RuntimeTickGameplayFn run_gameplay,
                                           RuntimeTickVoidFn render_scene);

#ifdef __cplusplus
}
#endif

#endif
