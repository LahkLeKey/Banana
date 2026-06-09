#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_WARFRONT_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_WARFRONT_H

#include "gameplay_service.h"

#ifdef __cplusplus
extern "C"
{
#endif

int runtime_gameplay_count_active_war_npcs(const World *world);

int runtime_gameplay_count_team_controllers(ControllerInstance **controllers,
                                           int controller_count,
                                           ControllerTeam team);

int runtime_gameplay_find_active_warfront(ControllerInstance **controllers,
                                         int controller_count,
                                         float trigger_radius,
                                         ControllerInstance **out_banana,
                                         ControllerInstance **out_bean);

#ifdef __cplusplus
}
#endif

#endif