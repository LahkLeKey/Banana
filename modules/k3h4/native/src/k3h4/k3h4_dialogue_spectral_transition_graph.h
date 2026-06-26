#ifndef BANANA_NATIVE_K3H4_DIALOGUE_SPECTRAL_TRANSITION_GRAPH_H
#define BANANA_NATIVE_K3H4_DIALOGUE_SPECTRAL_TRANSITION_GRAPH_H

#include "k3h4_dialogue_cluster_router.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum BananaK3h4DialogueSpectralState
    {
        BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE = 0,
        BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_REDIRECT = 1,
        BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ASSIST = 2,
        BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_DEESCALATE = 3,
        BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_DENY = 4,
    } BananaK3h4DialogueSpectralState;

    typedef enum BananaK3h4DialogueSpectralDenyReason
    {
        BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_NONE = 0,
        BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_ILLEGAL_JUMP = 1,
        BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_BOUNDARY_OUTSIDE = 2,
        BANANA_K3H4_DIALOGUE_SPECTRAL_DENY_GATE_BLOCKED = 3,
    } BananaK3h4DialogueSpectralDenyReason;

    enum
    {
        BANANA_K3H4_DIALOGUE_SPECTRAL_CANDIDATE_MAX = 3,
    };

    typedef struct BananaK3h4DialogueSpectralTransitionInput
    {
        BananaK3h4DialogueSpectralState current_state;
        BananaK3h4DialogueSpectralState requested_state;
        BananaK3h4DialogueRouteOutput route_output;
    } BananaK3h4DialogueSpectralTransitionInput;

    typedef struct BananaK3h4DialogueSpectralTransitionOutput
    {
        int graph_version;
        BananaK3h4DialogueSpectralState current_state;
        BananaK3h4DialogueSpectralState candidate_states[BANANA_K3H4_DIALOGUE_SPECTRAL_CANDIDATE_MAX];
        int candidate_count;
        BananaK3h4DialogueSpectralState next_state;
        int transition_allowed;
        BananaK3h4DialogueSpectralDenyReason deny_reason;
        int deny_reason_code;
    } BananaK3h4DialogueSpectralTransitionOutput;

    int banana_native_k3h4_resolve_spectral_transition(
        const BananaK3h4DialogueSpectralTransitionInput *input,
        BananaK3h4DialogueSpectralTransitionOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
