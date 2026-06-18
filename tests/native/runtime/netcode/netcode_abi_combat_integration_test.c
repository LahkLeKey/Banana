#include "runtime/abi/netcode/netcode_abi.h"

#include <stdio.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-abi-combat-integration] %s\n", message);
    return 1;
}

int main(void)
{
    RuntimeK3h4VectorSignalInput input = {
        .call_density = 42,
        .quest_percent = 64,
        .player_level = 5,
        .combo_streak = 1,
        .branch_pressure = 27,
        .dependency_pulse = 3,
        .workflow_depth = 2,
        .neural_relevance_score = 18,
        .network_dimensions = 3,
        .model_confidence = 74,
        .policy_momentum = 9,
        .assignment_family = RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE,
        .spectral_mode = RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED,
        .hardware_byte_order_tag = RUNTIME_K3H4_BYTE_ORDER_TAG,
        .hardware_dtype_tag = RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED,
        .hardware_alignment_bytes = RUNTIME_K3H4_ALIGNMENT_BYTES_4,
    };
    RuntimeNetcodeK3h4Output output;
    RuntimeK3h4ContractEnvelopeHeader header;
    RuntimeK3h4ContractStatus status;

    if (runtime_k3h4_abi_build_k3h4(input, &output) != RUNTIME_K3H4_CONTRACT_OK)
        return fail("failed to build combat-oriented k3h4 payload");

    if (output.envelope.contract_status != RUNTIME_K3H4_CONTRACT_OK)
        return fail("combat-oriented payload should validate as OK");

    if (output.envelope.payload_bytes != runtime_k3h4_abi_k3h4_payload_bytes())
        return fail("envelope payload bytes should match ABI contract");

    if (runtime_k3h4_abi_encode_k3h4_envelope(&output, &header) != 0)
        return fail("failed to encode envelope metadata");

    status = runtime_k3h4_abi_validate_k3h4_envelope(&header, &output, 1);
    if (status != RUNTIME_K3H4_CONTRACT_OK)
        return fail("encoded envelope failed ABI validation");

    input.hardware_alignment_bytes = 8;
    if (runtime_k3h4_abi_build_k3h4(input, &output) != RUNTIME_K3H4_CONTRACT_INVALID_PAYLOAD)
        return fail("invalid ABI hardware layout should fail contract preflight");

    return 0;
}
