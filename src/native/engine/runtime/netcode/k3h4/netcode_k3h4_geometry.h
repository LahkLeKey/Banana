#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_GEOMETRY_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_GEOMETRY_H

#include "netcode_k3h4_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_netcode_k3h4_prepare_geometry(
        RuntimeNetcodeK3h4PipelineContext *context);

    void runtime_netcode_k3h4_build_projection_nodes(
        RuntimeNetcodeK3h4PipelineContext *context);

    void runtime_netcode_k3h4_finalize_geometry(
        RuntimeNetcodeK3h4PipelineContext *context);

#ifdef __cplusplus
}
#endif

#endif
