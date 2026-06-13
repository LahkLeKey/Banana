#include "runtime/abi/netcode/netcode_abi.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-k3h4-determinism] %s\n", message);
    return 1;
}

int main(void)
{
    RuntimeK3h4VectorSignalInput input = {
        .call_density = 48,
        .quest_percent = 55,
        .player_level = 6,
        .combo_streak = 3,
        .branch_pressure = 22,
        .dependency_pulse = 37,
        .workflow_depth = 2,
        .neural_relevance_score = 61,
        .network_dimensions = 10,
        .model_confidence = 72,
        .policy_momentum = 64,
    };
    RuntimeNetcodeK3h4Output first_output;
    RuntimeNetcodeK3h4Output second_output;

    if (runtime_k3h4_abi_build_k3h4(input, &first_output) != 0)
        return fail("failed to build first k3h4 output");
    if (runtime_k3h4_abi_build_k3h4(input, &second_output) != 0)
        return fail("failed to build second k3h4 output");

    if (memcmp(&first_output, &second_output, sizeof(RuntimeNetcodeK3h4Output)) != 0)
        return fail("repeated deterministic output mismatch");

    if (first_output.cluster_count < 1 || first_output.cluster_count > 4)
        return fail("cluster count out of range");
    if (first_output.vector_count != 4)
        return fail("vector count mismatch");
    if (first_output.observability.deterministic_hash == 0)
        return fail("deterministic hash should be non-zero");
    if (first_output.observability.iteration_count <= 0)
        return fail("iteration count should be positive");

    if (first_output.weighted_voronoi_scores[0].score_validity != RUNTIME_NETCODE_SCORE_VALID)
        return fail("expected first weighted score to be valid");

    return 0;
}
