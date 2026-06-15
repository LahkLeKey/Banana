#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_OBSERVABILITY_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_OBSERVABILITY_H

#include "netcode_k3h4_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Finalizes the observability block after all geometry, radius, spectral,
     * and score fields are populated. This stage copies convergence metadata
     * from the upstream vector contract and computes the deterministic hash over
     * the finished k3h4 payload.
     */
    void runtime_netcode_k3h4_finalize_observability(
        RuntimeNetcodeK3h4PipelineContext *context);

#ifdef __cplusplus
}
#endif

#endif
