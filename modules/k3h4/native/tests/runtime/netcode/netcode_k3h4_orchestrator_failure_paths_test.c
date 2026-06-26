#include "runtime/netcode/k3h4/netcode_k3h4_orchestrator.h"
#include "runtime/support/test_support.h"

#include <string.h>

static int g_model_status = 0;
static int g_reward_status = 0;
static int g_link_status = 0;
static int g_vector_status = 0;
static int g_k3h4_status = 0;

static RuntimeNetcodeLearningInput g_last_learning_input;
static RuntimeNetcodeRewardInput g_last_reward_input;
static RuntimeNetcodeLinkInput g_last_link_input;
static RuntimeNetcodeVectorInput g_last_vector_input;
static int g_last_assignment_family = -1;
static int g_last_spectral_mode = -1;

int runtime_netcode_model_build(const RuntimeNetcodeLearningInput *input,
                                RuntimeNetcodeLearningOutput *out_output)
{
    if (input)
        g_last_learning_input = *input;
    if (out_output)
        memset(out_output, 0, sizeof(*out_output));
    return g_model_status;
}

int runtime_netcode_reward_build(const RuntimeNetcodeRewardInput *input,
                                 RuntimeNetcodeRewardOutput *out_output)
{
    if (input)
        g_last_reward_input = *input;
    if (out_output)
        memset(out_output, 0, sizeof(*out_output));
    return g_reward_status;
}

int runtime_netcode_link_build(const RuntimeNetcodeLinkInput *input,
                               RuntimeNetcodeLinkOutput *out_output)
{
    if (input)
        g_last_link_input = *input;
    if (out_output)
        memset(out_output, 0, sizeof(*out_output));
    return g_link_status;
}

int runtime_netcode_vector_build(const RuntimeNetcodeVectorInput *input,
                                 RuntimeNetcodeVectorOutput *out_output)
{
    if (input)
        g_last_vector_input = *input;
    if (out_output)
        memset(out_output, 0, sizeof(*out_output));
    return g_vector_status;
}

int runtime_netcode_k3h4_build_with_config(const RuntimeNetcodeVectorOutput *input,
                                           RuntimeNetcodeK3h4Output *out_output,
                                           int assignment_family,
                                           int spectral_mode)
{
    (void)input;
    if (out_output)
        memset(out_output, 0, sizeof(*out_output));

    g_last_assignment_family = assignment_family;
    g_last_spectral_mode = spectral_mode;

    return g_k3h4_status;
}

static void reset_statuses(void)
{
    g_model_status = 0;
    g_reward_status = 0;
    g_link_status = 0;
    g_vector_status = 0;
    g_k3h4_status = 0;
    g_last_assignment_family = -1;
    g_last_spectral_mode = -1;
    memset(&g_last_learning_input, 0, sizeof(g_last_learning_input));
    memset(&g_last_reward_input, 0, sizeof(g_last_reward_input));
    memset(&g_last_link_input, 0, sizeof(g_last_link_input));
    memset(&g_last_vector_input, 0, sizeof(g_last_vector_input));
}

static RuntimeNetcodeK3h4Request make_request(void)
{
    RuntimeNetcodeK3h4Request request;

    memset(&request, 0, sizeof(request));

    request.ledger.snapshots = 1;
    request.ledger.inspections = 2;
    request.ledger.trainings = 3;
    request.ledger.routes = 4;
    request.call_density = 11;
    request.quest_percent = 22;
    request.player_level = 9;
    request.combo_streak = 4;
    request.branch_pressure = 17;
    request.dependency_pulse = 31;
    request.workflow_depth = 6;
    request.interaction_signal = 45;
    request.neural_relevance_score = 72;
    request.network_dimensions = 10;
    request.model_confidence = 63;
    request.policy_momentum = 51;
    request.assignment_family = RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER;
    request.spectral_mode = RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH;
    request.hardware_byte_order_tag = 1;
    request.hardware_dtype_tag = 2;
    request.hardware_alignment_bytes = 4;

    return request;
}

static RuntimeNetcodeK3h4VectorSignalInput make_vector_signal(void)
{
    RuntimeNetcodeK3h4VectorSignalInput input;

    memset(&input, 0, sizeof(input));
    input.call_density = 7;
    input.quest_percent = 21;
    input.player_level = 14;
    input.combo_streak = 2;
    input.branch_pressure = 13;
    input.dependency_pulse = 19;
    input.workflow_depth = 3;
    input.neural_relevance_score = 55;
    input.network_dimensions = 10;
    input.model_confidence = 44;
    input.policy_momentum = 38;
    input.assignment_family = RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER;
    input.spectral_mode = RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH;

    return input;
}

static RuntimeNetcodeVectorInput make_vector_input(void)
{
    RuntimeNetcodeVectorInput input;

    memset(&input, 0, sizeof(input));
    input.call_density = 3;
    input.quest_percent = 6;
    input.player_level = 9;
    input.combo_streak = 12;
    input.branch_pressure = 15;
    input.dependency_pulse = 18;
    input.workflow_depth = 21;
    input.neural_relevance_score = 24;
    input.network_dimensions = 10;
    input.model_confidence = 27;
    input.policy_momentum = 30;

    return input;
}

static void test_orchestrate_full_null_and_step_failures(void **state)
{
    RuntimeNetcodeK3h4Request request;
    RuntimeNetcodeK3h4FullOutput out;

    (void)state;

    request = make_request();
    memset(&out, 0xAB, sizeof(out));

    reset_statuses();
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate_full(NULL, &out),
                              -1,
                              "orchestrate_full must reject null request");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate_full(&request, NULL),
                              -1,
                              "orchestrate_full must reject null output");

    reset_statuses();
    g_model_status = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate_full(&request, &out),
                              -1,
                              "orchestrate_full must fail when learning build fails");

    reset_statuses();
    g_reward_status = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate_full(&request, &out),
                              -1,
                              "orchestrate_full must fail when reward build fails");

    reset_statuses();
    g_link_status = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate_full(&request, &out),
                              -1,
                              "orchestrate_full must fail when link build fails");

    reset_statuses();
    g_vector_status = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate_full(&request, &out),
                              -1,
                              "orchestrate_full must fail when vector build fails");

    reset_statuses();
    g_k3h4_status = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate_full(&request, &out),
                              -1,
                              "orchestrate_full must fail when k3h4 build fails");
}

static void test_orchestrate_full_success_and_signal_mapping(void **state)
{
    RuntimeNetcodeK3h4Request request;
    RuntimeNetcodeK3h4FullOutput out;

    (void)state;

    request = make_request();
    memset(&out, 0xAB, sizeof(out));

    reset_statuses();
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate_full(&request, &out),
                              0,
                              "orchestrate_full must succeed when all stages succeed");

    BANANA_TEST_ASSERT_INT_EQ(g_last_learning_input.call_density,
                              request.call_density,
                              "orchestrate_full must map learning signal input");
    BANANA_TEST_ASSERT_INT_EQ(g_last_reward_input.interaction_signal,
                              request.interaction_signal,
                              "orchestrate_full must map reward interaction signal");
    BANANA_TEST_ASSERT_INT_EQ(g_last_link_input.player_level,
                              request.player_level,
                              "orchestrate_full must map link signal input");
    BANANA_TEST_ASSERT_INT_EQ(g_last_vector_input.policy_momentum,
                              request.policy_momentum,
                              "orchestrate_full must map vector signal input");
    BANANA_TEST_ASSERT_INT_EQ(g_last_assignment_family,
                              request.assignment_family,
                              "orchestrate_full must forward assignment family");
    BANANA_TEST_ASSERT_INT_EQ(g_last_spectral_mode,
                              request.spectral_mode,
                              "orchestrate_full must forward spectral mode");
}

static void test_build_helpers_cover_null_guards_and_success(void **state)
{
    RuntimeNetcodeInteractionLedger ledger;
    RuntimeNetcodeK3h4SignalInput signal;
    RuntimeNetcodeK3h4LinkSignalInput link;
    RuntimeNetcodeK3h4VectorSignalInput vector;
    RuntimeNetcodeLearningOutput learning;
    RuntimeNetcodeRewardOutput reward;
    RuntimeNetcodeLinkOutput link_out;
    RuntimeNetcodeVectorOutput vector_out;

    (void)state;

    memset(&ledger, 0, sizeof(ledger));
    memset(&signal, 0, sizeof(signal));
    memset(&link, 0, sizeof(link));
    memset(&vector, 0, sizeof(vector));
    memset(&learning, 0, sizeof(learning));
    memset(&reward, 0, sizeof(reward));
    memset(&link_out, 0, sizeof(link_out));
    memset(&vector_out, 0, sizeof(vector_out));

    reset_statuses();

    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_learning(NULL, &signal, &learning),
                              -1,
                              "build_learning must reject null ledger");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_learning(&ledger, NULL, &learning),
                              -1,
                              "build_learning must reject null signal input");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_learning(&ledger, &signal, NULL),
                              -1,
                              "build_learning must reject null learning output");

    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_reward(NULL, 0, &reward),
                              -1,
                              "build_reward must reject null signal input");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_reward(&signal, 0, NULL),
                              -1,
                              "build_reward must reject null reward output");

    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_link(NULL, &link_out),
                              -1,
                              "build_link must reject null link input");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_link(&link, NULL),
                              -1,
                              "build_link must reject null link output");

    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_vector(NULL, &vector_out),
                              -1,
                              "build_vector must reject null vector signal input");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_vector(&vector, NULL),
                              -1,
                              "build_vector must reject null vector output");

    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_learning(&ledger, &signal, &learning),
                              0,
                              "build_learning must propagate model success");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_reward(&signal, 8, &reward),
                              0,
                              "build_reward must propagate reward success");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_link(&link, &link_out),
                              0,
                              "build_link must propagate link success");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_vector(&vector, &vector_out),
                              0,
                              "build_vector must propagate vector success");
}

static void test_orchestrate_and_build_k3h4_paths(void **state)
{
    RuntimeNetcodeVectorInput vector_input;
    RuntimeNetcodeK3h4OrchestrationOutput orchestration_output;
    RuntimeNetcodeK3h4VectorSignalInput vector_signal;
    RuntimeNetcodeK3h4Output k3h4_output;

    (void)state;

    vector_input = make_vector_input();
    vector_signal = make_vector_signal();
    memset(&orchestration_output, 0, sizeof(orchestration_output));
    memset(&k3h4_output, 0, sizeof(k3h4_output));

    reset_statuses();
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate(NULL, &orchestration_output),
                              -1,
                              "orchestrate must reject null vector input");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate(&vector_input, NULL),
                              -1,
                              "orchestrate must reject null output");

    g_vector_status = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate(&vector_input, &orchestration_output),
                              -1,
                              "orchestrate must fail when vector build fails");

    reset_statuses();
    g_k3h4_status = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate(&vector_input, &orchestration_output),
                              -1,
                              "orchestrate must fail when k3h4 build fails");

    reset_statuses();
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_orchestrate(&vector_input, &orchestration_output),
                              0,
                              "orchestrate must succeed when both stages succeed");
    BANANA_TEST_ASSERT_INT_EQ(g_last_assignment_family,
                              RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE,
                              "orchestrate must use default assignment family");
    BANANA_TEST_ASSERT_INT_EQ(g_last_spectral_mode,
                              RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED,
                              "orchestrate must use default spectral mode");

    reset_statuses();
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_k3h4(NULL, &k3h4_output),
                              -1,
                              "build_k3h4 must reject null input");
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_k3h4(&vector_signal, NULL),
                              -1,
                              "build_k3h4 must reject null output");

    g_vector_status = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_k3h4(&vector_signal, &k3h4_output),
                              -1,
                              "build_k3h4 must fail when vector build fails");

    reset_statuses();
    g_k3h4_status = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_k3h4(&vector_signal, &k3h4_output),
                              -1,
                              "build_k3h4 must fail when k3h4 build fails");

    reset_statuses();
    BANANA_TEST_ASSERT_INT_EQ(runtime_netcode_k3h4_build_k3h4(&vector_signal, &k3h4_output),
                              0,
                              "build_k3h4 must succeed when both stages succeed");
    BANANA_TEST_ASSERT_INT_EQ(g_last_assignment_family,
                              vector_signal.assignment_family,
                              "build_k3h4 must forward assignment family");
    BANANA_TEST_ASSERT_INT_EQ(g_last_spectral_mode,
                              vector_signal.spectral_mode,
                              "build_k3h4 must forward spectral mode");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_orchestrate_full_null_and_step_failures),
    BANANA_TEST_CASE(test_orchestrate_full_success_and_signal_mapping),
    BANANA_TEST_CASE(test_build_helpers_cover_null_guards_and_success),
    BANANA_TEST_CASE(test_orchestrate_and_build_k3h4_paths)
)
