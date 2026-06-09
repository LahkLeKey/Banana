#include "engine_host.h"

#include "engine_aux_abi.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void runtime_engine_host_render_frame(Renderer *renderer)
{
    if (!renderer)
        return;

    renderer_begin_frame(renderer);
    renderer_end_frame(renderer);
}

const char *runtime_engine_host_launch_gate_mode_label_for(const char *trusted_mode_label)
{
    const char *trusted_mode = trusted_mode_label;
    const char *override_mode = getenv("BANANA_LAUNCH_GATE_MODE_OVERRIDE");
    banana_launch_gate_policy trusted_policy;
    banana_launch_gate_policy override_policy;

    if (!trusted_mode || trusted_mode[0] == '\0')
    {
        trusted_mode = getenv("BANANA_LAUNCH_GATE_MODE");
    }

    if (!trusted_mode || trusted_mode[0] == '\0')
    {
        trusted_mode = "development";
    }

    if (!override_mode || override_mode[0] == '\0')
    {
        return trusted_mode;
    }

    if (runtime_engine_aux_launch_gate_policy_resolve(trusted_mode, &trusted_policy) != 0)
    {
        return trusted_mode;
    }

    if (!trusted_policy.allow_override_context)
    {
        return trusted_mode;
    }

    if (runtime_engine_aux_launch_gate_policy_resolve(override_mode, &override_policy) != 0)
    {
        return trusted_mode;
    }

    return override_mode;
}

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
                                     float *move_input_z)
{
    if (entity_mesh)
        *entity_mesh = NULL;
    if (terrain_initialized)
        *terrain_initialized = 0;
    if (viewport_width)
        *viewport_width = 0;
    if (viewport_height)
        *viewport_height = 0;
    if (window)
        *window = NULL;
    if (renderer)
        *renderer = NULL;
    if (physics)
        *physics = NULL;
    if (world)
        *world = NULL;
    if (player_id)
        *player_id = 0;
    if (engine_initialized)
        *engine_initialized = 0;
    if (camera_valid)
        *camera_valid = 0;
    if (move_input_x)
        *move_input_x = 0.f;
    if (move_input_z)
        *move_input_z = 0.f;
}
