#include "runtime/abi/netcode/netcode_abi.h"

#include <stdio.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-abi-endianness] %s\n", message);
    return 1;
}

int main(void)
{
    RuntimeNetcodeVectorSignalInput input = {
        .call_density = 42,
        .quest_percent = 58,
        .player_level = 7,
        .combo_streak = 3,
        .branch_pressure = 21,
        .dependency_pulse = 35,
        .workflow_depth = 2,
        .neural_relevance_score = 63,
        .network_dimensions = 10,
        .model_confidence = 71,
        .policy_momentum = 65,
    };
    RuntimeNetcodeHypersphereOutput output;
    RuntimeNetcodeContractEnvelopeHeader swapped_header;
    RuntimeNetcodeContractStatus status;

    if (runtime_netcode_abi_build_hypersphere(input, &output) != RUNTIME_NETCODE_CONTRACT_OK)
        return fail("failed to build baseline hypersphere output");

    if (output.envelope.contract_status != RUNTIME_NETCODE_CONTRACT_OK)
        return fail("expected runtime envelope status to be OK");

    swapped_header.contract_version = output.envelope.contract_version;
    swapped_header.byte_order_tag = RUNTIME_NETCODE_HYPERSPHERE_KMEANS_BYTE_ORDER_TAG_SWAPPED;
    swapped_header.payload_bytes = output.envelope.payload_bytes;
    swapped_header.payload_crc32 = output.envelope.payload_crc32;

    status = runtime_netcode_abi_validate_hypersphere_envelope(&swapped_header, &output, 1);
    if (status != RUNTIME_NETCODE_CONTRACT_OK)
        return fail("expected byte-swapped tag to validate when allowed");

    status = runtime_netcode_abi_validate_hypersphere_envelope(&swapped_header, &output, 0);
    if (status != RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD)
        return fail("expected byte-swapped tag to fail when not allowed");

    return 0;
}
