#include "netcode_hypersphere.h"

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
    float centroid[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float centroid_normalized[RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    float total_coherence = 0.0f;
    float total_radius = 0.0f;
    float radius_variance = 0.0f;
    float average_radius;
    int dimensions;
    int i, j;

    if (!input || !out_output)
        return -1;

    dimensions = input->dimensions;
    if (dimensions <= 0 || dimensions > RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS)
        return -1;

    for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
        normalize_vector(input->node_vectors[i], dimensions, normalized[i]);

    for (j = 0; j < dimensions; j++)
    {
        float sum = 0.0f;
        for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
            sum += normalized[i][j];
        centroid[j] = sum / (float)RUNTIME_NETCODE_VECTOR_NODE_COUNT;
    }

    normalize_vector(centroid, dimensions, centroid_normalized);

    out_output->dimensions = dimensions;
    for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
    {
        float x, y, z;
        float coherence = ((dot(normalized[i], centroid_normalized, dimensions) + 1.0f) / 2.0f) * 100.0f;
        float radius;

        project_vector(normalized[i], dimensions, &x, &y, &z);
        radius = sqrtf(x * x + y * y);

        out_output->nodes[i].x = x;
        out_output->nodes[i].y = y;
        out_output->nodes[i].z = z;
        out_output->nodes[i].coherence = clamp_percent((int)lroundf(coherence));

        total_coherence += (float)out_output->nodes[i].coherence;
        total_radius += radius;
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

    return 0;
}
