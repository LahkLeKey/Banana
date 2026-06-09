#ifndef BANANA_ENGINE_RUNTIME_ENGINE_EXPORTS_CONTEXT_H
#define BANANA_ENGINE_RUNTIME_ENGINE_EXPORTS_CONTEXT_H

#include "../state/engine_state.h"
#include "../support/engine_aux_abi.h"
#include "../application/service/application_service_ports.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern EngineRuntimeState s_engine_state;
extern const RuntimeApplicationServicePorts *s_service_ports;

RuntimeEngineAuxContext runtime_engine_exports_aux_context(void);

#ifdef __cplusplus
}
#endif

#endif
