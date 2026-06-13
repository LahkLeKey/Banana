#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_CLUSTER_SCORING_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_CLUSTER_SCORING_H

#include "netcode_k3h4_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_netcode_k3h4_build_cluster_models(
        RuntimeNetcodeK3h4PipelineContext *context);

    void runtime_netcode_k3h4_build_cluster_radii_and_spectral(
        RuntimeNetcodeK3h4PipelineContext *context);

    void runtime_netcode_k3h4_build_weighted_voronoi_scores(
        RuntimeNetcodeK3h4PipelineContext *context);

    int runtime_netcode_k3h4_build_deterministic_hash(
        const RuntimeNetcodeK3h4Output *output);

#ifdef __cplusplus
}
#endif

#endif
