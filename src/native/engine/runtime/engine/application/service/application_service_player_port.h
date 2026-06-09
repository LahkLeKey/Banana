#ifndef BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_PLAYER_PORT_H
#define BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_PLAYER_PORT_H

#include "../../engine_state.h"
#include "../../../player/player_motion.h"

#ifdef __cplusplus
extern "C"
{
#endif

void runtime_application_player_update_motion_port(EngineRuntimeState *state,
                                                   float dt,
                                                   RuntimeTerrainSampleHeightFn sample_height);
void runtime_application_player_follow_camera_port(EngineRuntimeState *state);
void runtime_application_player_apply_click_input_port(EngineRuntimeState *state,
                                                       float normalized_x,
                                                       float normalized_y,
                                                       RuntimeTerrainSampleHeightFn sample_height);

#ifdef __cplusplus
}
#endif

#endif