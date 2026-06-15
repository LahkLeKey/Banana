#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_H

#include "netcode_vector.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Convergence outcome for the fixed-width native k3h4 clustering pass. */
    typedef enum RuntimeNetcodeK3h4ConvergenceStatus
    {
        RUNTIME_NETCODE_K3H4_CONVERGED = 0,
        RUNTIME_NETCODE_K3H4_MAX_ITERATIONS_EXCEEDED = 1,
        RUNTIME_NETCODE_K3H4_INVALID_INPUT = 2
    } RuntimeNetcodeK3h4ConvergenceStatus;

    /* Raw clustering result before geometry/scoring/envelope shaping. */
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
