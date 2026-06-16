#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_METRICS_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_METRICS_H

#include "../vector/netcode_vector.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Projected notebook node coordinates plus local geometry metadata. */
    typedef struct RuntimeNetcodeProjectionNode
    {
        float x;
        float y;
        float z;
        int coherence;
        float inradius;
        float nearest_neighbor_distance;
    } RuntimeNetcodeProjectionNode;

    /* Radius-state flags consumed by API and frontend normalization layers. */
    typedef enum RuntimeNetcodeRadiusState
    {
        RUNTIME_NETCODE_RADIUS_OK = 0,
        RUNTIME_NETCODE_RADIUS_SINGLE_CLUSTER = 1,
        RUNTIME_NETCODE_RADIUS_NEAR_ZERO_CLAMPED = 2
    } RuntimeNetcodeRadiusState;

    /* Indicates whether a weighted Voronoi score is safe to present as-is. */
    typedef enum RuntimeNetcodeScoreValidity
    {
        RUNTIME_NETCODE_SCORE_VALID = 0,
        RUNTIME_NETCODE_SCORE_INVALID_RADIUS = 1
    } RuntimeNetcodeScoreValidity;

    /* Describes whether spectral proxy data is measured or degraded/disabled. */
    typedef enum RuntimeNetcodeSpectralState
    {
        RUNTIME_NETCODE_SPECTRAL_OK = 0,
        RUNTIME_NETCODE_SPECTRAL_RADIUS_FLOOR_APPLIED = 1,
        RUNTIME_NETCODE_SPECTRAL_DISABLED = 2
    } RuntimeNetcodeSpectralState;

    /* Records whether the payload was read directly or through byte swapping. */
    typedef enum RuntimeNetcodeEndiannessDecodePath
    {
        RUNTIME_NETCODE_ENDIANNESS_LITTLE_ENDIAN = 0,
        RUNTIME_NETCODE_ENDIANNESS_BYTE_SWAPPED = 1
    } RuntimeNetcodeEndiannessDecodePath;

    /* Assignment family toggles the cluster-membership scoring strategy. */
    typedef enum RuntimeNetcodeK3h4AssignmentFamily
    {
        RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE = 0,
        RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER = 1
    } RuntimeNetcodeK3h4AssignmentFamily;

    /* Spectral mode enables or suppresses affinity-graph proxy generation. */
    typedef enum RuntimeNetcodeK3h4SpectralMode
    {
        RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED = 0,
        RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH = 1
    } RuntimeNetcodeK3h4SpectralMode;

    /*
     * Fixed-point cluster center exported through the ABI payload.
     *
     * Each component uses Q16 encoding:
     *   center_real[dim] = center_q16[dim] / 65536.0
     */
    typedef struct RuntimeNetcodeK3h4Center
    {
        int cluster_id;
        int member_count;
        int center_q16[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    } RuntimeNetcodeK3h4Center;

    /*
     * Inradius and nearest-neighbor distance summary for one cluster.
     *
     * nearest_neighbor_distance_q16 is the closest center-to-center distance.
     * inscribed_radius_q16 is computed as:
     *   r_q16 = round(nearest_neighbor_distance_q16 / 2)
     * then clamped to a floor when the geometry would collapse.
     */
    typedef struct RuntimeNetcodeK3h4Radius
    {
        int cluster_id;
        int nearest_neighbor_distance_q16;
        int inscribed_radius_q16;
        int radius_state;
    } RuntimeNetcodeK3h4Radius;

    /*
     * Distance-weighted token-to-cluster score emitted for notebook comparisons.
     *
     * distance_to_center_q16 stores the Mahalanobis distance in Q16 form.
     * weighted_score_q16 then depends on the selected assignment family:
     *
     *   multiplicative mode: score = distance_q16 / radius_q16
     *   power mode:          score = distance_q16^2 - radius_q16^2
     *
     * Smaller multiplicative scores are better; more negative power scores are
     * more interior to the cluster boundary.
     */
    typedef struct RuntimeNetcodeWeightedVoronoiScore
    {
        int vector_id;
        int cluster_id;
        int distance_to_center_q16;
        int weighted_score_q16;
        int score_validity;
    } RuntimeNetcodeWeightedVoronoiScore;

    /*
     * Compact spectral proxy summary per cluster.
     *
     * When spectral mode is enabled:
     *   frequency_proxy_q16 ~= 1 / radius_q16
     *   amplitude_proxy_q16 = member_count / vector_count
     * both encoded back into Q16 for deterministic transport.
     */
    typedef struct RuntimeNetcodeSpectralProxy
    {
        int cluster_id;
        int frequency_proxy_q16;
        int amplitude_proxy_q16;
        int spectral_state;
    } RuntimeNetcodeSpectralProxy;

    /*
     * Observability fields used to explain clustering stability and decoding.
     * deterministic_hash is built from the exported Q16 centers, radii,
     * spectral values, and weighted scores so equal payloads hash identically.
     */
    typedef struct RuntimeNetcodeK3h4Observability
    {
        int convergence_status;
        int iteration_count;
        int assignment_changes_last_iteration;
        int deterministic_hash;
        int endianness_decode_path;
    } RuntimeNetcodeK3h4Observability;

    /* Envelope metadata appended after the payload body for validation. */
    typedef struct RuntimeNetcodeEnvelopeMetadata
    {
        int contract_version;
        int byte_order_tag;
        int payload_bytes;
        int payload_crc32;
        int contract_status;
    } RuntimeNetcodeEnvelopeMetadata;

    /*
     * Full k3h4 payload that crosses the native ABI boundary.
     *
     * Float fields are reserved for presentation-friendly geometry, while the
     * quantitative clustering contract rides mostly in Q16 fields so the API
     * and frontend can reconstruct the same ordering and thresholds exactly.
     */
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

    /*
     * Builds the default k3h4 payload using environment-selected pipeline config.
     * This path preserves the same Q16 geometry/radius/score math as the
     * explicit variant, but resolves assignment and spectral mode from env.
     */
    int runtime_netcode_k3h4_build(const RuntimeNetcodeVectorOutput *input,
                                          RuntimeNetcodeK3h4Output *out_output);

    /*
     * Builds the k3h4 payload with explicit assignment and spectral settings.
     *
     * assignment_family selects the score equation applied to each
     * vector/cluster pair, while spectral_mode controls whether the radius-
     * derived affinity proxy is materialized.
     */
    int runtime_netcode_k3h4_build_with_config(
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output,
        int assignment_family,
        int spectral_mode);

#ifdef __cplusplus
}
#endif

#endif
