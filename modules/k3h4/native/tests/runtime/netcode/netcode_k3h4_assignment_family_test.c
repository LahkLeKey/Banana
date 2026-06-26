#include "runtime/netcode/k3h4/netcode_k3h4_metrics.h"
#include "runtime/netcode/vector/netcode_fixed_point.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-k3h4-assignment-family] %s\n", message);
    return 1;
}

static void write_input(RuntimeNetcodeVectorOutput *output)
{
    memset(output, 0, sizeof(*output));
    output->dimensions = 2;
    output->k3h4_cluster_count = 2;
    output->k3h4_member_counts[0] = 2;
    output->k3h4_member_counts[1] = 2;

    output->k3h4_assignments[0] = 0;
    output->k3h4_assignments[1] = 0;
    output->k3h4_assignments[2] = 1;
    output->k3h4_assignments[3] = 1;

    output->node_vectors[0][0] = 0.0f;
    output->node_vectors[0][1] = 0.0f;
    output->node_vectors[1][0] = 0.1f;
    output->node_vectors[1][1] = 0.0f;
    output->node_vectors[2][0] = 1.0f;
    output->node_vectors[2][1] = 1.0f;
    output->node_vectors[3][0] = 1.1f;
    output->node_vectors[3][1] = 1.0f;

    output->k3h4_centers_q16[0][0] = runtime_netcode_q16_from_float(0.05f);
    output->k3h4_centers_q16[0][1] = runtime_netcode_q16_from_float(0.0f);
    output->k3h4_centers_q16[1][0] = runtime_netcode_q16_from_float(1.05f);
    output->k3h4_centers_q16[1][1] = runtime_netcode_q16_from_float(1.0f);
}

int main(void)
{
    RuntimeNetcodeVectorOutput vector_output;
    RuntimeNetcodeK3h4Output multiplicative_output;
    RuntimeNetcodeK3h4Output power_output;
    int score_index = 0;
    int distance_q16;
    int radius_q16;
    int expected_mul_q16;
    int expected_pow_q16;

    write_input(&vector_output);

    if (runtime_netcode_k3h4_build_with_config(
            &vector_output,
            &multiplicative_output,
            RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE,
            RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED) != 0)
        return fail("failed to build multiplicative k3h4 output");

    distance_q16 = multiplicative_output.weighted_voronoi_scores[score_index].distance_to_center_q16;
    radius_q16 = multiplicative_output.radii[0].inscribed_radius_q16;
    expected_mul_q16 = runtime_netcode_q16_div(distance_q16, radius_q16);

    if (multiplicative_output.weighted_voronoi_scores[score_index].weighted_score_q16 != expected_mul_q16)
        return fail("multiplicative weighted score formula mismatch");

    if (runtime_netcode_k3h4_build_with_config(
            &vector_output,
            &power_output,
            RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER,
            RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH) != 0)
        return fail("failed to build power k3h4 output");

    distance_q16 = power_output.weighted_voronoi_scores[score_index].distance_to_center_q16;
    radius_q16 = power_output.radii[0].inscribed_radius_q16;
    expected_pow_q16 =
        runtime_netcode_q16_mul(distance_q16, distance_q16) -
        runtime_netcode_q16_mul(radius_q16, radius_q16);

    if (power_output.weighted_voronoi_scores[score_index].weighted_score_q16 != expected_pow_q16)
        return fail("power weighted score formula mismatch");

    if (power_output.weighted_voronoi_scores[score_index].weighted_score_q16 ==
        multiplicative_output.weighted_voronoi_scores[score_index].weighted_score_q16)
    {
        return fail("assignment family should change weighted score semantics");
    }

    if (multiplicative_output.spectral_proxy[0].spectral_state != RUNTIME_NETCODE_SPECTRAL_DISABLED)
        return fail("expected spectral state to be disabled when spectral mode is bypassed");

    if (power_output.spectral_proxy[0].spectral_state == RUNTIME_NETCODE_SPECTRAL_DISABLED)
        return fail("expected spectral state to be active when affinity-graph spectral mode is requested");

    return 0;
}
