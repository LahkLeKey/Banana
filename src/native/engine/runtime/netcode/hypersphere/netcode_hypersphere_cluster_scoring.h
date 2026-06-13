#ifndef BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_CLUSTER_SCORING_H
#define BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_CLUSTER_SCORING_H

#include "netcode_hypersphere_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_netcode_hypersphere_build_cluster_models(
        RuntimeNetcodeHyperspherePipelineContext *context);

    void runtime_netcode_hypersphere_build_cluster_radii_and_spectral(
        RuntimeNetcodeHyperspherePipelineContext *context);

    void runtime_netcode_hypersphere_build_weighted_voronoi_scores(
        RuntimeNetcodeHyperspherePipelineContext *context);

    int runtime_netcode_hypersphere_build_deterministic_hash(
        const RuntimeNetcodeHypersphereOutput *output);

#ifdef __cplusplus
}
#endif

#endif
