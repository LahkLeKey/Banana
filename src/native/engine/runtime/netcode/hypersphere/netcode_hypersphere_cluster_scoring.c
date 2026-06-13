#include "netcode_hypersphere_cluster_scoring.h"

#include "../vector/netcode_fixed_point.h"

#include <math.h>

static int distance_q16_between_vectors(const int *lhs_q16, const int *rhs_q16, int dimensions)
{
    int d;
    double sum = 0.0;
    for (d = 0; d < dimensions; d++)
    {
        double delta = (double)lhs_q16[d] - (double)rhs_q16[d];
        sum += delta * delta;
    }
    return (int)runtime_netcode_q16_round_div((int64_t)sqrt(sum), 1);
}

static void set_identity_matrix(float matrix[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
                                int dimensions,
                                float diagonal_value)
{
    int row;
    int column;
    for (row = 0; row < dimensions; row++)
    {
        for (column = 0; column < dimensions; column++)
            matrix[row][column] = row == column ? diagonal_value : 0.0f;
    }
}

static int invert_matrix(const float input[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
                         int dimensions,
                         float output[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS])
{
    double augmented[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS * 2];
    int row;
    int column;

    for (row = 0; row < dimensions; row++)
    {
        for (column = 0; column < dimensions; column++)
        {
            augmented[row][column] = (double)input[row][column];
            augmented[row][column + dimensions] = row == column ? 1.0 : 0.0;
        }
    }

    for (column = 0; column < dimensions; column++)
    {
        int pivot_row = column;
        double pivot_abs = fabs(augmented[column][column]);

        for (row = column + 1; row < dimensions; row++)
        {
            double candidate_abs = fabs(augmented[row][column]);
            if (candidate_abs > pivot_abs)
            {
                pivot_abs = candidate_abs;
                pivot_row = row;
            }
        }

        if (pivot_abs <= 0.000000001)
            return -1;

        if (pivot_row != column)
        {
            int swap_column;
            for (swap_column = 0; swap_column < dimensions * 2; swap_column++)
            {
                double temp = augmented[column][swap_column];
                augmented[column][swap_column] = augmented[pivot_row][swap_column];
                augmented[pivot_row][swap_column] = temp;
            }
        }

        {
            double pivot = augmented[column][column];
            int normalize_column;
            for (normalize_column = 0; normalize_column < dimensions * 2; normalize_column++)
                augmented[column][normalize_column] /= pivot;
        }

        for (row = 0; row < dimensions; row++)
        {
            int reduce_column;
            double factor;
            if (row == column) continue;

            factor = augmented[row][column];
            if (fabs(factor) <= 0.000000001) continue;

            for (reduce_column = 0; reduce_column < dimensions * 2; reduce_column++)
                augmented[row][reduce_column] -= factor * augmented[column][reduce_column];
        }
    }

    for (row = 0; row < dimensions; row++)
    {
        for (column = 0; column < dimensions; column++)
            output[row][column] = (float)augmented[row][column + dimensions];
    }

    return 0;
}

static void build_cluster_inverse_covariance(
    const RuntimeNetcodeVectorOutput *input,
    const int normalized_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
    const RuntimeNetcodeHypersphereOutput *output,
    int cluster_id,
    int dimensions,
    float out_inverse_covariance[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS])
{
    float covariance[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float center[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float variance_sum = 0.0f;
    int member_count = 0;
    int node;
    int row;
    int column;
    const float regularization = 0.001f;

    set_identity_matrix(covariance, dimensions, 0.0f);

    for (row = 0; row < dimensions; row++)
        center[row] = runtime_netcode_q16_to_float(output->centers[cluster_id].center_q16[row]);

    for (node = 0; node < output->vector_count; node++)
    {
        if (input->k3h4_assignments[node] != cluster_id)
            continue;

        member_count += 1;
        for (row = 0; row < dimensions; row++)
        {
            float delta_row = runtime_netcode_q16_to_float(normalized_q16[node][row]) - center[row];
            for (column = 0; column < dimensions; column++)
            {
                float delta_column = runtime_netcode_q16_to_float(normalized_q16[node][column]) - center[column];
                covariance[row][column] += delta_row * delta_column;
            }
        }
    }

    if (member_count > 1)
    {
        float denominator = (float)(member_count - 1);
        for (row = 0; row < dimensions; row++)
        {
            for (column = 0; column < dimensions; column++)
                covariance[row][column] /= denominator;
        }
    }
    else
    {
        set_identity_matrix(covariance, dimensions, 0.25f);
    }

    for (row = 0; row < dimensions; row++)
        covariance[row][row] += regularization;

    if (invert_matrix(covariance, dimensions, out_inverse_covariance) == 0)
        return;

    for (row = 0; row < dimensions; row++)
        variance_sum += covariance[row][row];

    {
        float isotropic_variance = variance_sum / (float)dimensions;
        if (isotropic_variance < regularization)
            isotropic_variance = regularization;

        set_identity_matrix(covariance, dimensions, isotropic_variance);
        (void)invert_matrix(covariance, dimensions, out_inverse_covariance);
    }
}

static float mahalanobis_squared_between_vectors(
    const int *lhs_q16,
    const int *rhs_q16,
    const float inverse_covariance[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
    int dimensions)
{
    float delta[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float transformed[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float result = 0.0f;
    int row;
    int column;

    for (row = 0; row < dimensions; row++)
        delta[row] = runtime_netcode_q16_to_float(lhs_q16[row] - rhs_q16[row]);

    for (row = 0; row < dimensions; row++)
    {
        float sum = 0.0f;
        for (column = 0; column < dimensions; column++)
            sum += inverse_covariance[row][column] * delta[column];
        transformed[row] = sum;
    }

    for (row = 0; row < dimensions; row++)
        result += delta[row] * transformed[row];

    if (result < 0.0f)
        return 0.0f;

    return result;
}

static unsigned int hash_append_u32(unsigned int hash, unsigned int value)
{
    int byte_index;
    for (byte_index = 0; byte_index < 4; byte_index++)
    {
        unsigned int byte_value = (value >> (byte_index * 8)) & 0xFFu;
        hash ^= byte_value;
        hash *= 16777619u;
    }
    return hash;
}

void runtime_netcode_hypersphere_build_cluster_models(
    RuntimeNetcodeHyperspherePipelineContext *context)
{
    int i;
    int j;

    for (i = 0; i < context->cluster_count; i++)
    {
        context->output->centers[i].cluster_id = i;
        context->output->centers[i].member_count = context->input->k3h4_member_counts[i];

        for (j = 0; j < context->dimensions; j++)
            context->output->centers[i].center_q16[j] = context->input->k3h4_centers_q16[i][j];

        build_cluster_inverse_covariance(
            context->input,
            context->normalized_q16,
            context->output,
            i,
            context->dimensions,
            context->cluster_inverse_covariance[i]);
    }
}

void runtime_netcode_hypersphere_build_cluster_radii_and_spectral(
    RuntimeNetcodeHyperspherePipelineContext *context)
{
    int i;

    for (i = 0; i < context->cluster_count; i++)
    {
        int nearest_q16 = 0;
        int nearest_set = 0;
        int inscribed_q16 = 0;
        int radius_state = RUNTIME_NETCODE_RADIUS_OK;

        if (context->cluster_count <= 1)
        {
            radius_state = RUNTIME_NETCODE_RADIUS_SINGLE_CLUSTER;
            inscribed_q16 = context->radius_floor_q16;
            nearest_q16 = 0;
        }
        else
        {
            int j;
            for (j = 0; j < context->cluster_count; j++)
            {
                int candidate_q16;
                if (i == j) continue;

                candidate_q16 = distance_q16_between_vectors(
                    context->output->centers[i].center_q16,
                    context->output->centers[j].center_q16,
                    context->dimensions);
                if (!nearest_set || candidate_q16 < nearest_q16)
                {
                    nearest_q16 = candidate_q16;
                    nearest_set = 1;
                }
            }

            inscribed_q16 = runtime_netcode_q16_round_div(nearest_q16, 2);
            if (inscribed_q16 < context->radius_floor_q16)
            {
                inscribed_q16 = context->radius_floor_q16;
                radius_state = RUNTIME_NETCODE_RADIUS_NEAR_ZERO_CLAMPED;
            }
        }

        context->output->radii[i].cluster_id = i;
        context->output->radii[i].nearest_neighbor_distance_q16 = nearest_q16;
        context->output->radii[i].inscribed_radius_q16 = inscribed_q16;
        context->output->radii[i].radius_state = radius_state;

        context->output->spectral_proxy[i].cluster_id = i;
        context->output->spectral_proxy[i].frequency_proxy_q16 = runtime_netcode_q16_div(NETCODE_Q16_ONE, inscribed_q16);
        context->output->spectral_proxy[i].amplitude_proxy_q16 = runtime_netcode_q16_round_div(
            ((int64_t)context->output->centers[i].member_count) << NETCODE_Q16_SHIFT,
            context->output->vector_count);
        context->output->spectral_proxy[i].spectral_state =
            radius_state == RUNTIME_NETCODE_RADIUS_NEAR_ZERO_CLAMPED
                ? RUNTIME_NETCODE_SPECTRAL_RADIUS_FLOOR_APPLIED
                : RUNTIME_NETCODE_SPECTRAL_OK;
    }
}

void runtime_netcode_hypersphere_build_weighted_voronoi_scores(
    RuntimeNetcodeHyperspherePipelineContext *context)
{
    int i;

    for (i = 0; i < context->output->vector_count; i++)
    {
        int j;
        for (j = 0; j < context->cluster_count; j++)
        {
            int score_index = i * context->cluster_count + j;
            int mahalanobis_q16;
            float mahalanobis_squared;

            mahalanobis_squared = mahalanobis_squared_between_vectors(
                context->normalized_q16[i],
                context->output->centers[j].center_q16,
                context->cluster_inverse_covariance[j],
                context->dimensions);
            mahalanobis_q16 = runtime_netcode_q16_from_float(sqrtf(mahalanobis_squared));

            context->output->weighted_voronoi_scores[score_index].vector_id = i;
            context->output->weighted_voronoi_scores[score_index].cluster_id = j;
            context->output->weighted_voronoi_scores[score_index].distance_to_center_q16 = mahalanobis_q16;

            if (context->output->radii[j].radius_state == RUNTIME_NETCODE_RADIUS_SINGLE_CLUSTER)
            {
                context->output->weighted_voronoi_scores[score_index].weighted_score_q16 = 0;
                context->output->weighted_voronoi_scores[score_index].score_validity =
                    RUNTIME_NETCODE_SCORE_INVALID_RADIUS;
            }
            else
            {
                context->output->weighted_voronoi_scores[score_index].weighted_score_q16 =
                    runtime_netcode_q16_from_float(mahalanobis_squared);
                context->output->weighted_voronoi_scores[score_index].score_validity =
                    RUNTIME_NETCODE_SCORE_VALID;
            }
        }
    }
}

int runtime_netcode_hypersphere_build_deterministic_hash(
    const RuntimeNetcodeHypersphereOutput *output)
{
    unsigned int hash = 2166136261u;
    int i;
    int j;

    hash = hash_append_u32(hash, (unsigned int)output->dimensions);
    hash = hash_append_u32(hash, (unsigned int)output->cluster_count);
    hash = hash_append_u32(hash, (unsigned int)output->vector_count);

    for (i = 0; i < output->cluster_count; i++)
    {
        hash = hash_append_u32(hash, (unsigned int)output->centers[i].cluster_id);
        hash = hash_append_u32(hash, (unsigned int)output->centers[i].member_count);
        for (j = 0; j < output->dimensions; j++)
            hash = hash_append_u32(hash, (unsigned int)output->centers[i].center_q16[j]);

        hash = hash_append_u32(hash, (unsigned int)output->radii[i].nearest_neighbor_distance_q16);
        hash = hash_append_u32(hash, (unsigned int)output->radii[i].inscribed_radius_q16);
        hash = hash_append_u32(hash, (unsigned int)output->radii[i].radius_state);
        hash = hash_append_u32(hash, (unsigned int)output->spectral_proxy[i].frequency_proxy_q16);
        hash = hash_append_u32(hash, (unsigned int)output->spectral_proxy[i].amplitude_proxy_q16);
        hash = hash_append_u32(hash, (unsigned int)output->spectral_proxy[i].spectral_state);
    }

    for (i = 0; i < output->vector_count * output->cluster_count; i++)
    {
        hash = hash_append_u32(hash, (unsigned int)output->weighted_voronoi_scores[i].vector_id);
        hash = hash_append_u32(hash, (unsigned int)output->weighted_voronoi_scores[i].cluster_id);
        hash = hash_append_u32(hash, (unsigned int)output->weighted_voronoi_scores[i].distance_to_center_q16);
        hash = hash_append_u32(hash, (unsigned int)output->weighted_voronoi_scores[i].weighted_score_q16);
        hash = hash_append_u32(hash, (unsigned int)output->weighted_voronoi_scores[i].score_validity);
    }

    return (int)hash;
}
