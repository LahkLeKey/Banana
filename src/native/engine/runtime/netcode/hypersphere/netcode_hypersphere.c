#include "netcode_hypersphere.h"

#include "../vector/netcode_fixed_point.h"

#include <math.h>
#include <string.h>

#ifndef BANANA_PI
#define BANANA_PI 3.14159265358979323846f
#endif

static int clamp_percent(int value)
{
    if (value < 0) return 0;
    if (value > 100) return 100;
    return value;
}

static void normalize_vector(const float *vector, int dimensions, float *out)
{
    int i;
    float sum = 0.0f;
    float magnitude;

    for (i = 0; i < dimensions; i++)
        sum += vector[i] * vector[i];

    magnitude = sqrtf(sum);
    if (magnitude <= 0.000001f)
    {
        for (i = 0; i < dimensions; i++) out[i] = 0.0f;
        return;
    }

    for (i = 0; i < dimensions; i++) out[i] = vector[i] / magnitude;
}

static float dot(const float *a, const float *b, int dimensions)
{
    int i;
    float result = 0.0f;
    for (i = 0; i < dimensions; i++)
        result += a[i] * b[i];
    return result;
}

static float euclidean_distance(const float *a, const float *b, int dimensions)
{
    int i;
    float sum = 0.0f;
    for (i = 0; i < dimensions; i++)
    {
        float delta = a[i] - b[i];
        sum += delta * delta;
    }
    return sqrtf(sum);
}

static int clamp_cluster_count(int value)
{
    if (value < 1) return 1;
    if (value > RUNTIME_NETCODE_VECTOR_NODE_COUNT) return RUNTIME_NETCODE_VECTOR_NODE_COUNT;
    return value;
}

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

static int build_deterministic_hash(const RuntimeNetcodeHypersphereOutput *output)
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
        {
            hash = hash_append_u32(hash, (unsigned int)output->centers[i].center_q16[j]);
        }
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

static void project_vector(const float *vector,
                           int dimensions,
                           float *out_x,
                           float *out_y,
                           float *out_z)
{
    int i;
    float x = 0.0f;
    float y = 0.0f;
    float scale;
    float nx;
    float ny;
    float planar_energy;

    for (i = 0; i < dimensions; i++)
    {
        float angle = (BANANA_PI * 2.0f * (float)i) / (float)dimensions;
        x += vector[i] * cosf(angle);
        y += vector[i] * sinf(angle);
    }

    scale = sqrtf(x * x + y * y);
    if (scale < 1.0f) scale = 1.0f;

    nx = x / scale;
    ny = y / scale;
    planar_energy = sqrtf(nx * nx + ny * ny);
    if (planar_energy > 1.0f) planar_energy = 1.0f;

    *out_x = nx;
    *out_y = ny;
    *out_z = sqrtf(fmaxf(0.0f, 1.0f - planar_energy * planar_energy));
}

int runtime_netcode_hypersphere_build(const RuntimeNetcodeVectorOutput *input,
                                      RuntimeNetcodeHypersphereOutput *out_output)
{
    float normalized[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    int normalized_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float centroid[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float centroid_normalized[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float total_coherence = 0.0f;
    float total_radius = 0.0f;
    float radius_variance = 0.0f;
    const int radius_floor_q16 = 64;
    float average_radius;
    int cluster_count;
    int dimensions;
    int i, j;

    if (!input || !out_output)
        return -1;

    dimensions = input->dimensions;
    if (dimensions <= 0 || dimensions > RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS)
        return -1;

    memset(out_output, 0, sizeof(*out_output));
    cluster_count = clamp_cluster_count(input->kmeans_cluster_count);

    for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
    {
        normalize_vector(input->node_vectors[i], dimensions, normalized[i]);
        for (j = 0; j < dimensions; j++)
            normalized_q16[i][j] = runtime_netcode_q16_from_float(normalized[i][j]);
    }

    for (j = 0; j < dimensions; j++)
    {
        float sum = 0.0f;
        for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
            sum += normalized[i][j];
        centroid[j] = sum / (float)RUNTIME_NETCODE_VECTOR_NODE_COUNT;
    }

    normalize_vector(centroid, dimensions, centroid_normalized);

    out_output->dimensions = dimensions;
    out_output->cluster_count = cluster_count;
    out_output->vector_count = RUNTIME_NETCODE_VECTOR_NODE_COUNT;

    for (i = 0; i < cluster_count; i++)
    {
        out_output->centers[i].cluster_id = i;
        out_output->centers[i].member_count = input->kmeans_member_counts[i];
        for (j = 0; j < dimensions; j++)
        {
            out_output->centers[i].center_q16[j] = input->kmeans_centers_q16[i][j];
        }
    }

    for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
    {
        float x, y, z;
        float coherence = ((dot(normalized[i], centroid_normalized, dimensions) + 1.0f) / 2.0f) * 100.0f;
        float radius;
        float nearest_neighbor_distance = 0.0f;
        float inradius = 0.0f;
        int nearest_set = 0;
        int neighbor;

        project_vector(normalized[i], dimensions, &x, &y, &z);
        radius = sqrtf(x * x + y * y);

        for (neighbor = 0; neighbor < RUNTIME_NETCODE_VECTOR_NODE_COUNT; neighbor++)
        {
            float candidate_distance;
            if (neighbor == i) continue;

            candidate_distance = euclidean_distance(normalized[i], normalized[neighbor], dimensions);
            if (!nearest_set || candidate_distance < nearest_neighbor_distance)
            {
                nearest_neighbor_distance = candidate_distance;
                nearest_set = 1;
            }
        }

        inradius = nearest_neighbor_distance * 0.5f;

        out_output->nodes[i].x = x;
        out_output->nodes[i].y = y;
        out_output->nodes[i].z = z;
        out_output->nodes[i].coherence = clamp_percent((int)lroundf(coherence));
        out_output->nodes[i].nearest_neighbor_distance = nearest_neighbor_distance;
        out_output->nodes[i].inradius = inradius;

        total_coherence += (float)out_output->nodes[i].coherence;
        total_radius += radius;
    }

    for (i = 0; i < cluster_count; i++)
    {
        int nearest_q16 = 0;
        int nearest_set = 0;
        int inscribed_q16 = 0;
        int radius_state = RUNTIME_NETCODE_RADIUS_OK;

        if (cluster_count <= 1)
        {
            radius_state = RUNTIME_NETCODE_RADIUS_SINGLE_CLUSTER;
            inscribed_q16 = radius_floor_q16;
            nearest_q16 = 0;
        }
        else
        {
            for (j = 0; j < cluster_count; j++)
            {
                int candidate_q16;
                if (i == j) continue;
                candidate_q16 = distance_q16_between_vectors(
                    out_output->centers[i].center_q16,
                    out_output->centers[j].center_q16,
                    dimensions);
                if (!nearest_set || candidate_q16 < nearest_q16)
                {
                    nearest_q16 = candidate_q16;
                    nearest_set = 1;
                }
            }

            inscribed_q16 = runtime_netcode_q16_round_div(nearest_q16, 2);
            if (inscribed_q16 < radius_floor_q16)
            {
                inscribed_q16 = radius_floor_q16;
                radius_state = RUNTIME_NETCODE_RADIUS_NEAR_ZERO_CLAMPED;
            }
        }

        out_output->radii[i].cluster_id = i;
        out_output->radii[i].nearest_neighbor_distance_q16 = nearest_q16;
        out_output->radii[i].inscribed_radius_q16 = inscribed_q16;
        out_output->radii[i].radius_state = radius_state;

        out_output->spectral_proxy[i].cluster_id = i;
        out_output->spectral_proxy[i].frequency_proxy_q16 = runtime_netcode_q16_div(NETCODE_Q16_ONE, inscribed_q16);
        out_output->spectral_proxy[i].amplitude_proxy_q16 = runtime_netcode_q16_round_div(
            ((int64_t)out_output->centers[i].member_count) << NETCODE_Q16_SHIFT,
            out_output->vector_count);
        out_output->spectral_proxy[i].spectral_state =
            radius_state == RUNTIME_NETCODE_RADIUS_NEAR_ZERO_CLAMPED
                ? RUNTIME_NETCODE_SPECTRAL_RADIUS_FLOOR_APPLIED
                : RUNTIME_NETCODE_SPECTRAL_OK;
    }

    for (i = 0; i < out_output->vector_count; i++)
    {
        for (j = 0; j < cluster_count; j++)
        {
            int score_index = i * cluster_count + j;
            int distance_q16 = distance_q16_between_vectors(
                normalized_q16[i],
                out_output->centers[j].center_q16,
                dimensions);
            int radius_q16 = out_output->radii[j].inscribed_radius_q16;

            out_output->weighted_voronoi_scores[score_index].vector_id = i;
            out_output->weighted_voronoi_scores[score_index].cluster_id = j;
            out_output->weighted_voronoi_scores[score_index].distance_to_center_q16 = distance_q16;

            if (out_output->radii[j].radius_state == RUNTIME_NETCODE_RADIUS_SINGLE_CLUSTER)
            {
                out_output->weighted_voronoi_scores[score_index].weighted_score_q16 = 0;
                out_output->weighted_voronoi_scores[score_index].score_validity =
                    RUNTIME_NETCODE_SCORE_INVALID_RADIUS;
            }
            else
            {
                out_output->weighted_voronoi_scores[score_index].weighted_score_q16 =
                    runtime_netcode_q16_div(distance_q16, radius_q16);
                out_output->weighted_voronoi_scores[score_index].score_validity =
                    RUNTIME_NETCODE_SCORE_VALID;
            }
        }
    }

    average_radius = total_radius / (float)RUNTIME_NETCODE_VECTOR_NODE_COUNT;
    for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
    {
        float radius = sqrtf(out_output->nodes[i].x * out_output->nodes[i].x +
                             out_output->nodes[i].y * out_output->nodes[i].y);
        float delta = radius - average_radius;
        radius_variance += delta * delta;
    }
    radius_variance /= (float)RUNTIME_NETCODE_VECTOR_NODE_COUNT;

    out_output->alignment =
        clamp_percent((int)lroundf(total_coherence / (float)RUNTIME_NETCODE_VECTOR_NODE_COUNT));
    out_output->radial_stability =
        clamp_percent((int)lroundf((1.0f - sqrtf(radius_variance)) * 100.0f));

    out_output->observability.convergence_status = input->kmeans_convergence_status;
    out_output->observability.iteration_count = input->kmeans_iteration_count;
    out_output->observability.assignment_changes_last_iteration =
        input->kmeans_assignment_changes_last_iteration;
    out_output->observability.endianness_decode_path =
        RUNTIME_NETCODE_ENDIANNESS_LITTLE_ENDIAN;
    out_output->observability.deterministic_hash = build_deterministic_hash(out_output);

    return 0;
}
