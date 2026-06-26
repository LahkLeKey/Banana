#include "k3h4/k3h4_dialogue_cluster_router.h"
#include "k3h4/k3h4_dialogue_spectral_transition_graph.h"

#include "runtime/support/test_support.h"

static int build_route(BananaK3h4DialogueIntent intent,
                       int has_entry_writ,
                       int mentions_secret_tunnel,
                       int ambiguity_basis_points,
                       BananaK3h4DialogueRouteOutput *out_output)
{
    BananaK3h4DialogueRouteInput route_input = {
        .intent = intent,
        .has_entry_writ = has_entry_writ,
        .mentions_secret_tunnel = mentions_secret_tunnel,
        .ambiguity_basis_points = ambiguity_basis_points,
    };

    return banana_native_k3h4_route_dialogue_cluster(&route_input, out_output);
}

static void test_generates_boundary_aware_transition_candidates(void **state)
{
    BananaK3h4DialogueRouteOutput route_output = {0};
    BananaK3h4DialogueSpectralTransitionInput transition_input;
    BananaK3h4DialogueSpectralTransitionOutput transition_output = {0};

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        build_route(BANANA_K3H4_DIALOGUE_INTENT_REQUEST_HELP, 1, 0, 0, &route_output),
        0,
        "route output generation must succeed");

    transition_input.current_state = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE;
    transition_input.requested_state = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ASSIST;
    transition_input.route_output = route_output;

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_resolve_spectral_transition(&transition_input, &transition_output),
        0,
        "legal transition request must resolve successfully");

    BANANA_TEST_ASSERT_INT_EQ(transition_output.transition_allowed,
                              1,
                              "assist transition should be allowed for assist response profile");
    BANANA_TEST_ASSERT_INT_EQ(transition_output.next_state,
                              BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ASSIST,
                              "next state must match requested legal assist state");
    BANANA_TEST_ASSERT_INT_EQ(transition_output.candidate_count,
                              2,
                              "assist profile should emit deterministic two-state candidate set");
}

static void test_blocks_illegal_jump_with_stable_deny_reason_codes(void **state)
{
    BananaK3h4DialogueRouteOutput route_output = {0};
    BananaK3h4DialogueSpectralTransitionInput transition_input;
    BananaK3h4DialogueSpectralTransitionOutput first_output = {0};
    BananaK3h4DialogueSpectralTransitionOutput second_output = {0};
    int first_status;
    int second_status;

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        build_route(BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS, 1, 0, 0, &route_output),
        0,
        "route output generation must succeed");

    transition_input.current_state = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE;
    transition_input.requested_state = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ASSIST;
    transition_input.route_output = route_output;

    first_status = banana_native_k3h4_resolve_spectral_transition(&transition_input, &first_output);
    second_status = banana_native_k3h4_resolve_spectral_transition(&transition_input, &second_output);

    BANANA_TEST_ASSERT_TRUE(first_status != 0,
                            "illegal jump must return non-zero status");
    BANANA_TEST_ASSERT_TRUE(second_status != 0,
                            "repeat illegal jump must remain non-zero");
    BANANA_TEST_ASSERT_INT_EQ(first_output.transition_allowed,
                              0,
                              "illegal jump must be disallowed");
    BANANA_TEST_ASSERT_INT_EQ(first_output.deny_reason,
                              BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_ILLEGAL_JUMP,
                              "illegal jump deny reason must be encoded");
    BANANA_TEST_ASSERT_INT_EQ(first_output.deny_reason_code,
                              second_output.deny_reason_code,
                              "deny reason code must be stable for identical illegal jump input");
}

static void test_gate_blocked_transitions_emit_gate_deny_reason(void **state)
{
    BananaK3h4DialogueRouteOutput route_output = {0};
    BananaK3h4DialogueSpectralTransitionInput transition_input;
    BananaK3h4DialogueSpectralTransitionOutput transition_output = {0};
    int status;

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        build_route(BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY, 0, 0, 0, &route_output),
        0,
        "route output generation must succeed");

    transition_input.current_state = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE;
    transition_input.requested_state = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ASSIST;
    transition_input.route_output = route_output;

    status = banana_native_k3h4_resolve_spectral_transition(&transition_input, &transition_output);

    BANANA_TEST_ASSERT_TRUE(status != 0,
                            "gate-blocked illegal request must return non-zero status");
    BANANA_TEST_ASSERT_INT_EQ(transition_output.deny_reason,
                              BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_GATE_BLOCKED,
                              "gate-blocked route must emit gate deny reason");
    BANANA_TEST_ASSERT_INT_EQ(transition_output.next_state,
                              BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_DENY,
                              "gate-blocked transition must deterministically resolve to deny state");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_generates_boundary_aware_transition_candidates),
    BANANA_TEST_CASE(test_blocks_illegal_jump_with_stable_deny_reason_codes),
    BANANA_TEST_CASE(test_gate_blocked_transitions_emit_gate_deny_reason))
