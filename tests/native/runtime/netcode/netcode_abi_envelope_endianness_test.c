#include "runtime/abi/netcode/netcode_abi.h"
#include "../support/test_support.h"

static void test_netcode_abi_envelope_endianness(void **state)
{
    (void)state;
    RuntimeK3h4VectorSignalInput input = {
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
        .assignment_family = RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE,
        .spectral_mode = RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH,
        .hardware_byte_order_tag = RUNTIME_K3H4_BYTE_ORDER_TAG,
        .hardware_dtype_tag = RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED,
        .hardware_alignment_bytes = RUNTIME_K3H4_ALIGNMENT_BYTES_4,
    };
    RuntimeNetcodeK3h4Output output;
    RuntimeK3h4ContractEnvelopeHeader swapped_header;
    RuntimeK3h4ContractStatus status;

    BANANA_TEST_ASSERT_INT_EQ(runtime_k3h4_abi_build_k3h4(input, &output), RUNTIME_K3H4_CONTRACT_OK,
                              "failed to build baseline k3h4 output");
    BANANA_TEST_ASSERT_INT_EQ(output.envelope.contract_status, RUNTIME_K3H4_CONTRACT_OK,
                              "expected runtime envelope status to be OK");

    swapped_header.contract_version = output.envelope.contract_version;
    swapped_header.byte_order_tag = RUNTIME_K3H4_BYTE_ORDER_TAG_SWAPPED;
    swapped_header.payload_bytes = output.envelope.payload_bytes;
    swapped_header.payload_crc32 = output.envelope.payload_crc32;

    status = runtime_k3h4_abi_validate_k3h4_envelope(&swapped_header, &output, 1);
    BANANA_TEST_ASSERT_INT_EQ(status, RUNTIME_K3H4_CONTRACT_OK,
                              "expected byte-swapped tag to validate when allowed");

    status = runtime_k3h4_abi_validate_k3h4_envelope(&swapped_header, &output, 0);
    BANANA_TEST_ASSERT_INT_EQ(status, RUNTIME_K3H4_CONTRACT_INVALID_PAYLOAD,
                              "expected byte-swapped tag to fail when not allowed");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_netcode_abi_envelope_endianness)
)
