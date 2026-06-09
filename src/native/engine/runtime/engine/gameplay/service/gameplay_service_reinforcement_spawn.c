#include "gameplay_service_reinforcement_spawn.h"

#include "gameplay_service_models.h"
#include "gameplay_service_reinforcement_metrics.h"
#include "gameplay_service_sentience_behavior.h"

#include "../../../controller/attach/controller_attach.h"

#include <string.h>

int runtime_gameplay_spawn_war_reinforcement(World *world,
                                             EngineRuntimeState *runtime_state,
                                             ControllerInstance **controllers,
                                             int max_controllers,
                                             int *inout_controller_count,
                                             ControllerTeam team,
                                             float anchor_x,
                                             float anchor_y,
                                             float anchor_z,
                                             float direction_x,
                                             float direction_z,
                                             float lateral_sign,
                                             int ordinal,
                                             RuntimeWarSentienceBehaviorMode behavior_mode,
                                             int war_escalation_tier,
                                             int war_intelligence_stage,
                                             int war_biome_stage_index)
{
    int biome_index = runtime_gameplay_clamp_biome_index(war_biome_stage_index);
    RuntimeWarReinforcementFamily family =
        runtime_gameplay_reinforcement_family_for_team(team,
                                                       war_escalation_tier,
                                                       war_intelligence_stage);
    float forward = 0.0f;
    float lateral = 0.0f;
    float oriented_direction_x = direction_x;
    float oriented_direction_z = direction_z;
    float x = anchor_x + (direction_x * forward) + ((-direction_z) * lateral);
    float z = anchor_z + (direction_z * forward) + (direction_x * lateral);
    float y = anchor_y + 0.35f;
    const char *controller_type = (team == CONTROLLER_TEAM_BANANA) ? "combat" : "wildlife";
    const char *gameplay_model_id = NULL;
    EntityId entity_id = 0;
    Entity *entity = NULL;
    uint32_t controller_id = 0;

    if (!world || !controllers || !inout_controller_count || max_controllers <= 0)
        return 0;

    runtime_gameplay_sentience_behavior_spawn_offsets(behavior_mode,
                                                      ordinal,
                                                      &forward,
                                                      &lateral);
    runtime_gameplay_sentience_behavior_apply_directionality(behavior_mode,
                                                             lateral_sign,
                                                             &oriented_direction_x,
                                                             &oriented_direction_z);
    lateral *= lateral_sign;

    x = anchor_x + (oriented_direction_x * forward) + ((-oriented_direction_z) * lateral);
    z = anchor_z + (oriented_direction_z * forward) + (oriented_direction_x * lateral);

    if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE)
    {
        gameplay_model_id = (team == CONTROLLER_TEAM_BEAN)
                                ? "gameplay/reference/banana-bean-green-v1"
                                : "gameplay/reference/banana-basic-v1";
    }
    else
    {
        gameplay_model_id = runtime_gameplay_reinforcement_model_id_for_family(family,
                                                                                behavior_mode,
                                                                                war_intelligence_stage,
                                                                                biome_index);
    }

    if (*inout_controller_count >= max_controllers)
        return 0;

    entity_id = world_spawn_entity(world, ENTITY_TYPE_NPC, x, y, z);
    if (!entity_id)
        return 0;

    entity = world_get_entity(world, entity_id);
    if (!entity)
        return 0;

    strncpy(entity->controller_kind, controller_type, sizeof(entity->controller_kind) - 1);
    entity->controller_kind[sizeof(entity->controller_kind) - 1] = '\0';
    strncpy(entity->gameplay_model_id, gameplay_model_id, sizeof(entity->gameplay_model_id) - 1);
    entity->gameplay_model_id[sizeof(entity->gameplay_model_id) - 1] = '\0';

    controller_id = runtime_controller_attach_to_entity_with_team(world,
                                                                  controllers,
                                                                  max_controllers,
                                                                  inout_controller_count,
                                                                  entity_id,
                                                                  controller_type,
                                                                  team);
    if (!controller_id)
    {
        world_despawn_entity(world, entity_id);
        return 0;
    }

    entity->controller_id = controller_id;
    runtime_gameplay_record_war_reinforcement_spawn(runtime_state,
                                                    team,
                                                    family,
                                                    behavior_mode,
                                                    war_intelligence_stage,
                                                    biome_index);
    return 1;
}