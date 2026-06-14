#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_METRICS_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_METRICS_H

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
        RUNTIME_NETCODE_SPECTRAL_RADIUS_FLOOR_APPLIED = 1,
        RUNTIME_NETCODE_SPECTRAL_DISABLED = 2
    } RuntimeNetcodeSpectralState;

    typedef enum RuntimeNetcodeEndiannessDecodePath
    {
        RUNTIME_NETCODE_ENDIANNESS_LITTLE_ENDIAN = 0,
        RUNTIME_NETCODE_ENDIANNESS_BYTE_SWAPPED = 1
    } RuntimeNetcodeEndiannessDecodePath;

    typedef enum RuntimeNetcodeK3h4AssignmentFamily
    {
        RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE = 0,
        RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER = 1
    } RuntimeNetcodeK3h4AssignmentFamily;

    typedef enum RuntimeNetcodeK3h4SpectralMode
    {
        RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED = 0,
        RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH = 1
    } RuntimeNetcodeK3h4SpectralMode;

    typedef struct RuntimeNetcodeK3h4Center
    {
        int cluster_id;
        int member_count;
        int center_q16[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    } RuntimeNetcodeK3h4Center;

    typedef struct RuntimeNetcodeK3h4Radius
    {
        int cluster_id;
        int nearest_neighbor_distance_q16;
        int inscribed_radius_q16;
        int radius_state;
    } RuntimeNetcodeK3h4Radius;

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

    typedef struct RuntimeNetcodeK3h4Observability
    {
        int convergence_status;
        int iteration_count;
        int assignment_changes_last_iteration;
        int deterministic_hash;
        int endianness_decode_path;
    } RuntimeNetcodeK3h4Observability;

    typedef struct RuntimeNetcodeEnvelopeMetadata
    {
        int contract_version;
        int byte_order_tag;
        int payload_bytes;
        int payload_crc32;
        int contract_status;
    } RuntimeNetcodeEnvelopeMetadata;

    typedef struct RuntimeNetcodeK3h4Output
    {
        int dimensions;
        RuntimeNetcodeProjectionNode nodes[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int alignment;
        int radial_stability;
        int cluster_count;
        int vector_count;
        RuntimeNetcodeK3h4Center centers[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        RuntimeNetcodeK3h4Radius radii[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        RuntimeNetcodeWeightedVoronoiScore weighted_voronoi_scores[
            RUNTIME_NETCODE_VECTOR_NODE_COUNT * RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        RuntimeNetcodeSpectralProxy spectral_proxy[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        RuntimeNetcodeK3h4Observability observability;
        RuntimeNetcodeEnvelopeMetadata envelope;
    } RuntimeNetcodeK3h4Output;

    int runtime_netcode_k3h4_build(const RuntimeNetcodeVectorOutput *input,
                                          RuntimeNetcodeK3h4Output *out_output);

    int runtime_netcode_k3h4_build_with_config(
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output,
        int assignment_family,
        int spectral_mode);

#ifdef __cplusplus
}
#endif

#endif
