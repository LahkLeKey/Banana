#include "netcode_k3h4_geometry.h"

#include "../vector/netcode_fixed_point.h"

#include <math.h>

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

void runtime_netcode_k3h4_prepare_geometry(
    RuntimeNetcodeK3h4PipelineContext *context)
{
    int i;
    int j;

    for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
    {
        normalize_vector(context->input->node_vectors[i], context->dimensions, context->normalized[i]);
        for (j = 0; j < context->dimensions; j++)
            context->normalized_q16[i][j] = runtime_netcode_q16_from_float(context->normalized[i][j]);
    }

    for (j = 0; j < context->dimensions; j++)
    {
        float sum = 0.0f;
        for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
            sum += context->normalized[i][j];
        context->centroid[j] = sum / (float)RUNTIME_NETCODE_VECTOR_NODE_COUNT;
    }

    normalize_vector(context->centroid, context->dimensions, context->centroid_normalized);
}

void runtime_netcode_k3h4_build_projection_nodes(
    RuntimeNetcodeK3h4PipelineContext *context)
{
    int i;

    for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
    {
        float x;
        float y;
        float z;
        float coherence = ((dot(context->normalized[i], context->centroid_normalized, context->dimensions) + 1.0f) / 2.0f) * 100.0f;
        float radius;
        float nearest_neighbor_distance = 0.0f;
        float inradius = 0.0f;
        int nearest_set = 0;
        int neighbor;

        project_vector(context->normalized[i], context->dimensions, &x, &y, &z);
        radius = sqrtf(x * x + y * y);

        for (neighbor = 0; neighbor < RUNTIME_NETCODE_VECTOR_NODE_COUNT; neighbor++)
        {
            float candidate_distance;
            if (neighbor == i) continue;

            candidate_distance = euclidean_distance(context->normalized[i], context->normalized[neighbor], context->dimensions);
            if (!nearest_set || candidate_distance < nearest_neighbor_distance)
            {
                nearest_neighbor_distance = candidate_distance;
                nearest_set = 1;
            }
        }

        inradius = nearest_neighbor_distance * 0.5f;

        context->output->nodes[i].x = x;
        context->output->nodes[i].y = y;
        context->output->nodes[i].z = z;
        context->output->nodes[i].coherence = clamp_percent((int)lroundf(coherence));
        context->output->nodes[i].nearest_neighbor_distance = nearest_neighbor_distance;
        context->output->nodes[i].inradius = inradius;

        context->total_coherence += (float)context->output->nodes[i].coherence;
        context->total_radius += radius;
    }
}

void runtime_netcode_k3h4_finalize_geometry(
    RuntimeNetcodeK3h4PipelineContext *context)
{
    float average_radius = context->total_radius / (float)RUNTIME_NETCODE_VECTOR_NODE_COUNT;
    int i;

    for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
    {
        float radius = sqrtf(context->output->nodes[i].x * context->output->nodes[i].x +
                             context->output->nodes[i].y * context->output->nodes[i].y);
        float delta = radius - average_radius;
        context->radius_variance += delta * delta;
    }
    context->radius_variance /= (float)RUNTIME_NETCODE_VECTOR_NODE_COUNT;

    context->output->alignment =
        clamp_percent((int)lroundf(context->total_coherence / (float)RUNTIME_NETCODE_VECTOR_NODE_COUNT));
    context->output->radial_stability =
        clamp_percent((int)lroundf((1.0f - sqrtf(context->radius_variance)) * 100.0f));
}
