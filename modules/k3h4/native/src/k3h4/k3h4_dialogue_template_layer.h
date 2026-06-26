#ifndef BANANA_NATIVE_K3H4_DIALOGUE_TEMPLATE_LAYER_H
#define BANANA_NATIVE_K3H4_DIALOGUE_TEMPLATE_LAYER_H

#include "k3h4_dialogue_cluster_router.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum BananaK3h4DialogueFact
    {
        BANANA_K3H4_DIALOGUE_FACT_ENTRY_WRIT_STATUS = 0,
        BANANA_K3H4_DIALOGUE_FACT_PUBLIC_GATE_DIRECTIONS = 1,
        BANANA_K3H4_DIALOGUE_FACT_ESCALATION_WARNING = 2,
        BANANA_K3H4_DIALOGUE_FACT_HELP_GUIDANCE = 3,
        BANANA_K3H4_DIALOGUE_FACT_SECRET_TUNNEL_LOCATION = 4,
        BANANA_K3H4_DIALOGUE_FACT_GUARD_ROTATION_WINDOWS = 5,
    } BananaK3h4DialogueFact;

    typedef enum BananaK3h4DialogueTemplateReason
    {
        BANANA_K3H4_DIALOGUE_TEMPLATE_REASON_PRIMARY = 0,
        BANANA_K3H4_DIALOGUE_TEMPLATE_REASON_GATE_BLOCK = 1,
        BANANA_K3H4_DIALOGUE_TEMPLATE_REASON_BOUNDARY_OUTSIDE = 2,
        BANANA_K3H4_DIALOGUE_TEMPLATE_REASON_FACT_VALIDATION = 3,
    } BananaK3h4DialogueTemplateReason;

    enum
    {
        BANANA_K3H4_DIALOGUE_TEMPLATE_STACK_MAX = 3,
        BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ENTRY_WRIT_STATUS = 1u << BANANA_K3H4_DIALOGUE_FACT_ENTRY_WRIT_STATUS,
        BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS = 1u << BANANA_K3H4_DIALOGUE_FACT_PUBLIC_GATE_DIRECTIONS,
        BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ESCALATION_WARNING = 1u << BANANA_K3H4_DIALOGUE_FACT_ESCALATION_WARNING,
        BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_HELP_GUIDANCE = 1u << BANANA_K3H4_DIALOGUE_FACT_HELP_GUIDANCE,
        BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_SECRET_TUNNEL_LOCATION = 1u << BANANA_K3H4_DIALOGUE_FACT_SECRET_TUNNEL_LOCATION,
        BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_GUARD_ROTATION_WINDOWS = 1u << BANANA_K3H4_DIALOGUE_FACT_GUARD_ROTATION_WINDOWS,
    };

    typedef struct BananaK3h4DialogueTemplateInput
    {
        BananaK3h4DialogueRouteOutput route_output;
        uint32_t proposed_fact_mask;
    } BananaK3h4DialogueTemplateInput;

    typedef struct BananaK3h4DialogueTemplateValidationResult
    {
        uint32_t allowed_fact_mask;
        uint32_t forbidden_fact_mask;
        uint32_t unauthorized_fact_mask;
        int has_unauthorized_facts;
    } BananaK3h4DialogueTemplateValidationResult;

    typedef struct BananaK3h4DialogueTemplateOutput
    {
        int policy_version;
        int selected_cluster_stack[BANANA_K3H4_DIALOGUE_TEMPLATE_STACK_MAX];
        int selected_cluster_stack_count;
        int selected_cluster_for_template;
        char selected_template_key[64];
        BananaK3h4DialogueTemplateValidationResult fact_validation;
        int fallback_applied;
        char fallback_template_key[64];
        BananaK3h4DialogueTemplateReason fallback_reason;
        int fallback_rank;
    } BananaK3h4DialogueTemplateOutput;

    int banana_native_k3h4_resolve_dialogue_template(const BananaK3h4DialogueTemplateInput *input,
                                                      BananaK3h4DialogueTemplateOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
