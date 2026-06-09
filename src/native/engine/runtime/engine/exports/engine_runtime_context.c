#include "engine_runtime_context.h"

RuntimeEngineAuxContext runtime_engine_exports_aux_context(void)
{
    return runtime_engine_aux_context(s_engine_state.world,
                                      s_engine_state.controllers,
                                      s_engine_state.controller_count);
}
