#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_GEOMETRY_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_GEOMETRY_H

#include "netcode_k3h4_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Normalizes node vectors, stores their Q16 mirrors, and computes the
     * centroid direction that later coherence calculations use.
     */
    void runtime_netcode_k3h4_prepare_geometry(
        RuntimeNetcodeK3h4PipelineContext *context);

    /*
     * Emits per-node projection coordinates, coherence, inradius, and
     * nearest-neighbor distance for notebook-facing geometry views.
     */
    void runtime_netcode_k3h4_build_projection_nodes(
        RuntimeNetcodeK3h4PipelineContext *context);

    /* Collapses accumulated node geometry into alignment and radial stability. */
    void runtime_netcode_k3h4_finalize_geometry(
        RuntimeNetcodeK3h4PipelineContext *context);

#ifdef __cplusplus
}
#endif

#endif
