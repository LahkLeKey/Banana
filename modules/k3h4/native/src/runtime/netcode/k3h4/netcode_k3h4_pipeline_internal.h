#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_INTERNAL_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_INTERNAL_H

#include "netcode_k3h4_pipeline.h"

/*
 * Scratch context shared across the k3h4 pipeline stages.
 *
 * The pipeline keeps both real-valued normalized vectors and their Q16 copies:
 * - normalized      drives display geometry and covariance estimation
 * - normalized_q16  preserves deterministic center/radius/score calculations
 *
 * cluster_inverse_covariance[i] stores Sigma_i^{-1} for Mahalanobis scoring.
 * radius_floor_q16, assignment_family, and spectral_enabled capture the active
 * runtime configuration chosen by setup.
 */
typedef struct RuntimeNetcodeK3h4PipelineContext
{
    const RuntimeNetcodeVectorOutput *input;
    RuntimeNetcodeK3h4Output *output;
    float normalized[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    int normalized_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float centroid[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float centroid_normalized[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float cluster_inverse_covariance[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float total_coherence;
    float total_radius;
    float radius_variance;
    int cluster_count;
    int dimensions;
    int radius_floor_q16;
    int assignment_family;
    int spectral_enabled;
} RuntimeNetcodeK3h4PipelineContext;

#endif
