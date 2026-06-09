#ifndef BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_RENDER_ACTOR_RESOLVER_H
#define BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_RENDER_ACTOR_RESOLVER_H

#include "../../engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

void runtime_application_render_actor_resolver_set_state(EngineRuntimeState *state);
Mesh *runtime_application_render_mesh_for_actor(const Entity *entity, Mesh *default_mesh);
void runtime_application_render_command_for_actor(const Entity *entity,
                                                  Mesh *resolved_mesh,
                                                  Material resolved_material,
                                                  RendererDrawCommand *out_command);

#ifdef __cplusplus
}
#endif

#endif