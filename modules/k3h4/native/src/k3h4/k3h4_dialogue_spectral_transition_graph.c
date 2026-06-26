#include "k3h4_dialogue_spectral_transition_graph.h"

#include <stddef.h>
#include <string.h>

enum
{
    BANANA_K3H4_DIALOGUE_SPECTRAL_GRAPH_VERSION = 1,
};

static int is_candidate(BananaK3h4DialogueSpectralState state,
                        const BananaK3h4DialogueSpectralTransitionOutput *output)
{
    int index;

    for (index = 0; index < output->candidate_count; ++index)
    {
        if (output->candidate_states[index] == state)
            return 1;
    }

    return 0;
}

static int build_deny_reason_code(BananaK3h4DialogueSpectralDenyReason reason,
                                  const BananaK3h4DialogueSpectralTransitionInput *input)
{
    uint32_t hash = 2166136261u;
    const uint32_t fields[] = {
        (uint32_t)reason,
        (uint32_t)input->current_state,
        (uint32_t)input->requested_state,
        (uint32_t)input->route_output.boundary_state,
        (uint32_t)input->route_output.resolved_gate,
    };
    size_t index;

    for (index = 0; index < sizeof(fields) / sizeof(fields[0]); ++index)
    {
        hash ^= fields[index];
        hash *= 16777619u;
    }

    return (int)(hash & 0x7fffffff);
}

static void populate_candidates(const BananaK3h4DialogueSpectralTransitionInput *input,
                                BananaK3h4DialogueSpectralTransitionOutput *out_output)
{
    out_output->candidate_count = 0;

    if (input->route_output.boundary_state == BANANA_K3H4_DIALOGUE_BOUNDARY_OUTSIDE)
    {
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_DENY;
        return;
    }

    if (input->route_output.resolved_gate != BANANA_K3H4_DIALOGUE_GATE_NONE)
    {
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_DENY;

        if (input->route_output.response_contract.response_mode == BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT)
            out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_REDIRECT;

        return;
    }

    switch (input->route_output.response_contract.response_mode)
    {
    case BANANA_K3H4_DIALOGUE_RESPONSE_ASSIST:
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ASSIST;
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_REDIRECT;
        break;
    case BANANA_K3H4_DIALOGUE_RESPONSE_DEESCALATE:
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_DEESCALATE;
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_REDIRECT;
        break;
    case BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT:
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_REDIRECT;
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE;
        break;
    case BANANA_K3H4_DIALOGUE_RESPONSE_DENY:
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_DENY;
        break;
    case BANANA_K3H4_DIALOGUE_RESPONSE_NEUTRAL:
    default:
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE;
        out_output->candidate_states[out_output->candidate_count++] = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_REDIRECT;
        break;
    }
}

int banana_native_k3h4_resolve_spectral_transition(
    const BananaK3h4DialogueSpectralTransitionInput *input,
    BananaK3h4DialogueSpectralTransitionOutput *out_output)
{
    BananaK3h4DialogueSpectralDenyReason deny_reason = BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_NONE;

    if (input == NULL || out_output == NULL)
        return -1;

    memset(out_output, 0, sizeof(*out_output));
    out_output->graph_version = BANANA_K3H4_DIALOGUE_SPECTRAL_GRAPH_VERSION;
    out_output->current_state = input->current_state;

    populate_candidates(input, out_output);

    if (input->route_output.boundary_state == BANANA_K3H4_DIALOGUE_BOUNDARY_OUTSIDE)
        deny_reason = BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_BOUNDARY_OUTSIDE;
    else if (input->route_output.resolved_gate != BANANA_K3H4_DIALOGUE_GATE_NONE)
        deny_reason = BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_GATE_BLOCKED;

    if (is_candidate(input->requested_state, out_output))
    {
        out_output->transition_allowed = 1;
        out_output->next_state = input->requested_state;
        out_output->deny_reason = BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_NONE;
        out_output->deny_reason_code = 0;
        return 0;
    }

    out_output->transition_allowed = 0;
    out_output->next_state = out_output->candidate_states[0];
    out_output->deny_reason =
        (deny_reason == BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_NONE)
            ? BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_ILLEGAL_JUMP
            : deny_reason;
    out_output->deny_reason_code = build_deny_reason_code(out_output->deny_reason, input);

    return -2;
}
