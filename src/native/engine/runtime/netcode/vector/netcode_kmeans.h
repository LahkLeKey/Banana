#ifndef BANANA_ENGINE_RUNTIME_NETCODE_KMEANS_H
#define BANANA_ENGINE_RUNTIME_NETCODE_KMEANS_H

#include "netcode_vector.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum RuntimeNetcodeKmeansConvergenceStatus
    {
        RUNTIME_NETCODE_KMEANS_CONVERGED = 0,
        RUNTIME_NETCODE_KMEANS_MAX_ITERATIONS_EXCEEDED = 1,
        RUNTIME_NETCODE_KMEANS_INVALID_INPUT = 2
    } RuntimeNetcodeKmeansConvergenceStatus;

    typedef struct RuntimeNetcodeKmeansResult
    {
        int cluster_count;
        int vector_count;
        int iteration_count;
        int convergence_status;
        int assignment_changes_last_iteration;
        int assignments[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int member_counts[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int centers_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    } RuntimeNetcodeKmeansResult;

    int runtime_netcode_kmeans_compute(
        const float vectors[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
        int vector_count,
        int dimensions,
        int cluster_count,
        int max_iterations,
        int convergence_threshold_q16,
        RuntimeNetcodeKmeansResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
