#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_CLUSTER_SCORING_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_CLUSTER_SCORING_H

#include "netcode_k3h4_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Copies cluster centers/member counts into the payload and prepares the
     * inverse covariance matrices used by Mahalanobis distance scoring.
     */
    void runtime_netcode_k3h4_build_cluster_models(
        RuntimeNetcodeK3h4PipelineContext *context);

    /*
     * Computes center-to-center distances and derives:
     *   inscribed_radius_q16 = max(radius_floor_q16, nearest_neighbor_q16 / 2)
     * plus the optional spectral proxies built from those radii.
     */
    void runtime_netcode_k3h4_build_cluster_radii_and_spectral(
        RuntimeNetcodeK3h4PipelineContext *context);

    /*
     * Emits one score per vector/cluster pair using Mahalanobis distance.
     * The stored Q16 score is either distance/radius or distance^2 - radius^2,
     * depending on the active assignment family.
     */
    void runtime_netcode_k3h4_build_weighted_voronoi_scores(
        RuntimeNetcodeK3h4PipelineContext *context);

    /* Hashes the deterministic Q16-bearing sections of the finished payload. */
    int runtime_netcode_k3h4_build_deterministic_hash(
        const RuntimeNetcodeK3h4Output *output);

#ifdef __cplusplus
}
#endif

#endif
