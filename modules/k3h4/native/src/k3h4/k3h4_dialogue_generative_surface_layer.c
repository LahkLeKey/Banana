#include "k3h4_dialogue_generative_surface_layer.h"

#include <stddef.h>
#include <string.h>

enum
{
    BANANA_K3H4_DIALOGUE_GENERATOR_CONTRACT_VERSION = 1,
    BANANA_K3H4_DIALOGUE_GENERATOR_STATUS_OK = 0,
    BANANA_K3H4_DIALOGUE_GENERATOR_STATUS_VALIDATION_FAILED = 2,
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

static int build_deny_code(BananaK3h4DialogueGeneratorDenyReason reason,
                           const BananaK3h4DialogueGeneratorOutput *output)
{
    uint32_t hash = 2166136261u;
    const uint32_t fields[] = {
        (uint32_t)reason,
        (uint32_t)output->selected_cluster_stack_count,
        (uint32_t)output->selected_cluster_stack[0],
        output->validated_fact_mask,
    };
    size_t index;

    for (index = 0; index < sizeof(fields) / sizeof(fields[0]); ++index)
    {
        hash ^= fields[index];
        hash *= 16777619u;
    }

    return (int)(hash & 0x7fffffff);
}

static void populate_base_output(const BananaK3h4DialogueGeneratorInput *input,
                                 BananaK3h4DialogueGeneratorOutput *out_output)
{
    memset(out_output, 0, sizeof(*out_output));

    out_output->contract_version = BANANA_K3H4_DIALOGUE_GENERATOR_CONTRACT_VERSION;
    out_output->selected_cluster_stack_count = input->template_output.selected_cluster_stack_count;
    memcpy(out_output->selected_cluster_stack,
           input->template_output.selected_cluster_stack,
           sizeof(out_output->selected_cluster_stack));

    out_output->validated_fact_mask = input->selected_fact_mask &
                                      input->template_output.fact_validation.allowed_fact_mask &
                                      ~input->template_output.fact_validation.forbidden_fact_mask;

    copy_literal(out_output->selected_template_key,
                 sizeof(out_output->selected_template_key),
                 input->template_output.selected_template_key);

    out_output->fail_closed = 0;
    out_output->status_code = BANANA_K3H4_DIALOGUE_GENERATOR_STATUS_OK;
    out_output->deny_reason = BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_NONE;
    out_output->deny_code = 0;
    out_output->deny_template_key[0] = '\0';
}

static int validate_contract(const BananaK3h4DialogueGeneratorInput *input,
                             BananaK3h4DialogueGeneratorOutput *out_output)
{
    uint32_t unauthorized_from_template = input->template_output.fact_validation.unauthorized_fact_mask;
    uint32_t selected_forbidden = input->selected_fact_mask & input->template_output.fact_validation.forbidden_fact_mask;
    BananaK3h4DialogueGeneratorDenyReason deny_reason = BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_NONE;

    if (unauthorized_from_template != 0u)
        deny_reason = BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_UNAUTHORIZED_FACT;
    else if (selected_forbidden != 0u)
        deny_reason = BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_FORBIDDEN_FACT;
    else if (input->canon_violation_mask != 0u)
        deny_reason = BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_CANON_VIOLATION;

    if (deny_reason == BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_NONE)
        return 0;

    out_output->fail_closed = 1;
    out_output->status_code = BANANA_K3H4_DIALOGUE_GENERATOR_STATUS_VALIDATION_FAILED;
    out_output->deny_reason = deny_reason;
    copy_literal(out_output->deny_template_key,
                 sizeof(out_output->deny_template_key),
                 "k3h4.template.edda.south_gate.deny.fail_closed");
    out_output->deny_code = build_deny_code(deny_reason, out_output);
    copy_literal(out_output->selected_template_key,
                 sizeof(out_output->selected_template_key),
                 out_output->deny_template_key);

    return -BANANA_K3H4_DIALOGUE_GENERATOR_STATUS_VALIDATION_FAILED;
}

int banana_native_k3h4_assemble_dialogue_generator_contract(
    const BananaK3h4DialogueGeneratorInput *input,
    BananaK3h4DialogueGeneratorOutput *out_output)
{
    if (input == NULL || out_output == NULL)
        return -1;

    populate_base_output(input, out_output);
    return validate_contract(input, out_output);
}
