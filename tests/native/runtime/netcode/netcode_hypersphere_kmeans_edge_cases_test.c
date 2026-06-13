#include "runtime/netcode/hypersphere/netcode_hypersphere.h"
#include "runtime/netcode/vector/netcode_fixed_point.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-kmeans-edge] %s\n", message);
    return 1;
}

static void write_uniform_input(RuntimeNetcodeVectorOutput *output, int dimensions, float value)
{
    int node;
    int dim;
    memset(output, 0, sizeof(*output));
    output->dimensions = dimensions;
    for (node = 0; node < RUNTIME_NETCODE_VECTOR_NODE_COUNT; node++)
    {
        for (dim = 0; dim < dimensions; dim++)
        {
            output->node_vectors[node][dim] = value;
        }
    }
}

int main(void)
{
    RuntimeNetcodeVectorOutput vector_output;
    RuntimeNetcodeHypersphereOutput hypersphere_output;

    write_uniform_input(&vector_output, 6, 0.25f);
    vector_output.kmeans_cluster_count = 1;
    vector_output.kmeans_member_counts[0] = 4;
    for (int dim = 0; dim < 6; dim++)
    {
        vector_output.kmeans_centers_q16[0][dim] = runtime_netcode_q16_from_float(0.25f);
    }

    if (runtime_netcode_hypersphere_build(&vector_output, &hypersphere_output) != 0)
        return fail("failed to build single-cluster hypersphere");

    if (hypersphere_output.radii[0].radius_state != RUNTIME_NETCODE_RADIUS_SINGLE_CLUSTER)
        return fail("single-cluster radius state mismatch");
    if (hypersphere_output.weighted_voronoi_scores[0].score_validity != RUNTIME_NETCODE_SCORE_INVALID_RADIUS)
        return fail("single-cluster score validity should be invalid-radius");

    write_uniform_input(&vector_output, 6, 0.35f);
    vector_output.kmeans_cluster_count = 2;
    vector_output.kmeans_member_counts[0] = 2;
    vector_output.kmeans_member_counts[1] = 2;
    for (int dim = 0; dim < 6; dim++)
    {
        int q16 = runtime_netcode_q16_from_float(0.35f);
        vector_output.kmeans_centers_q16[0][dim] = q16;
        vector_output.kmeans_centers_q16[1][dim] = q16;
    }

    if (runtime_netcode_hypersphere_build(&vector_output, &hypersphere_output) != 0)
        return fail("failed to build near-zero-radius hypersphere");

    if (hypersphere_output.radii[0].radius_state != RUNTIME_NETCODE_RADIUS_NEAR_ZERO_CLAMPED)
        return fail("expected near-zero clamp state for cluster 0");
    if (hypersphere_output.radii[0].inscribed_radius_q16 <= 0)
        return fail("inscribed radius should be positive after clamp");
    if (hypersphere_output.spectral_proxy[0].spectral_state != RUNTIME_NETCODE_SPECTRAL_RADIUS_FLOOR_APPLIED)
        return fail("spectral state should indicate radius floor applied");

    return 0;
}
