#include "netcode_k3h4.h"

#include "netcode_fixed_point.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#if defined(BANANA_ENGINE_HAS_CBLAS)
#include <cblas.h>
#endif

typedef enum RuntimeNetcodeK3h4Backend
{
    RUNTIME_NETCODE_K3H4_BACKEND_SCALAR = 0,
    RUNTIME_NETCODE_K3H4_BACKEND_BLAS = 1
} RuntimeNetcodeK3h4Backend;

/* Clamp helper for cluster count and iteration-related bounds. */
static int clamp(int value, int min_value, int max_value)
{
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

static void float_vectors_to_q16(
    const float vectors[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
    int vector_count,
    int dimensions,
    int out_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS])
{
    /* Every vector component is moved into Q16 once up front so later passes
     * can work in a deterministic fixed-point domain.
     */
    int vector_index;
    int dim;

    for (vector_index = 0; vector_index < vector_count; vector_index++)
    {
        for (dim = 0; dim < dimensions; dim++)
        {
            out_q16[vector_index][dim] = runtime_netcode_q16_from_float(vectors[vector_index][dim]);
        }
    }
}

static int64_t squared_distance_q16(
    const int *lhs_q16,
    const int *rhs_q16,
    int dimensions)
{
    /* Returns sum_k (lhs[k] - rhs[k])^2 with all operands already in Q16. */
    int dim;
    int64_t sum = 0;

    for (dim = 0; dim < dimensions; dim++)
    {
        int64_t delta = (int64_t)lhs_q16[dim] - (int64_t)rhs_q16[dim];
        sum += delta * delta;
    }

    return sum;
}

#if defined(BANANA_ENGINE_HAS_CBLAS)
static int64_t squared_distance_q16_blas(
    const int *lhs_q16,
    const int *rhs_q16,
    int dimensions)
{
    float delta[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    int dim;
    float distance;

    for (dim = 0; dim < dimensions; dim++)
    {
        delta[dim] = ((float)lhs_q16[dim] - (float)rhs_q16[dim]) / (float)NETCODE_Q16_ONE;
    }

    distance = cblas_sdot(dimensions, delta, 1, delta, 1);
    return (int64_t)(distance * (float)NETCODE_Q16_ONE * (float)NETCODE_Q16_ONE + 0.5f);
}
#endif

static RuntimeNetcodeK3h4Backend resolve_backend(void)
{
    /* BLAS changes the implementation strategy, not the mathematical contract. */
    const char *backend_env = getenv("BANANA_NETCODE_K3H4_BACKEND");
    if (!backend_env || backend_env[0] == '\0' || strcmp(backend_env, "auto") == 0)
    {
#if defined(BANANA_ENGINE_HAS_CBLAS)
        return RUNTIME_NETCODE_K3H4_BACKEND_BLAS;
#else
        return RUNTIME_NETCODE_K3H4_BACKEND_SCALAR;
#endif
    }

    if (strcmp(backend_env, "blas") == 0)
    {
#if defined(BANANA_ENGINE_HAS_CBLAS)
        return RUNTIME_NETCODE_K3H4_BACKEND_BLAS;
#else
        return RUNTIME_NETCODE_K3H4_BACKEND_SCALAR;
#endif
    }

    return RUNTIME_NETCODE_K3H4_BACKEND_SCALAR;
}

static int64_t distance_for_backend(
    RuntimeNetcodeK3h4Backend backend,
    const int *lhs_q16,
    const int *rhs_q16,
    int dimensions)
{
#if defined(BANANA_ENGINE_HAS_CBLAS)
    if (backend == RUNTIME_NETCODE_K3H4_BACKEND_BLAS)
    {
        return squared_distance_q16_blas(lhs_q16, rhs_q16, dimensions);
    }
#else
    (void)backend;
#endif

    return squared_distance_q16(lhs_q16, rhs_q16, dimensions);
}

static int choose_cluster(
    const int vector_q16[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
    RuntimeNetcodeK3h4Backend backend,
    int cluster_count,
    int dimensions,
    const int centers_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS])
{
    /* Argmin over squared distances in fixed-point space. */
    int cluster_index;
    int selected_cluster = 0;
    int64_t selected_distance = LLONG_MAX;

    for (cluster_index = 0; cluster_index < cluster_count; cluster_index++)
    {
        int64_t distance = distance_for_backend(backend, vector_q16, centers_q16[cluster_index], dimensions);
        if (distance < selected_distance)
        {
            selected_distance = distance;
            selected_cluster = cluster_index;
        }
    }

    return selected_cluster;
}

int runtime_netcode_k3h4_compute(
    const float vectors[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
    int vector_count,
    int dimensions,
    int cluster_count,
    int max_iterations,
    int convergence_threshold_q16,
    RuntimeNetcodeK3h4Result *out_result)
{
    int vectors_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS] = {{0}};
    int centers_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS] = {{0}};
    int updated_centers_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS] = {{0}};
    int assignments[RUNTIME_NETCODE_VECTOR_NODE_COUNT] = {0};
    int member_counts[RUNTIME_NETCODE_VECTOR_NODE_COUNT] = {0};
    RuntimeNetcodeK3h4Backend backend;
    int iteration;
    int vector_index;
    int cluster_index;
    int dim;

    if (!vectors || !out_result || dimensions <= 0 || dimensions > RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS ||
        vector_count <= 0 || vector_count > RUNTIME_NETCODE_VECTOR_NODE_COUNT)
    {
        if (out_result)
        {
            memset(out_result, 0, sizeof(*out_result));
            out_result->convergence_status = RUNTIME_NETCODE_K3H4_INVALID_INPUT;
        }
        return -1;
    }

    cluster_count = clamp(cluster_count, 1, vector_count);
    max_iterations = max_iterations <= 0 ? 32 : max_iterations;
    convergence_threshold_q16 = convergence_threshold_q16 <= 0 ? 8 : convergence_threshold_q16;
    backend = resolve_backend();

    float_vectors_to_q16(vectors, vector_count, dimensions, vectors_q16);

    for (cluster_index = 0; cluster_index < cluster_count; cluster_index++)
    {
        for (dim = 0; dim < dimensions; dim++)
        {
            centers_q16[cluster_index][dim] = vectors_q16[cluster_index][dim];
        }
    }

    memset(out_result, 0, sizeof(*out_result));
    out_result->cluster_count = cluster_count;
    out_result->vector_count = vector_count;
    out_result->convergence_status = RUNTIME_NETCODE_K3H4_MAX_ITERATIONS_EXCEEDED;

    for (iteration = 0; iteration < max_iterations; iteration++)
    {
        int assignment_changes = 0;
        int64_t sums[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS] = {{0}};
        int max_center_shift = 0;

        /* Step 1: assign each vector to its nearest current center. */
        memset(member_counts, 0, sizeof(member_counts));

        for (vector_index = 0; vector_index < vector_count; vector_index++)
        {
            int chosen_cluster = choose_cluster(vectors_q16[vector_index], backend, cluster_count, dimensions, centers_q16);
            if (iteration == 0 || assignments[vector_index] != chosen_cluster)
            {
                assignment_changes += 1;
            }
            assignments[vector_index] = chosen_cluster;
            member_counts[chosen_cluster] += 1;

            for (dim = 0; dim < dimensions; dim++)
            {
                sums[chosen_cluster][dim] += vectors_q16[vector_index][dim];
            }
        }

        for (cluster_index = 0; cluster_index < cluster_count; cluster_index++)
        {
            if (member_counts[cluster_index] == 0)
            {
                for (dim = 0; dim < dimensions; dim++)
                {
                    updated_centers_q16[cluster_index][dim] = centers_q16[cluster_index][dim];
                }
                continue;
            }

            /* Step 2: recompute each center as the component-wise mean in Q16. */
            for (dim = 0; dim < dimensions; dim++)
            {
                updated_centers_q16[cluster_index][dim] =
                    runtime_netcode_q16_round_div(sums[cluster_index][dim], member_counts[cluster_index]);
            }
        }

        /* Step 3: track the largest per-component center movement in Q16. */
        for (cluster_index = 0; cluster_index < cluster_count; cluster_index++)
        {
            for (dim = 0; dim < dimensions; dim++)
            {
                int shift = updated_centers_q16[cluster_index][dim] - centers_q16[cluster_index][dim];
                if (shift < 0) shift = -shift;
                if (shift > max_center_shift) max_center_shift = shift;
                centers_q16[cluster_index][dim] = updated_centers_q16[cluster_index][dim];
            }
        }

        out_result->iteration_count = iteration + 1;
        out_result->assignment_changes_last_iteration = assignment_changes;

        if (assignment_changes == 0 || max_center_shift <= convergence_threshold_q16)
        {
            out_result->convergence_status = RUNTIME_NETCODE_K3H4_CONVERGED;
            break;
        }
    }

    for (vector_index = 0; vector_index < vector_count; vector_index++)
    {
        out_result->assignments[vector_index] = assignments[vector_index];
    }
    for (cluster_index = 0; cluster_index < cluster_count; cluster_index++)
    {
        out_result->member_counts[cluster_index] = member_counts[cluster_index];
        for (dim = 0; dim < dimensions; dim++)
        {
            out_result->centers_q16[cluster_index][dim] = centers_q16[cluster_index][dim];
        }
    }

    return 0;
}
