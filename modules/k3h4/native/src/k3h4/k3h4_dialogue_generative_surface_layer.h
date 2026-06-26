#ifndef BANANA_NATIVE_K3H4_DIALOGUE_GENERATIVE_SURFACE_LAYER_H
#define BANANA_NATIVE_K3H4_DIALOGUE_GENERATIVE_SURFACE_LAYER_H

#include "k3h4_dialogue_template_layer.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum BananaK3h4DialogueGeneratorDenyReason
    {
        BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_NONE = 0,
        BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_UNAUTHORIZED_FACT = 1,
        BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_FORBIDDEN_FACT = 2,
        BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_CANON_VIOLATION = 3,
    } BananaK3h4DialogueGeneratorDenyReason;

    typedef struct BananaK3h4DialogueGeneratorInput
    {
        BananaK3h4DialogueTemplateOutput template_output;
        uint32_t selected_fact_mask;
        uint32_t canon_violation_mask;
    } BananaK3h4DialogueGeneratorInput;

    typedef struct BananaK3h4DialogueGeneratorOutput
    {
        int contract_version;
        int selected_cluster_stack[BANANA_K3H4_DIALOGUE_TEMPLATE_STACK_MAX];
        int selected_cluster_stack_count;
        uint32_t validated_fact_mask;
        char selected_template_key[64];
        int fail_closed;
        int status_code;
        BananaK3h4DialogueGeneratorDenyReason deny_reason;
        int deny_code;
        char deny_template_key[64];
    } BananaK3h4DialogueGeneratorOutput;

    int banana_native_k3h4_assemble_dialogue_generator_contract(
        const BananaK3h4DialogueGeneratorInput *input,
        BananaK3h4DialogueGeneratorOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
