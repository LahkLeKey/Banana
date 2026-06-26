#include "k3h4/k3h4_dialogue_abi_envelope.h"
#include "k3h4/k3h4_dialogue_cluster_router.h"
#include "k3h4/k3h4_dialogue_spectral_transition_graph.h"

#include "runtime/support/test_support.h"

#include <string.h>

static int build_transition_payload(BananaK3h4DialogueSpectralTransitionOutput *out_payload)
{
    BananaK3h4DialogueRouteInput route_input = {
        .intent = BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS,
        .has_entry_writ = 1,
        .mentions_secret_tunnel = 0,
        .ambiguity_basis_points = 0,
    };
    BananaK3h4DialogueRouteOutput route_output = {0};
    BananaK3h4DialogueSpectralTransitionInput transition_input;

    if (banana_native_k3h4_route_dialogue_cluster(&route_input, &route_output) != 0)
        return -1;

    transition_input.current_state = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE;
    transition_input.requested_state = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_REDIRECT;
    transition_input.route_output = route_output;

    return banana_native_k3h4_resolve_spectral_transition(&transition_input, out_payload);
}

static void test_roundtrip_decode_emits_telemetry_and_payload(void **state)
{
    BananaK3h4DialogueSpectralTransitionOutput payload = {0};
    uint8_t buffer[512];
    size_t written = 0;
    BananaK3h4DialogueAbiEnvelope envelope = {0};

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(build_transition_payload(&payload),
                              0,
                              "transition payload build must succeed");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_abi_encode(&payload, buffer, sizeof(buffer), &written),
        BANANA_K3H4_DIALOGUE_ABI_OK,
        "abi encode must succeed");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_abi_decode(buffer, written, &envelope),
        BANANA_K3H4_DIALOGUE_ABI_OK,
        "abi decode must succeed");

    BANANA_TEST_ASSERT_INT_EQ((int)envelope.contract_version,
                              BANANA_K3H4_DIALOGUE_ABI_CONTRACT_VERSION,
                              "decoded contract version must match");
    BANANA_TEST_ASSERT_INT_EQ(envelope.telemetry.payload_bytes,
                              (int)banana_native_k3h4_dialogue_abi_payload_size(),
                              "telemetry payload bytes must match contract payload size");
    BANANA_TEST_ASSERT_INT_EQ(envelope.telemetry.endianness_decode_path,
                              BANANA_K3H4_DIALOGUE_ABI_DECODE_PATH_LITTLE_ENDIAN,
                              "decode path telemetry must report little-endian path");
    BANANA_TEST_ASSERT_INT_EQ(envelope.payload.next_state,
                              payload.next_state,
                              "decoded payload must preserve transition next_state");
}

static void test_mismatch_rejections_do_not_partially_mutate_output(void **state)
{
    BananaK3h4DialogueSpectralTransitionOutput payload = {0};
    uint8_t buffer[512];
    size_t written = 0;
    BananaK3h4DialogueAbiEnvelope before;
    BananaK3h4DialogueAbiEnvelope after;
    int status;

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(build_transition_payload(&payload),
                              0,
                              "transition payload build must succeed");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_abi_encode(&payload, buffer, sizeof(buffer), &written),
        BANANA_K3H4_DIALOGUE_ABI_OK,
        "abi encode must succeed");

    memset(&before, 0x5a, sizeof(before));
    after = before;

    /* Version mismatch */
    buffer[4] ^= 0x01u;
    status = banana_native_k3h4_dialogue_abi_decode(buffer, written, &after);
    BANANA_TEST_ASSERT_INT_EQ(status,
                              BANANA_K3H4_DIALOGUE_ABI_CONTRACT_VERSION_MISMATCH,
                              "version mismatch must reject payload");
    BANANA_TEST_ASSERT_TRUE(memcmp(&before, &after, sizeof(before)) == 0,
                            "version mismatch must not partially mutate output");
    buffer[4] ^= 0x01u;

    /* Endianness mismatch */
    buffer[8] = 0xffu;
    status = banana_native_k3h4_dialogue_abi_decode(buffer, written, &after);
    BANANA_TEST_ASSERT_INT_EQ(status,
                              BANANA_K3H4_DIALOGUE_ABI_ENDIANNESS_MISMATCH,
                              "endianness mismatch must reject payload");
    BANANA_TEST_ASSERT_TRUE(memcmp(&before, &after, sizeof(before)) == 0,
                            "endianness mismatch must not partially mutate output");
    buffer[8] = (uint8_t)BANANA_K3H4_DIALOGUE_ABI_BYTE_ORDER_LITTLE_ENDIAN;

    /* Payload size mismatch */
    buffer[12] ^= 0x01u;
    status = banana_native_k3h4_dialogue_abi_decode(buffer, written, &after);
    BANANA_TEST_ASSERT_INT_EQ(status,
                              BANANA_K3H4_DIALOGUE_ABI_PAYLOAD_SIZE_MISMATCH,
                              "size mismatch must reject payload");
    BANANA_TEST_ASSERT_TRUE(memcmp(&before, &after, sizeof(before)) == 0,
                            "size mismatch must not partially mutate output");
    buffer[12] ^= 0x01u;

    /* CRC mismatch */
    buffer[20] ^= 0x01u;
    status = banana_native_k3h4_dialogue_abi_decode(buffer, written, &after);
    BANANA_TEST_ASSERT_INT_EQ(status,
                              BANANA_K3H4_DIALOGUE_ABI_CRC_MISMATCH,
                              "crc mismatch must reject payload");
    BANANA_TEST_ASSERT_TRUE(memcmp(&before, &after, sizeof(before)) == 0,
                            "crc mismatch must not partially mutate output");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_roundtrip_decode_emits_telemetry_and_payload),
    BANANA_TEST_CASE(test_mismatch_rejections_do_not_partially_mutate_output))
