#include "runtime/abi/netcode/netcode_abi.h"

#include <stdio.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-abi-error-contract] %s\n", message);
    return 1;
}

int main(void)
{
    RuntimeK3h4VectorSignalInput input = {
        .call_density = 33,
        .quest_percent = 67,
        .player_level = 4,
        .combo_streak = 2,
        .branch_pressure = 18,
        .dependency_pulse = 29,
        .workflow_depth = 1,
        .neural_relevance_score = 55,
        .network_dimensions = 8,
        .model_confidence = 69,
        .policy_momentum = 61,
    };
    RuntimeNetcodeK3h4Output output;
    RuntimeK3h4ContractEnvelopeHeader header;
    RuntimeK3h4ContractStatus status;

    if (runtime_k3h4_abi_build_k3h4(input, &output) != RUNTIME_K3H4_CONTRACT_OK)
        return fail("failed to build baseline k3h4 output");

    header.contract_version = 99;
    header.byte_order_tag = output.envelope.byte_order_tag;
    header.payload_bytes = output.envelope.payload_bytes;
    header.payload_crc32 = output.envelope.payload_crc32;
    status = runtime_k3h4_abi_validate_k3h4_envelope(&header, &output, 1);
    if (status != RUNTIME_K3H4_CONTRACT_UNSUPPORTED_VERSION)
        return fail("expected unsupported version contract error");

    header.contract_version = output.envelope.contract_version;
    header.payload_bytes = runtime_k3h4_abi_k3h4_payload_bytes() + 4;
    status = runtime_k3h4_abi_validate_k3h4_envelope(&header, &output, 1);
    if (status != RUNTIME_K3H4_CONTRACT_INVALID_PAYLOAD)
        return fail("expected invalid payload contract error for malformed payload length");

    header.payload_bytes = output.envelope.payload_bytes;
    header.payload_crc32 = output.envelope.payload_crc32 ^ 0x00FF00FF;
    status = runtime_k3h4_abi_validate_k3h4_envelope(&header, &output, 1);
    if (status != RUNTIME_K3H4_CONTRACT_CRC_MISMATCH)
        return fail("expected CRC mismatch contract error");

    return 0;
}
