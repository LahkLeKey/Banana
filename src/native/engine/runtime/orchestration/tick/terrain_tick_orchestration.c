#include "runtime_tick_orchestration.h"

int runtime_tick_orchestration_rebuild_dirty_chunks(void *context, int max_chunks)
{
    RuntimeTickOrchestrationContext *orchestration =
        (RuntimeTickOrchestrationContext *)context;

    if (!orchestration || !orchestration->state || !orchestration->service_ports ||
        !orchestration->service_ports->terrain.rebuild_dirty_chunks)
        return 0;

    return orchestration->service_ports->terrain.rebuild_dirty_chunks(orchestration->state,
                                                                       max_chunks);
}
