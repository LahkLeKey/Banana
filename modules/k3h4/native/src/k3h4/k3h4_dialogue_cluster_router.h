#ifndef BANANA_NATIVE_K3H4_DIALOGUE_CLUSTER_ROUTER_H
#define BANANA_NATIVE_K3H4_DIALOGUE_CLUSTER_ROUTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum BananaK3h4DialogueIntent
    {
        BANANA_K3H4_DIALOGUE_INTENT_UNKNOWN = 0,
        BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY = 1,
        BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS = 2,
        BANANA_K3H4_DIALOGUE_INTENT_INSULT = 3,
        BANANA_K3H4_DIALOGUE_INTENT_REQUEST_HELP = 4,
    } BananaK3h4DialogueIntent;

    typedef enum BananaK3h4DialogueBoundaryState
    {
        BANANA_K3H4_DIALOGUE_BOUNDARY_OUTSIDE = 0,
        BANANA_K3H4_DIALOGUE_BOUNDARY_EDGE = 1,
        BANANA_K3H4_DIALOGUE_BOUNDARY_CORE = 2,
    } BananaK3h4DialogueBoundaryState;

    typedef enum BananaK3h4DialogueSafetyGate
    {
        BANANA_K3H4_DIALOGUE_GATE_NONE = 0,
        BANANA_K3H4_DIALOGUE_GATE_DO_NOT_REVEAL_SECRET_TUNNEL = 1,
        BANANA_K3H4_DIALOGUE_GATE_DO_NOT_GRANT_ENTRY_WITHOUT_WRIT = 2,
    } BananaK3h4DialogueSafetyGate;

    typedef enum BananaK3h4DialogueResponseMode
    {
        BANANA_K3H4_DIALOGUE_RESPONSE_NEUTRAL = 0,
        BANANA_K3H4_DIALOGUE_RESPONSE_DENY = 1,
        BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT = 2,
        BANANA_K3H4_DIALOGUE_RESPONSE_DEESCALATE = 3,
        BANANA_K3H4_DIALOGUE_RESPONSE_ASSIST = 4,
    } BananaK3h4DialogueResponseMode;

    enum
    {
        BANANA_K3H4_DIALOGUE_GATE_MASK_SECRET_TUNNEL = 1u << 0,
        BANANA_K3H4_DIALOGUE_GATE_MASK_WRIT_REQUIRED = 1u << 1,
    };

    typedef struct BananaK3h4DialogueRouteInput
    {
        BananaK3h4DialogueIntent intent;
        int has_entry_writ;
        int mentions_secret_tunnel;
        int ambiguity_basis_points;
    } BananaK3h4DialogueRouteInput;

    typedef struct BananaK3h4DialogueResponseContract
    {
        int contract_version;
        BananaK3h4DialogueResponseMode response_mode;
        int may_grant_entry;
        int should_transition;
        int target_cluster_id;
        char response_key[64];
    } BananaK3h4DialogueResponseContract;

    typedef struct BananaK3h4DialogueRouteOutput
    {
        int route_version;
        int cluster_id;
        int normalized_score_basis_points;
        BananaK3h4DialogueBoundaryState boundary_state;
        int transition_id;
        BananaK3h4DialogueSafetyGate resolved_gate;
        int gate_precedence_rank;
        uint32_t triggered_gate_mask;
        int tie_break_applied;
        char tie_break_rule[32];
        BananaK3h4DialogueResponseContract response_contract;
    } BananaK3h4DialogueRouteOutput;

    /*
     * Routes the pilot dialogue slice for Edda at South Gate deterministically.
     * This function emits routing evidence only; gameplay mutations are owned
     * by higher layers across the state authority boundary.
     */
    int banana_native_k3h4_route_dialogue_cluster(const BananaK3h4DialogueRouteInput *input,
                                                   BananaK3h4DialogueRouteOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
