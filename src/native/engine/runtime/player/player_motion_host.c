#include "player_motion_host.h"

#include "../camera/basis/camera_basis.h"
#include "player_registry.h"

void runtime_player_motion_tick(World *world,
                                EntityId primary_player_id,
                                float move_input_x,
                                float move_input_z,
                                const float *camera_eye,
                                const float *camera_target,
                                int camera_valid,
                                float speed,
                                float dt,
                                float island_span,
                                RuntimeTerrainSampleHeightFn sample_height)
{
    NativePlayerBinding *bindings[BANANA_MAX_NATIVE_PLAYERS] = {0};
    int binding_count = runtime_player_registry_count();
    float forward[3] = {0.f, 0.f, -1.f};
    float right[3] = {1.f, 0.f, 0.f};
    int i = 0;

    if (!world)
        return;

    (void)runtime_camera_compute_ground_basis(camera_eye,
                                              camera_target,
                                              camera_valid,
                                              forward,
                                              right);

    if (binding_count > BANANA_MAX_NATIVE_PLAYERS)
        binding_count = BANANA_MAX_NATIVE_PLAYERS;

    for (i = 0; i < binding_count; i++)
        bindings[i] = runtime_player_registry_get(i);

    runtime_player_motion_apply(world,
                                bindings,
                                binding_count,
                                primary_player_id,
                                move_input_x,
                                move_input_z,
                                forward,
                                right,
                                speed,
                                dt,
                                island_span,
                                sample_height);
}
