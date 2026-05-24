#include "runtime_tick_orchestration.h"

void runtime_tick_orchestration_update_player_motion(void *context, float dt)
{
    RuntimeTickOrchestrationContext *orchestration =
        (RuntimeTickOrchestrationContext *)context;

    if (!orchestration || !orchestration->state || !orchestration->sample_height ||
        !orchestration->service_ports || !orchestration->service_ports->player.update_motion)
        return;

    orchestration->service_ports->player.update_motion(orchestration->state,
                                                       dt,
                                                       orchestration->sample_height);
}

void runtime_tick_orchestration_follow_player_camera(void *context)
{
    RuntimeTickOrchestrationContext *orchestration =
        (RuntimeTickOrchestrationContext *)context;

    if (!orchestration || !orchestration->state || !orchestration->service_ports ||
        !orchestration->service_ports->player.follow_camera)
        return;

    orchestration->service_ports->player.follow_camera(orchestration->state);
}

void runtime_tick_orchestration_apply_click_move_input(void *context,
                                                       float normalized_x,
                                                       float normalized_y)
{
    RuntimeTickOrchestrationContext *orchestration =
        (RuntimeTickOrchestrationContext *)context;

    if (!orchestration || !orchestration->state || !orchestration->sample_height ||
        !orchestration->service_ports || !orchestration->service_ports->player.apply_click_input)
        return;

    orchestration->service_ports->player.apply_click_input(orchestration->state,
                                                           normalized_x,
                                                           normalized_y,
                                                           orchestration->sample_height);
}
