#include "runtime/netcode/hypersphere/netcode_hypersphere.h"
#include "runtime/netcode/hypersphere/netcode_hypersphere_pipeline.h"
#include "runtime/netcode/vector/netcode_fixed_point.h"
#include "runtime/netcode/vector/netcode_vector.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-hypersphere-pipeline-equivalence] %s\n", message);
    return 1;
}

static int assert_envelope_zero(const RuntimeNetcodeHypersphereOutput *output,
                                const char *label)
{
    if (output->envelope.contract_version != 0 ||
        output->envelope.byte_order_tag != 0 ||
        output->envelope.payload_bytes != 0 ||
        output->envelope.payload_crc32 != 0 ||
        output->envelope.contract_status != 0)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s should keep envelope metadata zeroed\n",
                label);
        return 1;
    }
    return 0;
}

static int assert_outputs_equal(const RuntimeNetcodeHypersphereOutput *lhs,
                                const RuntimeNetcodeHypersphereOutput *rhs,
                                const char *label)
{
    if (memcmp(lhs, rhs, sizeof(RuntimeNetcodeHypersphereOutput)) != 0)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s\n",
                label);
        return 1;
    }
    return 0;
}

static int assert_coherence_bounds(const RuntimeNetcodeHypersphereOutput *output,
                                   const char *label)
{
    int node;
    for (node = 0; node < RUNTIME_NETCODE_VECTOR_NODE_COUNT; node++)
    {
        if (output->nodes[node].coherence < 0 || output->nodes[node].coherence > 100)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s coherence out of bounds at node %d\n",
                    label,
                    node);
            return 1;
        }
    }
    return 0;
}

static int assert_output_unchanged(const RuntimeNetcodeHypersphereOutput *actual,
                                   const RuntimeNetcodeHypersphereOutput *expected,
                                   const char *label)
{
    if (memcmp(actual, expected, sizeof(RuntimeNetcodeHypersphereOutput)) != 0)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s\n",
                label);
        return 1;
    }
    return 0;
}

typedef int (*RuntimeNetcodeHypersphereBuildFn)(
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeHypersphereOutput *out_output);

static int assert_failure_output_unchanged(
    RuntimeNetcodeHypersphereBuildFn build_fn,
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeHypersphereOutput *output,
    const RuntimeNetcodeHypersphereOutput *expected_output,
    const char *failure_label,
    const char *unchanged_label)
{
    if (build_fn(input, output) != -1)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s\n",
                failure_label);
        return 1;
    }

    return assert_output_unchanged(output, expected_output, unchanged_label);
}

static int assert_node_geometry_sanity(const RuntimeNetcodeHypersphereOutput *output,
                                       const char *label)
{
    int node;
    for (node = 0; node < RUNTIME_NETCODE_VECTOR_NODE_COUNT; node++)
    {
        if (output->nodes[node].nearest_neighbor_distance < 0.0f)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s nearest-neighbor distance negative at node %d\n",
                    label,
                    node);
            return 1;
        }

        if (output->nodes[node].inradius < 0.0f)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s inradius negative at node %d\n",
                    label,
                    node);
            return 1;
        }

        if (output->nodes[node].inradius > output->nodes[node].nearest_neighbor_distance)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s inradius exceeds nearest-neighbor distance at node %d\n",
                    label,
                    node);
            return 1;
        }
    }
    return 0;
}

static int assert_summary_metric_bounds(const RuntimeNetcodeHypersphereOutput *output,
                                        const char *label)
{
    if (output->alignment < 0 || output->alignment > 100)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s alignment out of bounds\n",
                label);
        return 1;
    }

    if (output->radial_stability < 0 || output->radial_stability > 100)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s radial_stability out of bounds\n",
                label);
        return 1;
    }

    return 0;
}

static int assert_cluster_contract_sanity(const RuntimeNetcodeHypersphereOutput *output,
                                          const char *label)
{
    int cluster;
    int score_index;
    int member_count_sum = 0;

    if (output->cluster_count < 1 || output->cluster_count > RUNTIME_NETCODE_VECTOR_NODE_COUNT)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s cluster_count out of bounds\n",
                label);
        return 1;
    }

    for (cluster = 0; cluster < output->cluster_count; cluster++)
    {
        const RuntimeNetcodeK3h4Center *center = &output->centers[cluster];
        const RuntimeNetcodeK3h4Radius *radius = &output->radii[cluster];
        const RuntimeNetcodeSpectralProxy *spectral = &output->spectral_proxy[cluster];

        if (center->cluster_id != cluster)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s center cluster_id mismatch at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }

        if (center->member_count < 0)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s center member_count negative at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }

        if (center->member_count > output->vector_count)
        {
            fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s center member_count exceeds vector_count at cluster %d\n",
                label,
                cluster);
            return 1;
        }

        member_count_sum += center->member_count;

        if (radius->cluster_id != cluster)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s radius cluster_id mismatch at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }

        if (radius->radius_state < RUNTIME_NETCODE_RADIUS_OK ||
            radius->radius_state > RUNTIME_NETCODE_RADIUS_NEAR_ZERO_CLAMPED)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s radius_state out of enum bounds at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }

        if (radius->inscribed_radius_q16 <= 0)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s inscribed_radius_q16 must be positive at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }

        if (spectral->cluster_id != cluster)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s spectral cluster_id mismatch at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }

        if (spectral->spectral_state < RUNTIME_NETCODE_SPECTRAL_OK ||
            spectral->spectral_state > RUNTIME_NETCODE_SPECTRAL_RADIUS_FLOOR_APPLIED)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s spectral_state out of enum bounds at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }

        if (spectral->frequency_proxy_q16 < 0 || spectral->amplitude_proxy_q16 < 0)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s spectral proxy values must be non-negative at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }

        if (radius->radius_state == RUNTIME_NETCODE_RADIUS_NEAR_ZERO_CLAMPED)
        {
            if (spectral->spectral_state != RUNTIME_NETCODE_SPECTRAL_RADIUS_FLOOR_APPLIED)
            {
                fprintf(stderr,
                        "[netcode-hypersphere-pipeline-equivalence] %s near-zero-clamped radius must map to radius-floor-applied spectral state at cluster %d\n",
                        label,
                        cluster);
                return 1;
            }
        }
        else if (spectral->spectral_state != RUNTIME_NETCODE_SPECTRAL_OK)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s non-clamped radius must map to spectral ok state at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }

        if (spectral->amplitude_proxy_q16 > NETCODE_Q16_ONE)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s spectral amplitude proxy exceeds q16 unit range at cluster %d\n",
                    label,
                    cluster);
            return 1;
        }
    }

    if (member_count_sum < 0 || member_count_sum > output->vector_count)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s member_count sum out of bounds\n",
                label);
        return 1;
    }

    for (score_index = 0;
         score_index < output->vector_count * output->cluster_count;
         score_index++)
    {
        const RuntimeNetcodeWeightedVoronoiScore *score = &output->weighted_voronoi_scores[score_index];
        const RuntimeNetcodeK3h4Radius *radius;

        if (score->vector_id < 0 || score->vector_id >= output->vector_count)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s weighted score vector_id out of bounds at index %d\n",
                    label,
                    score_index);
            return 1;
        }

        if (score->cluster_id < 0 || score->cluster_id >= output->cluster_count)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s weighted score cluster_id out of bounds at index %d\n",
                    label,
                    score_index);
            return 1;
        }

        if (score->distance_to_center_q16 < 0)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s weighted score distance negative at index %d\n",
                    label,
                    score_index);
            return 1;
        }

        if (score->weighted_score_q16 < 0)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s weighted score value negative at index %d\n",
                    label,
                    score_index);
            return 1;
        }

        if (score->score_validity < RUNTIME_NETCODE_SCORE_VALID ||
            score->score_validity > RUNTIME_NETCODE_SCORE_INVALID_RADIUS)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s weighted score validity out of enum bounds at index %d\n",
                    label,
                    score_index);
            return 1;
        }

        radius = &output->radii[score->cluster_id];

        if (score->score_validity == RUNTIME_NETCODE_SCORE_INVALID_RADIUS)
        {
            if (radius->radius_state != RUNTIME_NETCODE_RADIUS_SINGLE_CLUSTER)
            {
                fprintf(stderr,
                        "[netcode-hypersphere-pipeline-equivalence] %s invalid-radius score referenced non-single-cluster radius at index %d\n",
                        label,
                        score_index);
                return 1;
            }

            if (score->weighted_score_q16 != 0)
            {
                fprintf(stderr,
                        "[netcode-hypersphere-pipeline-equivalence] %s invalid-radius score should have zero weighted_score_q16 at index %d\n",
                        label,
                        score_index);
                return 1;
            }
        }
        else if (radius->radius_state == RUNTIME_NETCODE_RADIUS_SINGLE_CLUSTER)
        {
            fprintf(stderr,
                    "[netcode-hypersphere-pipeline-equivalence] %s valid score referenced single-cluster radius at index %d\n",
                    label,
                    score_index);
            return 1;
        }
    }

    return 0;
}

static int assert_observability_contract_sanity(
    const RuntimeNetcodeHypersphereOutput *output,
    const char *label)
{
    if (output->observability.iteration_count < 0)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s observability iteration_count negative\n",
                label);
        return 1;
    }

    if (output->observability.assignment_changes_last_iteration < 0 ||
        output->observability.assignment_changes_last_iteration > output->vector_count)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s assignment_changes_last_iteration out of bounds\n",
                label);
        return 1;
    }

    if (output->observability.endianness_decode_path < RUNTIME_NETCODE_ENDIANNESS_LITTLE_ENDIAN ||
        output->observability.endianness_decode_path > RUNTIME_NETCODE_ENDIANNESS_BYTE_SWAPPED)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s endianness_decode_path out of enum bounds\n",
                label);
        return 1;
    }

    if (output->observability.endianness_decode_path !=
        RUNTIME_NETCODE_ENDIANNESS_LITTLE_ENDIAN)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s raw hypersphere compute should report little-endian decode path\n",
                label);
        return 1;
    }

    if (output->observability.deterministic_hash == 0)
    {
        fprintf(stderr,
                "[netcode-hypersphere-pipeline-equivalence] %s deterministic_hash should be non-zero\n",
                label);
        return 1;
    }

    return 0;
}

static int assert_success_contract_pair(const RuntimeNetcodeHypersphereOutput *facade_output,
                                        const RuntimeNetcodeHypersphereOutput *pipeline_output,
                                        const char *parity_label,
                                        const char *facade_label,
                                        const char *pipeline_label)
{
    if (assert_outputs_equal(facade_output, pipeline_output, parity_label))
        return 1;

    if (assert_envelope_zero(facade_output, facade_label))
        return 1;
    if (assert_envelope_zero(pipeline_output, pipeline_label))
        return 1;

    if (assert_node_geometry_sanity(facade_output, facade_label))
        return 1;
    if (assert_node_geometry_sanity(pipeline_output, pipeline_label))
        return 1;

    if (assert_summary_metric_bounds(facade_output, facade_label))
        return 1;
    if (assert_summary_metric_bounds(pipeline_output, pipeline_label))
        return 1;

    if (assert_cluster_contract_sanity(facade_output, facade_label))
        return 1;
    if (assert_cluster_contract_sanity(pipeline_output, pipeline_label))
        return 1;

    if (assert_observability_contract_sanity(facade_output, facade_label))
        return 1;
    if (assert_observability_contract_sanity(pipeline_output, pipeline_label))
        return 1;

    return 0;
}

int main(void)
{
    RuntimeNetcodeVectorInput input = {
        .call_density = 45,
        .quest_percent = 57,
        .player_level = 8,
        .combo_streak = 2,
        .branch_pressure = 20,
        .dependency_pulse = 33,
        .workflow_depth = 2,
        .neural_relevance_score = 64,
        .network_dimensions = 10,
        .model_confidence = 70,
        .policy_momentum = 62,
    };
    RuntimeNetcodeVectorOutput vector_output;
    RuntimeNetcodeHypersphereOutput facade_output;
    RuntimeNetcodeHypersphereOutput pipeline_output;
    RuntimeNetcodeHypersphereOutput canonical_output;
    RuntimeNetcodeHypersphereOutput expected_facade_output;
    RuntimeNetcodeHypersphereOutput expected_pipeline_output;

    if (runtime_netcode_hypersphere_build(NULL, &facade_output) != -1)
        return fail("facade should reject NULL input with -1");
    if (runtime_netcode_hypersphere_pipeline_execute(NULL, &pipeline_output) != -1)
        return fail("pipeline should reject NULL input with -1");

    memset(&facade_output, 0x5A, sizeof(facade_output));
    memset(&pipeline_output, 0xA5, sizeof(pipeline_output));
    memcpy(&expected_facade_output, &facade_output, sizeof(expected_facade_output));
    memcpy(&expected_pipeline_output, &pipeline_output, sizeof(expected_pipeline_output));

    if (assert_failure_output_unchanged(
            runtime_netcode_hypersphere_build,
            NULL,
            &facade_output,
            &expected_facade_output,
            "facade NULL-input failure should remain deterministic",
            "facade should not mutate output on NULL-input failure"))
        return 1;
    if (assert_failure_output_unchanged(
            runtime_netcode_hypersphere_pipeline_execute,
            NULL,
            &pipeline_output,
            &expected_pipeline_output,
            "pipeline NULL-input failure should remain deterministic",
            "pipeline should not mutate output on NULL-input failure"))
        return 1;

    if (runtime_netcode_hypersphere_build(&vector_output, NULL) != -1)
        return fail("facade should reject NULL output with -1");
    if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, NULL) != -1)
        return fail("pipeline should reject NULL output with -1");

    if (runtime_netcode_vector_build(&input, &vector_output) != 0)
        return fail("failed to build vector output");

    memset(&facade_output, 0xAA, sizeof(facade_output));
    memset(&pipeline_output, 0x55, sizeof(pipeline_output));
    if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
        return fail("facade hypersphere build failed");

    if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
        return fail("pipeline hypersphere execution failed");

    if (assert_success_contract_pair(&facade_output,
                                     &pipeline_output,
                                     "facade and pipeline outputs diverged",
                                     "raw hypersphere facade output",
                                     "raw hypersphere pipeline output"))
        return 1;

    memcpy(&canonical_output, &facade_output, sizeof(canonical_output));

    vector_output.k3h4_cluster_count = 0;
    memset(&facade_output, 0xAA, sizeof(facade_output));
    memset(&pipeline_output, 0x55, sizeof(pipeline_output));
    if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
        return fail("facade low-cluster clamp build failed");
    if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
        return fail("pipeline low-cluster clamp build failed");
    if (assert_success_contract_pair(&facade_output,
                                     &pipeline_output,
                                     "low-cluster clamp outputs diverged",
                                     "low-cluster facade output",
                                     "low-cluster pipeline output"))
        return 1;
    if (facade_output.cluster_count != 1)
        return fail("expected low cluster-count clamp to 1");

    vector_output.k3h4_cluster_count = 99;
    memset(&facade_output, 0xAA, sizeof(facade_output));
    memset(&pipeline_output, 0x55, sizeof(pipeline_output));
    if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
        return fail("facade high-cluster clamp build failed");
    if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
        return fail("pipeline high-cluster clamp build failed");
    if (assert_success_contract_pair(&facade_output,
                                     &pipeline_output,
                                     "high-cluster clamp outputs diverged",
                                     "high-cluster facade output",
                                     "high-cluster pipeline output"))
        return 1;
    if (facade_output.cluster_count != RUNTIME_NETCODE_VECTOR_NODE_COUNT)
        return fail("expected high cluster-count clamp to vector node count");

    vector_output.k3h4_cluster_count = 2;
    memset(&facade_output, 0xAB, sizeof(facade_output));
    memset(&pipeline_output, 0xBA, sizeof(pipeline_output));
    if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
        return fail("facade in-range cluster-count build failed");
    if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
        return fail("pipeline in-range cluster-count build failed");
    if (assert_success_contract_pair(&facade_output,
                                     &pipeline_output,
                                     "in-range cluster-count outputs diverged",
                                     "in-range cluster-count facade output",
                                     "in-range cluster-count pipeline output"))
        return 1;
    if (facade_output.cluster_count != 2)
        return fail("expected in-range cluster count to remain unchanged");

    vector_output.dimensions = 0;
    memset(&facade_output, 0x11, sizeof(facade_output));
    memset(&pipeline_output, 0x22, sizeof(pipeline_output));
    memcpy(&expected_facade_output, &facade_output, sizeof(expected_facade_output));
    memcpy(&expected_pipeline_output, &pipeline_output, sizeof(expected_pipeline_output));

    if (assert_failure_output_unchanged(
            runtime_netcode_hypersphere_build,
            &vector_output,
            &facade_output,
            &expected_facade_output,
            "facade should reject zero dimensions with -1",
            "facade should not mutate output on zero-dimension failure"))
        return 1;
    if (assert_failure_output_unchanged(
            runtime_netcode_hypersphere_pipeline_execute,
            &vector_output,
            &pipeline_output,
            &expected_pipeline_output,
            "pipeline should reject zero dimensions with -1",
            "pipeline should not mutate output on zero-dimension failure"))
        return 1;

    vector_output.dimensions = RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS + 1;
    memset(&facade_output, 0x33, sizeof(facade_output));
    memset(&pipeline_output, 0x44, sizeof(pipeline_output));
    memcpy(&expected_facade_output, &facade_output, sizeof(expected_facade_output));
    memcpy(&expected_pipeline_output, &pipeline_output, sizeof(expected_pipeline_output));

    if (assert_failure_output_unchanged(
            runtime_netcode_hypersphere_build,
            &vector_output,
            &facade_output,
            &expected_facade_output,
            "facade should reject oversized dimensions with -1",
            "facade should not mutate output on oversized-dimension failure"))
        return 1;
    if (assert_failure_output_unchanged(
            runtime_netcode_hypersphere_pipeline_execute,
            &vector_output,
            &pipeline_output,
            &expected_pipeline_output,
            "pipeline should reject oversized dimensions with -1",
            "pipeline should not mutate output on oversized-dimension failure"))
        return 1;

    if (runtime_netcode_vector_build(&input, &vector_output) != 0)
        return fail("failed to rebuild vector output after failure injections");

    memset(&facade_output, 0x77, sizeof(facade_output));
    memset(&pipeline_output, 0x88, sizeof(pipeline_output));

    if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
        return fail("facade recovery build failed");
    if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
        return fail("pipeline recovery build failed");

    if (assert_success_contract_pair(&facade_output,
                                     &pipeline_output,
                                     "recovery outputs diverged after failure injections",
                                     "recovery facade output",
                                     "recovery pipeline output"))
        return 1;
    if (assert_outputs_equal(&facade_output, &canonical_output,
                             "recovery facade output drifted from canonical success output"))
        return 1;

    memset(&facade_output, 0x91, sizeof(facade_output));
    memset(&pipeline_output, 0x19, sizeof(pipeline_output));

    if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
        return fail("pipeline order-independence build failed");
    if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
        return fail("facade order-independence build failed");

    if (assert_success_contract_pair(&facade_output,
                                     &pipeline_output,
                                     "order-independence outputs diverged",
                                     "order-independence facade output",
                                     "order-independence pipeline output"))
        return 1;
    if (assert_outputs_equal(&pipeline_output, &canonical_output,
                             "order-independence output drifted from canonical success output"))
        return 1;

    for (int iteration = 0; iteration < 6; iteration++)
    {
        memset(&facade_output, 0x30 + iteration, sizeof(facade_output));
        memset(&pipeline_output, 0x90 - iteration, sizeof(pipeline_output));

        if ((iteration % 2) == 0)
        {
            if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
                return fail("alternating loop facade build failed");
            if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
                return fail("alternating loop pipeline execute failed");
        }
        else
        {
            if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
                return fail("alternating loop pipeline execute failed");
            if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
                return fail("alternating loop facade build failed");
        }

        if (assert_success_contract_pair(&facade_output,
                                         &pipeline_output,
                                         "alternating loop outputs diverged",
                                         "alternating loop facade output",
                                         "alternating loop pipeline output"))
            return 1;
        if (assert_outputs_equal(&facade_output, &canonical_output,
                                 "alternating loop output drifted from canonical success output"))
            return 1;
    }

    {
        const RuntimeNetcodeVectorInput parity_inputs[] = {
            {
                .call_density = 17,
                .quest_percent = 29,
                .player_level = 2,
                .combo_streak = 1,
                .branch_pressure = 9,
                .dependency_pulse = 13,
                .workflow_depth = 1,
                .neural_relevance_score = 31,
                .network_dimensions = 1,
                .model_confidence = 44,
                .policy_momentum = 27,
            },
            {
                .call_density = 22,
                .quest_percent = 38,
                .player_level = 3,
                .combo_streak = 1,
                .branch_pressure = 14,
                .dependency_pulse = 19,
                .workflow_depth = 1,
                .neural_relevance_score = 41,
                .network_dimensions = 7,
                .model_confidence = 52,
                .policy_momentum = 35,
            },
            {
                .call_density = 61,
                .quest_percent = 74,
                .player_level = 11,
                .combo_streak = 5,
                .branch_pressure = 29,
                .dependency_pulse = 44,
                .workflow_depth = 3,
                .neural_relevance_score = 73,
                .network_dimensions = 12,
                .model_confidence = 85,
                .policy_momentum = 79,
            },
            {
                .call_density = 33,
                .quest_percent = 47,
                .player_level = 6,
                .combo_streak = 2,
                .branch_pressure = 18,
                .dependency_pulse = 27,
                .workflow_depth = 2,
                .neural_relevance_score = 58,
                .network_dimensions = 9,
                .model_confidence = 66,
                .policy_momentum = 54,
            },
            {
                .call_density = 68,
                .quest_percent = 81,
                .player_level = 14,
                .combo_streak = 6,
                .branch_pressure = 35,
                .dependency_pulse = 53,
                .workflow_depth = 3,
                .neural_relevance_score = 89,
                .network_dimensions = RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS,
                .model_confidence = 92,
                .policy_momentum = 88,
            },
        };

        for (int case_index = 0;
             case_index < (int)(sizeof(parity_inputs) / sizeof(parity_inputs[0]));
             case_index++)
        {
            int exact_member_count_sum = 0;

            if (runtime_netcode_vector_build(&parity_inputs[case_index], &vector_output) != 0)
                return fail("failed to build vector output for parity scenario");

            memset(&facade_output, 0x20 + case_index, sizeof(facade_output));
            memset(&pipeline_output, 0xE0 - case_index, sizeof(pipeline_output));

            if ((case_index % 2) == 0)
            {
                if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
                    return fail("facade build failed for parity scenario");
                if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
                    return fail("pipeline execute failed for parity scenario");
            }
            else
            {
                if (runtime_netcode_hypersphere_pipeline_execute(&vector_output, &pipeline_output) != 0)
                    return fail("pipeline execute failed for parity scenario");
                if (runtime_netcode_hypersphere_build(&vector_output, &facade_output) != 0)
                    return fail("facade build failed for parity scenario");
            }

            if (assert_success_contract_pair(&facade_output,
                                             &pipeline_output,
                                             "multi-input parity scenario outputs diverged",
                                             "multi-input facade output",
                                             "multi-input pipeline output"))
                return 1;

            if (facade_output.dimensions != vector_output.dimensions)
                return fail("multi-input parity scenario dimensions mismatch");
            if (facade_output.dimensions < 6 ||
                facade_output.dimensions > RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS)
                return fail("multi-input parity scenario dimensions out of normalized bounds");
            if (facade_output.vector_count != RUNTIME_NETCODE_VECTOR_NODE_COUNT)
                return fail("multi-input parity scenario vector_count mismatch");
            if (facade_output.cluster_count < 1 ||
                facade_output.cluster_count > RUNTIME_NETCODE_VECTOR_NODE_COUNT)
                return fail("multi-input parity scenario cluster_count out of bounds");
            if (facade_output.cluster_count != vector_output.k3h4_cluster_count)
                return fail("multi-input parity scenario cluster_count drifted from vector k3h4 result");
            if (facade_output.observability.deterministic_hash == 0)
                return fail("multi-input parity scenario deterministic hash should be non-zero");

            for (int cluster = 0; cluster < facade_output.cluster_count; cluster++)
            {
                exact_member_count_sum += facade_output.centers[cluster].member_count;
            }
            if (exact_member_count_sum != facade_output.vector_count)
                return fail("multi-input parity scenario member_count sum mismatch");

            if (assert_coherence_bounds(&facade_output,
                                        "multi-input parity scenario facade output"))
                return 1;
            if (assert_coherence_bounds(&pipeline_output,
                                        "multi-input parity scenario pipeline output"))
                return 1;

        }
    }

    return 0;
}
