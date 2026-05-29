#ifndef BANANA_ENGINE_RUNTIME_CONTROLLER_ATTACH_H
#define BANANA_ENGINE_RUNTIME_CONTROLLER_ATTACH_H

#include "../../../ai/controller.h"
#include "../../../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    uint32_t runtime_controller_create_and_register(ControllerInstance **controllers,
                                                    int max_controllers,
                                                    int *inout_controller_count,
                                                    const char *type,
                                                    float x,
                                                    float y,
                                                    float z);

    uint32_t runtime_controller_create_and_register_with_team(ControllerInstance **controllers,
                                                              int max_controllers,
                                                              int *inout_controller_count,
                                                              const char *type,
                                                              float x,
                                                              float y,
                                                              float z,
                                                              ControllerTeam team);

    uint32_t runtime_controller_attach_to_entity(World *world,
                                                 ControllerInstance **controllers,
                                                 int max_controllers,
                                                 int *inout_controller_count,
                                                 uint32_t entity_id,
                                                 const char *type);

    uint32_t runtime_controller_attach_to_entity_with_team(World *world,
                                                           ControllerInstance **controllers,
                                                           int max_controllers,
                                                           int *inout_controller_count,
                                                           uint32_t entity_id,
                                                           const char *type,
                                                           ControllerTeam team);

#ifdef __cplusplus
}
#endif

#endif
