#ifndef BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_PIPELINE_INTERNAL_H
#define BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_PIPELINE_INTERNAL_H

#include "netcode_hypersphere_pipeline.h"

typedef struct RuntimeNetcodeHyperspherePipelineContext
{
    const RuntimeNetcodeVectorOutput *input;
    RuntimeNetcodeHypersphereOutput *output;
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
} RuntimeNetcodeHyperspherePipelineContext;

#endif
