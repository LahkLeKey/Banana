#include "runtime/netcode/vector/netcode_vector.h"
#include "runtime/netcode/vector/netcode_k3h4.h"
#include "runtime/support/test_support.h"

#include <string.h>

static int g_k3h4_status = 0;

int runtime_netcode_k3h4_compute(
    const float vectors[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
    int vector_count,
    int dimensions,
    int cluster_count,
    int max_iterations,
    int convergence_threshold_q16,
    RuntimeNetcodeK3h4Result *out_result)
{
    (void)vectors;
    (void)vector_count;
    (void)dimensions;
    (void)cluster_count;
    (void)max_iterations;
    (void)convergence_threshold_q16;

    if (out_result)
        memset(out_result, 0, sizeof(*out_result));

    return g_k3h4_status;
}

static RuntimeNetcodeVectorInput make_input(void)
{
    RuntimeNetcodeVectorInput input;
    memset(&input, 0, sizeof(input));

    input.call_density = 54;
    input.quest_percent = 62;
    input.player_level = 4;
    input.combo_streak = 3;
    input.branch_pressure = 28;
    input.dependency_pulse = 36;
    input.workflow_depth = 2;
    input.neural_relevance_score = 58;
    input.network_dimensions = 10;
    input.model_confidence = 46;
    input.policy_momentum = 52;

    return input;
}

static void test_netcode_vector_build_returns_minus_one_when_k3h4_fails(void **state)
{
    RuntimeNetcodeVectorInput input;
    RuntimeNetcodeVectorOutput output;

    (void)state;

    input = make_input();
    memset(&output, 0, sizeof(output));
    g_k3h4_status = -1;

    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_vector_build(&input, &output), -1,
                              "netcode vector build must fail when k3h4 compute fails");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_netcode_vector_build_returns_minus_one_when_k3h4_fails)
)