#ifndef BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_GEOMETRY_H
#define BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_GEOMETRY_H

#include "netcode_k3h4_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_netcode_k3h4_prepare_geometry(
        RuntimeNetcodeHyperspherePipelineContext *context);

    void runtime_netcode_k3h4_build_projection_nodes(
        RuntimeNetcodeHyperspherePipelineContext *context);

    void runtime_netcode_k3h4_finalize_geometry(
        RuntimeNetcodeHyperspherePipelineContext *context);

#ifdef __cplusplus
}
#endif

#endif
