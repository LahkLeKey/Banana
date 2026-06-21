#include "k3h4/application/k3h4_metrics_application_service.h"

#include "../../runtime/support/test_support.h"

static RuntimeK3h4SignalInput g_last_signal_input;
static RuntimeK3h4LinkSignalInput g_last_link_input;
static RuntimeK3h4VectorSignalInput g_last_vector_input;
static int g_last_interaction_signal = 0;

static int stub_build_learning(RuntimeK3h4SignalInput signal_input,
                               RuntimeNetcodeLearningOutput *out_output)
{
    g_last_signal_input = signal_input;
    if (out_output)
        out_output->model_confidence = 88;
    return 101;
}

static int stub_build_reward(RuntimeK3h4SignalInput signal_input,
                             int interaction_signal,
                             RuntimeNetcodeRewardOutput *out_output)
{
    g_last_signal_input = signal_input;
    g_last_interaction_signal = interaction_signal;
    if (out_output)
        out_output->projected_reward_xp = 321;
    return 102;
}

static int stub_build_link(RuntimeK3h4LinkSignalInput signal_input,
                           RuntimeNetcodeLinkOutput *out_output)
{
    g_last_link_input = signal_input;
    if (out_output)
        out_output->player = 7;
    return 103;
}

static int stub_build_vector(RuntimeK3h4VectorSignalInput signal_input,
                             RuntimeNetcodeVectorOutput *out_output)
{
    g_last_vector_input = signal_input;
    if (out_output)
        out_output->dimensions = 3;
    return 104;
}

static int stub_build_k3h4(RuntimeK3h4VectorSignalInput signal_input,
                           RuntimeNetcodeK3h4Output *out_output)
{
    g_last_vector_input = signal_input;
    if (out_output)
        out_output->cluster_count = 2;
    return 105;
}

static void test_application_service_rejects_missing_port_or_callbacks(void **state)
{
    (void)state;
    RuntimeK3h4SignalInput signal_input = {0};
    RuntimeK3h4LinkSignalInput link_input = {0};
    RuntimeK3h4VectorSignalInput vector_input = {0};
    RuntimeNetcodeLearningOutput learning_output = {0};
    RuntimeNetcodeRewardOutput reward_output = {0};
    RuntimeNetcodeLinkOutput link_output = {0};
    RuntimeNetcodeVectorOutput vector_output = {0};
    RuntimeNetcodeK3h4Output k3h4_output = {0};
    BananaNativeK3h4DomainPort port = {0};

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_learning(NULL, signal_input, &learning_output),
        -1,
        "learning build must fail when domain port is null");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_reward(NULL, signal_input, 3, &reward_output),
        -1,
        "reward build must fail when domain port is null");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_link(NULL, link_input, &link_output),
        -1,
        "link build must fail when domain port is null");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_vector(NULL, vector_input, &vector_output),
        -1,
        "vector build must fail when domain port is null");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_k3h4(NULL, vector_input, &k3h4_output),
        -1,
        "k3h4 build must fail when domain port is null");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_learning(&port, signal_input, &learning_output),
        -1,
        "learning build must fail when learning callback is missing");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_reward(&port, signal_input, 3, &reward_output),
        -1,
        "reward build must fail when reward callback is missing");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_link(&port, link_input, &link_output),
        -1,
        "link build must fail when link callback is missing");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_vector(&port, vector_input, &vector_output),
        -1,
        "vector build must fail when vector callback is missing");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_k3h4(&port, vector_input, &k3h4_output),
        -1,
        "k3h4 build must fail when k3h4 callback is missing");
}

static void test_application_service_forwards_to_domain_port(void **state)
{
    (void)state;
    BananaNativeK3h4DomainPort port = {
        .build_learning = stub_build_learning,
        .build_reward = stub_build_reward,
        .build_link = stub_build_link,
        .build_vector = stub_build_vector,
        .build_k3h4 = stub_build_k3h4,
    };
    RuntimeK3h4SignalInput signal_input = {
        .call_density = 42,
        .quest_percent = 64,
        .combo_streak = 7,
        .branch_pressure = 27,
        .workflow_depth = 3,
    };
    RuntimeK3h4LinkSignalInput link_input = {
        .call_density = 42,
        .quest_percent = 64,
        .player_level = 5,
        .combo_streak = 7,
        .branch_pressure = 27,
        .dependency_pulse = 3,
        .interaction_signal = 9,
    };
    RuntimeK3h4VectorSignalInput vector_input = {
        .call_density = 42,
        .quest_percent = 64,
        .player_level = 5,
        .combo_streak = 7,
        .branch_pressure = 27,
        .dependency_pulse = 3,
        .workflow_depth = 3,
        .neural_relevance_score = 18,
        .network_dimensions = 3,
        .model_confidence = 74,
        .policy_momentum = 9,
        .assignment_family = RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE,
        .spectral_mode = RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED,
        .hardware_byte_order_tag = RUNTIME_K3H4_BYTE_ORDER_TAG,
        .hardware_dtype_tag = RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED,
        .hardware_alignment_bytes = RUNTIME_K3H4_ALIGNMENT_BYTES_4,
    };
    RuntimeNetcodeLearningOutput learning_output = {0};
    RuntimeNetcodeRewardOutput reward_output = {0};
    RuntimeNetcodeLinkOutput link_output = {0};
    RuntimeNetcodeVectorOutput vector_output = {0};
    RuntimeNetcodeK3h4Output k3h4_output = {0};

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_learning(&port, signal_input, &learning_output),
        101,
        "learning build must return the bound callback result");
    BANANA_TEST_ASSERT_INT_EQ(g_last_signal_input.call_density,
                              signal_input.call_density,
                              "learning build must forward signal input");
    BANANA_TEST_ASSERT_INT_EQ(learning_output.model_confidence,
                              88,
                              "learning build must forward the output buffer");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_reward(&port, signal_input, 9, &reward_output),
        102,
        "reward build must return the bound callback result");
    BANANA_TEST_ASSERT_INT_EQ(g_last_interaction_signal,
                              9,
                              "reward build must forward interaction signal");
    BANANA_TEST_ASSERT_INT_EQ(reward_output.projected_reward_xp,
                              321,
                              "reward build must forward the output buffer");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_link(&port, link_input, &link_output),
        103,
        "link build must return the bound callback result");
    BANANA_TEST_ASSERT_INT_EQ(g_last_link_input.player_level,
                              link_input.player_level,
                              "link build must forward link input");
    BANANA_TEST_ASSERT_INT_EQ(link_output.player,
                              7,
                              "link build must forward the output buffer");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_vector(&port, vector_input, &vector_output),
        104,
        "vector build must return the bound callback result");
    BANANA_TEST_ASSERT_INT_EQ(g_last_vector_input.network_dimensions,
                              vector_input.network_dimensions,
                              "vector build must forward vector input");
    BANANA_TEST_ASSERT_INT_EQ(vector_output.dimensions,
                              3,
                              "vector build must forward the output buffer");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_application_build_k3h4(&port, vector_input, &k3h4_output),
        105,
        "k3h4 build must return the bound callback result");
    BANANA_TEST_ASSERT_INT_EQ(g_last_vector_input.policy_momentum,
                              vector_input.policy_momentum,
                              "k3h4 build must forward vector input");
    BANANA_TEST_ASSERT_INT_EQ(k3h4_output.cluster_count,
                              2,
                              "k3h4 build must forward the output buffer");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_application_service_rejects_missing_port_or_callbacks),
    BANANA_TEST_CASE(test_application_service_forwards_to_domain_port)
)
