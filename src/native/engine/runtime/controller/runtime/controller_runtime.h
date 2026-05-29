#ifndef BANANA_ENGINE_RUNTIME_CONTROLLER_RUNTIME_H
#define BANANA_ENGINE_RUNTIME_CONTROLLER_RUNTIME_H

#include "../../../ai/controller.h"

#ifdef __cplusplus
extern "C"
{
#endif

    ControllerInstance *runtime_controller_find_by_id(ControllerInstance **controllers,
                                                      int controller_count,
                                                      uint32_t controller_id);

    int runtime_controller_update_by_id(ControllerInstance **controllers,
                                        int controller_count,
                                        uint32_t controller_id,
                                        float dt);

    int runtime_controller_signal_by_id(ControllerInstance **controllers,
                                        int controller_count,
                                        uint32_t controller_id,
                                        const char *signal_name,
                                        const void *data);

    int runtime_controller_assign_team_by_id(ControllerInstance **controllers,
                                             int controller_count,
                                             uint32_t controller_id,
                                             ControllerTeam team);

    int runtime_controller_signal_team_war(ControllerInstance **controllers,
                                           int controller_count,
                                           float trigger_radius,
                                           const char *signal_name);

#ifdef __cplusplus
}
#endif

#endif
