#ifndef BANANA_ENGINE_RUNTIME_ENGINE_HOST_H
#define BANANA_ENGINE_RUNTIME_ENGINE_HOST_H

#include "../../physics/world.h"
#include "../../render/mesh.h"
#include "../../render/renderer.h"
#include "../../render/window.h"
#include "../../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_engine_host_render_frame(Renderer *renderer);

    const char *runtime_engine_host_launch_gate_mode_label_for(const char *trusted_mode_label);

    void runtime_engine_host_reset_state(Window **window,
                                         Renderer **renderer,
                                         PhysicsWorld **physics,
                                         World **world,
                                         Mesh **entity_mesh,
                                         EntityId *player_id,
                                         int *terrain_initialized,
                                         int *viewport_width,
                                         int *viewport_height,
                                         int *engine_initialized,
                                         int *camera_valid,
                                         float *move_input_x,
                                         float *move_input_z);

#ifdef __cplusplus
}
#endif

#endif
