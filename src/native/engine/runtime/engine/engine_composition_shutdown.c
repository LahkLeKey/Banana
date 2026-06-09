#include "engine_composition_shutdown.h"

#include "application/service/application_service_ports.h"
#include "engine_lifecycle.h"

#include "../../ai/npc_merchant.h"
#include "../../ui/ui.h"
#include "../../engine_serialize.h"
#include "../input/contract/input_contract.h"
#include "../player/player_builds.h"
#include "../player/player_registry.h"

#include <stdio.h>

void runtime_engine_composition_shutdown_state(EngineRuntimeState *state)
{
    if (!state)
        return;

    runtime_engine_lifecycle_destroy_controllers(state->controllers,
                                                 BANANA_ENGINE_MAX_ACTIVE_CONTROLLERS,
                                                 &state->controller_count);

    mesh_destroy(state->entity_mesh);
    mesh_destroy(state->pbj_pickup_mesh);
    mesh_destroy(state->banana_team_mesh);
    mesh_destroy(state->bean_team_mesh);
    mesh_destroy(state->landmark_mesh);
    mesh_destroy(state->traversal_mesh);
    runtime_engine_lifecycle_reset_terrain_chunks(state->terrain_chunks,
                                                  BANANA_ENGINE_TERRAIN_TOTAL_CHUNKS);
    world_destroy(state->world);
    physics_world_destroy(state->physics);
    renderer_destroy(state->renderer);
    window_destroy(state->window);

    if (state->merchants_seeded)
        npc_merchant_shutdown();

    player_builds_cleanup();
    runtime_player_registry_reset();
    runtime_input_contract_reset();
    runtime_application_service_ports_reset();
    engine_serialize_reset();
    runtime_engine_state_reset(state);

    fprintf(stdout, "[engine] shutdown complete\n");
}