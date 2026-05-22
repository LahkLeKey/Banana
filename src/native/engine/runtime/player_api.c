#include "player_api.h"
#include "player_builds.h"

#include <string.h>

static BuildClass default_class_for_controller(const char *controller_kind)
{
    if (controller_kind && strcmp(controller_kind, "ai") == 0)
    {
        return BUILD_CLASS_RANGER;
    }
    return BUILD_CLASS_VANGUARD;
}

uint32_t runtime_player_api_upsert(World *world,
                                   const char *player_guid,
                                   const char *player_name,
                                   const char *controller_kind,
                                   int active,
                                   RuntimeTerrainSampleFn terrain_sample_height,
                                   RuntimeControllerAttachFn attach_controller,
                                   EntityId *inout_primary_player_id)
{
    NativePlayerBinding *binding = runtime_player_registry_upsert(world,
                                                                  player_guid,
                                                                  player_name,
                                                                  controller_kind,
                                                                  active,
                                                                  terrain_sample_height,
                                                                  attach_controller,
                                                                  inout_primary_player_id);
    if (binding)
    {
        (void)player_builds_upsert(player_guid, default_class_for_controller(controller_kind));
    }
    return binding ? binding->entity_id : 0;
}

void runtime_player_api_apply_input(const char *player_guid,
                                    float input_x,
                                    float input_z)
{
    runtime_player_registry_apply_input(player_guid, input_x, input_z);
}

void runtime_player_api_set_transform(World *world,
                                      const char *player_guid,
                                      float x,
                                      float y,
                                      float z,
                                      int active,
                                      float island_span,
                                      RuntimeTerrainSampleFn terrain_sample_height)
{
    runtime_player_registry_set_transform(world,
                                          player_guid,
                                          x,
                                          y,
                                          z,
                                          active,
                                          island_span,
                                          terrain_sample_height);
}
