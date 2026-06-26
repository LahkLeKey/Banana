#include "k3h4_dialogue_template_layer.h"

#include <stddef.h>
#include <string.h>

enum
{
    BANANA_K3H4_DIALOGUE_TEMPLATE_POLICY_VERSION = 1,
    BANANA_K3H4_CLUSTER_GLOBAL_FALLBACK = 1099,
};

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

static int fallback_cluster_for_mode(BananaK3h4DialogueResponseMode mode)
{
    switch (mode)
    {
    case BANANA_K3H4_DIALOGUE_RESPONSE_DENY:
        return 1001;
    case BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT:
        return 1002;
    case BANANA_K3H4_DIALOGUE_RESPONSE_DEESCALATE:
        return 1003;
    case BANANA_K3H4_DIALOGUE_RESPONSE_ASSIST:
        return 1004;
    case BANANA_K3H4_DIALOGUE_RESPONSE_NEUTRAL:
    default:
        return 1001;
    }
}

static uint32_t forbidden_fact_mask_for_route(const BananaK3h4DialogueRouteOutput *route_output)
{
    uint32_t forbidden = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_SECRET_TUNNEL_LOCATION |
                         BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_GUARD_ROTATION_WINDOWS;

    if (route_output->resolved_gate == BANANA_K3H4_DIALOGUE_GATE_DO_NOT_REVEAL_SECRET_TUNNEL)
        forbidden |= BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS;

    if (route_output->resolved_gate == BANANA_K3H4_DIALOGUE_GATE_DO_NOT_GRANT_ENTRY_WITHOUT_WRIT)
        forbidden |= BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ENTRY_WRIT_STATUS;

    return forbidden;
}

static uint32_t allowed_fact_mask_for_route(const BananaK3h4DialogueRouteOutput *route_output)
{
    uint32_t allowed = 0u;

    switch (route_output->response_contract.response_mode)
    {
    case BANANA_K3H4_DIALOGUE_RESPONSE_DENY:
        allowed = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ESCALATION_WARNING;
        break;
    case BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT:
        allowed = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS |
                  BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ESCALATION_WARNING;
        break;
    case BANANA_K3H4_DIALOGUE_RESPONSE_DEESCALATE:
        allowed = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ESCALATION_WARNING;
        break;
    case BANANA_K3H4_DIALOGUE_RESPONSE_ASSIST:
        allowed = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_HELP_GUIDANCE |
                  BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS;
        break;
    case BANANA_K3H4_DIALOGUE_RESPONSE_NEUTRAL:
    default:
        allowed = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ENTRY_WRIT_STATUS |
                  BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS;
        break;
    }

    return allowed;
}

static void build_cluster_stack(const BananaK3h4DialogueRouteOutput *route_output,
                                BananaK3h4DialogueTemplateOutput *out_output)
{
    out_output->selected_cluster_stack_count = BANANA_K3H4_DIALOGUE_TEMPLATE_STACK_MAX;
    out_output->selected_cluster_stack[0] = route_output->cluster_id;
    out_output->selected_cluster_stack[1] = fallback_cluster_for_mode(route_output->response_contract.response_mode);
    out_output->selected_cluster_stack[2] = BANANA_K3H4_CLUSTER_GLOBAL_FALLBACK;
}

static void resolve_primary_template(const BananaK3h4DialogueRouteOutput *route_output,
                                     BananaK3h4DialogueTemplateOutput *out_output)
{
    out_output->selected_cluster_for_template = out_output->selected_cluster_stack[0];

    if (route_output->resolved_gate == BANANA_K3H4_DIALOGUE_GATE_DO_NOT_REVEAL_SECRET_TUNNEL)
    {
        copy_literal(out_output->selected_template_key,
                     sizeof(out_output->selected_template_key),
                     "k3h4.template.edda.south_gate.redirect_public");
        return;
    }

    if (route_output->resolved_gate == BANANA_K3H4_DIALOGUE_GATE_DO_NOT_GRANT_ENTRY_WITHOUT_WRIT)
    {
        copy_literal(out_output->selected_template_key,
                     sizeof(out_output->selected_template_key),
                     "k3h4.template.edda.south_gate.require_writ");
        return;
    }

    switch (route_output->response_contract.response_mode)
    {
    case BANANA_K3H4_DIALOGUE_RESPONSE_DEESCALATE:
        copy_literal(out_output->selected_template_key,
                     sizeof(out_output->selected_template_key),
                     "k3h4.template.edda.south_gate.deescalate");
        return;
    case BANANA_K3H4_DIALOGUE_RESPONSE_ASSIST:
        copy_literal(out_output->selected_template_key,
                     sizeof(out_output->selected_template_key),
                     "k3h4.template.edda.south_gate.assist");
        return;
    case BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT:
        copy_literal(out_output->selected_template_key,
                     sizeof(out_output->selected_template_key),
                     "k3h4.template.edda.south_gate.redirect");
        return;
    case BANANA_K3H4_DIALOGUE_RESPONSE_DENY:
        copy_literal(out_output->selected_template_key,
                     sizeof(out_output->selected_template_key),
                     "k3h4.template.edda.south_gate.deny");
        return;
    case BANANA_K3H4_DIALOGUE_RESPONSE_NEUTRAL:
    default:
        copy_literal(out_output->selected_template_key,
                     sizeof(out_output->selected_template_key),
                     "k3h4.template.edda.south_gate.neutral");
        return;
    }
}

static void resolve_fallback(BananaK3h4DialogueTemplateOutput *out_output,
                             BananaK3h4DialogueTemplateReason reason,
                             int fallback_rank,
                             const char *fallback_template_key)
{
    out_output->fallback_applied = 1;
    out_output->fallback_reason = reason;
    out_output->fallback_rank = fallback_rank;
    out_output->selected_cluster_for_template = out_output->selected_cluster_stack[fallback_rank];
    copy_literal(out_output->fallback_template_key,
                 sizeof(out_output->fallback_template_key),
                 fallback_template_key);
    copy_literal(out_output->selected_template_key,
                 sizeof(out_output->selected_template_key),
                 fallback_template_key);
}

int banana_native_k3h4_resolve_dialogue_template(const BananaK3h4DialogueTemplateInput *input,
                                                  BananaK3h4DialogueTemplateOutput *out_output)
{
    BananaK3h4DialogueTemplateValidationResult validation;

    if (input == NULL || out_output == NULL)
        return -1;

    memset(out_output, 0, sizeof(*out_output));

    out_output->policy_version = BANANA_K3H4_DIALOGUE_TEMPLATE_POLICY_VERSION;
    build_cluster_stack(&input->route_output, out_output);
    resolve_primary_template(&input->route_output, out_output);

    validation.allowed_fact_mask = allowed_fact_mask_for_route(&input->route_output);
    validation.forbidden_fact_mask = forbidden_fact_mask_for_route(&input->route_output);
    validation.unauthorized_fact_mask = input->proposed_fact_mask &
        (~validation.allowed_fact_mask | validation.forbidden_fact_mask);
    validation.has_unauthorized_facts = validation.unauthorized_fact_mask != 0u;

    out_output->fact_validation = validation;

    if (validation.has_unauthorized_facts)
    {
        resolve_fallback(out_output,
                         BANANA_K3H4_DIALOGUE_TEMPLATE_REASON_FACT_VALIDATION,
                         1,
                         "k3h4.template.edda.south_gate.fallback.safe");
        return 0;
    }

    if (input->route_output.resolved_gate != BANANA_K3H4_DIALOGUE_GATE_NONE)
    {
        resolve_fallback(out_output,
                         BANANA_K3H4_DIALOGUE_TEMPLATE_REASON_GATE_BLOCK,
                         1,
                         "k3h4.template.edda.south_gate.fallback.gate");
        return 0;
    }

    if (input->route_output.boundary_state == BANANA_K3H4_DIALOGUE_BOUNDARY_OUTSIDE)
    {
        resolve_fallback(out_output,
                         BANANA_K3H4_DIALOGUE_TEMPLATE_REASON_BOUNDARY_OUTSIDE,
                         2,
                         "k3h4.template.edda.south_gate.fallback.boundary");
        return 0;
    }

    out_output->fallback_applied = 0;
    out_output->fallback_reason = BANANA_K3H4_DIALOGUE_TEMPLATE_REASON_PRIMARY;
    out_output->fallback_rank = 0;
    out_output->fallback_template_key[0] = '\0';

    return 0;
}
