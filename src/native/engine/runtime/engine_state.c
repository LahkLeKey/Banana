#include "engine_state.h"

#include <string.h>

void runtime_engine_state_reset(EngineRuntimeState *state)
{
    if (!state)
        return;

    memset(state, 0, sizeof(*state));
}
