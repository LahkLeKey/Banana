#ifndef BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_H
#define BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_H

#include "../vector/netcode_vector.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeNetcodeProjectionNode
    {
        float x;
        float y;
        float z;
        int coherence;
        float inradius;
        float nearest_neighbor_distance;
    } RuntimeNetcodeProjectionNode;

    typedef enum RuntimeNetcodeRadiusState
    {
        RUNTIME_NETCODE_RADIUS_OK = 0,
        RUNTIME_NETCODE_RADIUS_SINGLE_CLUSTER = 1,
        RUNTIME_NETCODE_RADIUS_NEAR_ZERO_CLAMPED = 2
    } RuntimeNetcodeRadiusState;

    typedef enum RuntimeNetcodeScoreValidity
    {
        RUNTIME_NETCODE_SCORE_VALID = 0,
        RUNTIME_NETCODE_SCORE_INVALID_RADIUS = 1
    } RuntimeNetcodeScoreValidity;

    typedef enum RuntimeNetcodeSpectralState
    {
        RUNTIME_NETCODE_SPECTRAL_OK = 0,
        RUNTIME_NETCODE_SPECTRAL_RADIUS_FLOOR_APPLIED = 1
    } RuntimeNetcodeSpectralState;

    typedef enum RuntimeNetcodeEndiannessDecodePath
    {
        RUNTIME_NETCODE_ENDIANNESS_LITTLE_ENDIAN = 0,
        RUNTIME_NETCODE_ENDIANNESS_BYTE_SWAPPED = 1
    } RuntimeNetcodeEndiannessDecodePath;

    typedef struct RuntimeNetcodeKmeansCenter
    {
        int cluster_id;
        int member_count;
        int center_q16[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    } RuntimeNetcodeKmeansCenter;

    typedef struct RuntimeNetcodeKmeansRadius
    {
        int cluster_id;
        int nearest_neighbor_distance_q16;
        int inscribed_radius_q16;
        int radius_state;
    } RuntimeNetcodeKmeansRadius;

    typedef struct RuntimeNetcodeWeightedVoronoiScore
    {
        int vector_id;
        int cluster_id;
        int distance_to_center_q16;
        int weighted_score_q16;
        int score_validity;
    } RuntimeNetcodeWeightedVoronoiScore;

    typedef struct RuntimeNetcodeSpectralProxy
    {
        int cluster_id;
        int frequency_proxy_q16;
        int amplitude_proxy_q16;
        int spectral_state;
    } RuntimeNetcodeSpectralProxy;

    typedef struct RuntimeNetcodeKmeansObservability
    {
        int convergence_status;
        int iteration_count;
        int assignment_changes_last_iteration;
        int deterministic_hash;
        int endianness_decode_path;
    } RuntimeNetcodeKmeansObservability;

    typedef struct RuntimeNetcodeHypersphereOutput
    {
        int dimensions;
        RuntimeNetcodeProjectionNode nodes[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int alignment;
        int radial_stability;
        int cluster_count;
        int vector_count;
        RuntimeNetcodeKmeansCenter centers[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        RuntimeNetcodeKmeansRadius radii[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        RuntimeNetcodeWeightedVoronoiScore weighted_voronoi_scores[
            RUNTIME_NETCODE_VECTOR_NODE_COUNT * RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        RuntimeNetcodeSpectralProxy spectral_proxy[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        RuntimeNetcodeKmeansObservability observability;
    } RuntimeNetcodeHypersphereOutput;

    int runtime_netcode_hypersphere_build(const RuntimeNetcodeVectorOutput *input,
                                          RuntimeNetcodeHypersphereOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
