#ifndef BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_RENDER_PORT_H
#define BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_RENDER_PORT_H

#include "../../state/engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

void runtime_application_render_submit_scene_port(EngineRuntimeState *state);
void runtime_application_render_ports_reset(void);

#ifdef __cplusplus
}
#endif

#endif