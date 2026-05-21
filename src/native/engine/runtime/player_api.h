#ifndef BANANA_ENGINE_RUNTIME_PLAYER_API_H
#define BANANA_ENGINE_RUNTIME_PLAYER_API_H

#include "player_registry.h"

#ifdef __cplusplus
extern "C"
{
#endif

    uint32_t runtime_player_api_upsert(World *world,
                                       const char *player_guid,
                                       const char *player_name,
                                       const char *controller_kind,
                                       int active,
                                       RuntimeTerrainSampleFn terrain_sample_height,
                                       RuntimeControllerAttachFn attach_controller,
                                       EntityId *inout_primary_player_id);

    void runtime_player_api_apply_input(const char *player_guid,
                                        float input_x,
                                        float input_z);

    void runtime_player_api_set_transform(World *world,
                                          const char *player_guid,
                                          float x,
                                          float y,
                                          float z,
                                          int active,
                                          float island_span,
                                          RuntimeTerrainSampleFn terrain_sample_height);

#ifdef __cplusplus
}
#endif

#endif
