#include "runtime/netcode/hypersphere/netcode_hypersphere.h"
#include "runtime/netcode/vector/netcode_fixed_point.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-covariance-ellipsoid] %s\n", message);
    return 1;
}

int main(void)
{
    RuntimeNetcodeVectorOutput vector_output;
    RuntimeNetcodeHypersphereOutput hypersphere_output;
    int dimensions = 2;
    int score_own;
    int score_cross;
    int legacy_score;

    memset(&vector_output, 0, sizeof(vector_output));
    vector_output.dimensions = dimensions;
    vector_output.kmeans_cluster_count = 2;
    vector_output.kmeans_member_counts[0] = 2;
    vector_output.kmeans_member_counts[1] = 2;

    /* Two elongated directional clusters to exercise anisotropic covariance. */
    vector_output.node_vectors[0][0] = 1.00f;
    vector_output.node_vectors[0][1] = 0.05f;
    vector_output.node_vectors[1][0] = 1.00f;
    vector_output.node_vectors[1][1] = 0.25f;
    vector_output.node_vectors[2][0] = 0.05f;
    vector_output.node_vectors[2][1] = 1.00f;
    vector_output.node_vectors[3][0] = 0.25f;
    vector_output.node_vectors[3][1] = 1.00f;

    vector_output.kmeans_assignments[0] = 0;
    vector_output.kmeans_assignments[1] = 0;
    vector_output.kmeans_assignments[2] = 1;
    vector_output.kmeans_assignments[3] = 1;

    vector_output.kmeans_centers_q16[0][0] = runtime_netcode_q16_from_float(0.99f);
    vector_output.kmeans_centers_q16[0][1] = runtime_netcode_q16_from_float(0.15f);
    vector_output.kmeans_centers_q16[1][0] = runtime_netcode_q16_from_float(0.15f);
    vector_output.kmeans_centers_q16[1][1] = runtime_netcode_q16_from_float(0.99f);

    if (runtime_netcode_hypersphere_build(&vector_output, &hypersphere_output) != 0)
        return fail("failed to build covariance ellipsoid output");

    score_own = hypersphere_output.weighted_voronoi_scores[0].weighted_score_q16;
    score_cross = hypersphere_output.weighted_voronoi_scores[1].weighted_score_q16;
    if (score_own >= score_cross)
        return fail("expected own-cluster Mahalanobis score to be lower than cross-cluster score");

    legacy_score = runtime_netcode_q16_div(
        hypersphere_output.weighted_voronoi_scores[0].distance_to_center_q16,
        hypersphere_output.radii[0].inscribed_radius_q16);

    if (legacy_score == score_own)
        return fail("expected covariance ellipsoid score path to differ from hypersphere legacy score path");

    if (hypersphere_output.weighted_voronoi_scores[0].score_validity != RUNTIME_NETCODE_SCORE_VALID)
        return fail("expected own-cluster score to be valid");

    return 0;
}
