#include "player_runtime_service.h"

#include "../../camera/follow/camera_follow.h"
#include "../../input/contract/input_contract.h"
#include "../player_motion_host.h"

void runtime_player_runtime_update_motion(World *world,
                                          EntityId player_id,
                                          RuntimeMoveTargetState *move_target_state,
                                          float *move_input_x,
                                          float *move_input_z,
                                          const float *camera_eye,
                                          const float *camera_target,
                                          int camera_valid,
                                          float speed,
                                          float dt,
                                          float island_span,
                                          RuntimeTerrainSampleHeightFn sample_height)
{
    float next_move_x = 0.f;
    float next_move_z = 0.f;

    if (!move_target_state || !move_input_x || !move_input_z)
        return;

    next_move_x = *move_input_x;
    next_move_z = *move_input_z;

    if (move_target_state->has_target && world && player_id)
    {
        Entity *player = world_get_entity(world, player_id);
        if (player && player->active)
        {
            RuntimeMoveTargetSteering steering = {0};
            if (runtime_move_target_compute_steering(move_target_state,
                                                     player->position,
                                                     camera_eye,
                                                     camera_target,
                                                     camera_valid,
                                                     0.35f,
                                                     &steering) &&
                steering.reached_target)
            {
                runtime_move_target_clear(move_target_state);
                runtime_input_contract_mark_target_reached();
                next_move_x = 0.f;
                next_move_z = 0.f;
            }
            else
            {
                next_move_x = steering.move_x;
                next_move_z = steering.move_z;
                runtime_input_contract_set_has_move_target(steering.has_target);
            }
        }
        else
        {
            runtime_move_target_clear(move_target_state);
            runtime_input_contract_set_has_move_target(0);
        }
    }

    runtime_player_motion_tick(world,
                               player_id,
                               next_move_x,
                               next_move_z,
                               camera_eye,
                               camera_target,
                               camera_valid,
                               speed,
                               dt,
                               island_span,
                               sample_height);

    *move_input_x = next_move_x;
    *move_input_z = next_move_z;
}

void runtime_player_runtime_follow_camera(Renderer *renderer,
                                          World *world,
                                          EntityId player_id,
                                          int viewport_width,
                                          int viewport_height,
                                          float *camera_eye,
                                          float *camera_target,
                                          int *camera_valid)
{
    runtime_camera_follow_player(renderer,
                                 world,
                                 player_id,
                                 viewport_width,
                                 viewport_height,
                                 camera_eye,
                                 camera_target,
                                 camera_valid);
}
