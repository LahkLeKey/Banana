#include "k3h4_dialogue_cluster_router.h"

#include <stddef.h>
#include <string.h>

enum
{
    BANANA_K3H4_DIALOGUE_ROUTE_VERSION = 1,
    BANANA_K3H4_RESPONSE_CONTRACT_VERSION = 1,
};

typedef struct BananaK3h4IntentRouteProfile
{
    int cluster_id;
    int base_score_basis_points;
} BananaK3h4IntentRouteProfile;

static int clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value)
        return min_value;
    if (value > max_value)
        return max_value;
    return value;
}

static void copy_literal(char *out_text, size_t out_text_size, const char *literal)
{
    if (out_text == NULL || out_text_size == 0)
        return;

    if (literal == NULL)
    {
        out_text[0] = '\0';
        return;
    }

    strncpy(out_text, literal, out_text_size - 1);
    out_text[out_text_size - 1] = '\0';
}

static BananaK3h4IntentRouteProfile profile_for_intent(BananaK3h4DialogueIntent intent)
{
    switch (intent)
    {
    case BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY:
        return (BananaK3h4IntentRouteProfile){ .cluster_id = 1001, .base_score_basis_points = 9200 };
    case BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS:
        return (BananaK3h4IntentRouteProfile){ .cluster_id = 1002, .base_score_basis_points = 8800 };
    case BANANA_K3H4_DIALOGUE_INTENT_INSULT:
        return (BananaK3h4IntentRouteProfile){ .cluster_id = 1003, .base_score_basis_points = 9000 };
    case BANANA_K3H4_DIALOGUE_INTENT_REQUEST_HELP:
        return (BananaK3h4IntentRouteProfile){ .cluster_id = 1004, .base_score_basis_points = 8700 };
    case BANANA_K3H4_DIALOGUE_INTENT_UNKNOWN:
    default:
        return (BananaK3h4IntentRouteProfile){ .cluster_id = 1099, .base_score_basis_points = 6200 };
    }
}

static BananaK3h4DialogueBoundaryState boundary_for_score(int score_basis_points)
{
    if (score_basis_points >= 8500)
        return BANANA_K3H4_DIALOGUE_BOUNDARY_CORE;
    if (score_basis_points >= 6500)
        return BANANA_K3H4_DIALOGUE_BOUNDARY_EDGE;
    return BANANA_K3H4_DIALOGUE_BOUNDARY_OUTSIDE;
}

static int compute_transition_id(const BananaK3h4DialogueRouteOutput *output)
{
    uint32_t hash = 2166136261u;
    const uint32_t fields[] = {
        (uint32_t)output->route_version,
        (uint32_t)output->cluster_id,
        (uint32_t)output->normalized_score_basis_points,
        (uint32_t)output->boundary_state,
        (uint32_t)output->resolved_gate,
        (uint32_t)output->gate_precedence_rank,
        output->triggered_gate_mask,
    };
    size_t index;

    for (index = 0; index < sizeof(fields) / sizeof(fields[0]); ++index)
    {
        hash ^= fields[index];
        hash *= 16777619u;
    }

    return (int)(hash & 0x7fffffff);
}

static void resolve_gates(const BananaK3h4DialogueRouteInput *input,
                          BananaK3h4DialogueRouteOutput *out_output)
{
    uint32_t triggered_mask = 0u;
    int secret_gate_triggered = input->mentions_secret_tunnel != 0;
    int writ_gate_triggered =
        input->intent == BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY && input->has_entry_writ == 0;

    if (secret_gate_triggered)
        triggered_mask |= BANANA_K3H4_DIALOGUE_GATE_MASK_SECRET_TUNNEL;
    if (writ_gate_triggered)
        triggered_mask |= BANANA_K3H4_DIALOGUE_GATE_MASK_WRIT_REQUIRED;

    out_output->triggered_gate_mask = triggered_mask;

    if (secret_gate_triggered)
    {
        out_output->resolved_gate = BANANA_K3H4_DIALOGUE_GATE_DO_NOT_REVEAL_SECRET_TUNNEL;
        out_output->gate_precedence_rank = 1;
        out_output->tie_break_applied = writ_gate_triggered ? 1 : 0;
        copy_literal(out_output->tie_break_rule,
                     sizeof(out_output->tie_break_rule),
                     writ_gate_triggered ? "precedence-order" : "single-gate");
        return;
    }

    if (writ_gate_triggered)
    {
        out_output->resolved_gate = BANANA_K3H4_DIALOGUE_GATE_DO_NOT_GRANT_ENTRY_WITHOUT_WRIT;
        out_output->gate_precedence_rank = 2;
        out_output->tie_break_applied = 0;
        copy_literal(out_output->tie_break_rule, sizeof(out_output->tie_break_rule), "single-gate");
        return;
    }

    out_output->resolved_gate = BANANA_K3H4_DIALOGUE_GATE_NONE;
    out_output->gate_precedence_rank = 0;
    out_output->tie_break_applied = 0;
    copy_literal(out_output->tie_break_rule, sizeof(out_output->tie_break_rule), "no-gate");
}

static void fill_response_contract(const BananaK3h4DialogueRouteInput *input,
                                   BananaK3h4DialogueRouteOutput *out_output)
{
    BananaK3h4DialogueResponseContract *contract = &out_output->response_contract;

    memset(contract, 0, sizeof(*contract));
    contract->contract_version = BANANA_K3H4_RESPONSE_CONTRACT_VERSION;
    contract->target_cluster_id = out_output->cluster_id;

    if (out_output->resolved_gate == BANANA_K3H4_DIALOGUE_GATE_DO_NOT_REVEAL_SECRET_TUNNEL)
    {
        contract->response_mode = BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT;
        contract->may_grant_entry = 0;
        contract->should_transition = 1;
        copy_literal(contract->response_key,
                     sizeof(contract->response_key),
                     "k3h4.edda.south_gate.redirect.public_route");
        return;
    }

    if (out_output->resolved_gate == BANANA_K3H4_DIALOGUE_GATE_DO_NOT_GRANT_ENTRY_WITHOUT_WRIT)
    {
        contract->response_mode = BANANA_K3H4_DIALOGUE_RESPONSE_DENY;
        contract->may_grant_entry = 0;
        contract->should_transition = 1;
        copy_literal(contract->response_key,
                     sizeof(contract->response_key),
                     "k3h4.edda.south_gate.require_writ");
        return;
    }

    switch (input->intent)
    {
    case BANANA_K3H4_DIALOGUE_INTENT_INSULT:
        contract->response_mode = BANANA_K3H4_DIALOGUE_RESPONSE_DEESCALATE;
        contract->may_grant_entry = 0;
        contract->should_transition = 1;
        copy_literal(contract->response_key,
                     sizeof(contract->response_key),
                     "k3h4.edda.south_gate.deescalate");
        return;
    case BANANA_K3H4_DIALOGUE_INTENT_REQUEST_HELP:
        contract->response_mode = BANANA_K3H4_DIALOGUE_RESPONSE_ASSIST;
        contract->may_grant_entry = input->has_entry_writ ? 1 : 0;
        contract->should_transition = 0;
        copy_literal(contract->response_key,
                     sizeof(contract->response_key),
                     "k3h4.edda.south_gate.assist");
        return;
    case BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS:
        contract->response_mode = BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT;
        contract->may_grant_entry = input->has_entry_writ ? 1 : 0;
        contract->should_transition = 0;
        copy_literal(contract->response_key,
                     sizeof(contract->response_key),
                     "k3h4.edda.south_gate.directions");
        return;
    case BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY:
        contract->response_mode = BANANA_K3H4_DIALOGUE_RESPONSE_NEUTRAL;
        contract->may_grant_entry = input->has_entry_writ ? 1 : 0;
        contract->should_transition = 0;
        copy_literal(contract->response_key,
                     sizeof(contract->response_key),
                     input->has_entry_writ ?
                         "k3h4.edda.south_gate.entry_review" :
                         "k3h4.edda.south_gate.entry_incomplete");
        return;
    case BANANA_K3H4_DIALOGUE_INTENT_UNKNOWN:
    default:
        contract->response_mode = BANANA_K3H4_DIALOGUE_RESPONSE_NEUTRAL;
        contract->may_grant_entry = 0;
        contract->should_transition = 1;
        copy_literal(contract->response_key,
                     sizeof(contract->response_key),
                     "k3h4.edda.south_gate.fallback");
        return;
    }
}

int banana_native_k3h4_route_dialogue_cluster(const BananaK3h4DialogueRouteInput *input,
                                               BananaK3h4DialogueRouteOutput *out_output)
{
    BananaK3h4IntentRouteProfile profile;

    if (input == NULL || out_output == NULL)
        return -1;

    memset(out_output, 0, sizeof(*out_output));

    profile = profile_for_intent(input->intent);
    out_output->route_version = BANANA_K3H4_DIALOGUE_ROUTE_VERSION;
    out_output->cluster_id = profile.cluster_id;
    out_output->normalized_score_basis_points = clamp_int(
        profile.base_score_basis_points - clamp_int(input->ambiguity_basis_points, 0, 3000),
        0,
        10000);
    out_output->boundary_state = boundary_for_score(out_output->normalized_score_basis_points);

    resolve_gates(input, out_output);
    fill_response_contract(input, out_output);

    out_output->transition_id = compute_transition_id(out_output);

    return 0;
}
