#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_H

#include "runtime/netcode/vector/netcode_vector.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * This stage runs the bounded clustering kernel that the later k3h4
     * pipeline decorates with geometry, radii, spectral proxies, and envelope
     * metadata.
     *
     * The kernel first converts each float component x into Q16:
     *   x_q16 = round(x * 2^16)
     * It then assigns each vector to the nearest center under squared distance
     * in that fixed-point space:
     *   d(i, j)^2 = sum_k (v_i[k]_q16 - c_j[k]_q16)^2
     *
     * After each assignment pass, centers are recomputed as the component-wise
     * arithmetic mean of their assigned members, still in Q16 units.
     */
    /* Convergence outcome for the fixed-width native k3h4 clustering pass. */
    typedef enum RuntimeNetcodeK3h4ConvergenceStatus
    {
        RUNTIME_NETCODE_K3H4_CONVERGED = 0,
        RUNTIME_NETCODE_K3H4_MAX_ITERATIONS_EXCEEDED = 1,
        RUNTIME_NETCODE_K3H4_INVALID_INPUT = 2
    } RuntimeNetcodeK3h4ConvergenceStatus;

    /*
     * Raw clustering result before geometry/scoring/envelope shaping.
     *
     * centers_q16 stores cluster centroids in fixed-point coordinates, so each
     * component c satisfies:
     *   c_real = centers_q16[cluster][dim] / 65536.0
     */
    typedef struct RuntimeNetcodeK3h4Result
    {
        int cluster_count;
        int vector_count;
        int iteration_count;
        int convergence_status;
        int assignment_changes_last_iteration;
        int assignments[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int member_counts[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int centers_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    } RuntimeNetcodeK3h4Result;

    /*
     * Runs the bounded native k3h4 clustering step over vector embeddings.
     * Later pipeline stages transform this result into projection, radius,
     * spectral, and observability sections of the exported payload.
     *
     * Convergence is checked with two guards:
     *   1. no assignment changed in the last iteration, or
     *   2. max_center_shift <= convergence_threshold_q16
     *
     * max_center_shift is an L-infinity bound in Q16 units:
     *   max_center_shift = max_{cluster, dim} |c_new_q16 - c_old_q16|
     *
     * A threshold of 8 therefore means 8 / 65536 ~= 1.22e-4 in real units.
     */
    int runtime_netcode_k3h4_compute(
        const float vectors[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
        int vector_count,
        int dimensions,
        int cluster_count,
        int max_iterations,
        int convergence_threshold_q16,
        RuntimeNetcodeK3h4Result *out_result);

#ifdef __cplusplus
}
#endif

#endif
