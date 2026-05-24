#include "runtime_tick_orchestration.h"

#include "../../engine/gameplay_service.h"

void runtime_tick_orchestration_gameplay(void *context)
{
    RuntimeTickOrchestrationContext *orchestration =
        (RuntimeTickOrchestrationContext *)context;

    if (!orchestration || !orchestration->state)
        return;

    runtime_gameplay_service_tick(orchestration->state->world,
                                  orchestration->state->controllers,
                                  orchestration->state->controller_count,
                                  orchestration->state->player_id,
                                  &orchestration->state->pbj_pickup_collected,
                                  6.0f,
                                  1.55f);
}

void runtime_tick_orchestration_render_scene(void *context)
{
    RuntimeTickOrchestrationContext *orchestration =
        (RuntimeTickOrchestrationContext *)context;

    if (!orchestration || !orchestration->state || !orchestration->service_ports ||
        !orchestration->service_ports->render.submit_scene)
        return;

    orchestration->service_ports->render.submit_scene(orchestration->state);
}
