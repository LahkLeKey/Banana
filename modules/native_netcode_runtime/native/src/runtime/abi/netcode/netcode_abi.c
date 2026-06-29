#include "netcode_abi.h"

#include "../../netcode/contract/netcode_contract.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static uint32_t crc32_update(uint32_t crc, uint8_t byte)
{
    int bit;
    crc ^= byte;
    for (bit = 0; bit < 8; bit++)
    {
        uint32_t mask = (uint32_t)-(int32_t)(crc & 1u);
        crc = (crc >> 1) ^ (0xEDB88320u & mask);
    }
    return crc;
}

static uint32_t crc32_compute(const uint8_t *bytes, size_t length)
{
    uint32_t crc = 0xFFFFFFFFu;
    size_t index;

    for (index = 0; index < length; index++)
    {
        crc = crc32_update(crc, bytes[index]);
    }

    return ~crc;
}

int runtime_netcode_abi_k3h4_payload_bytes(void)
{
    /* The payload length stops at the envelope field so CRC32 only covers the
     * deterministic native contract body, not the metadata appended after it.
     */
    return (int)offsetof(RuntimeNetcodeK3h4Output, envelope);
}

int runtime_netcode_abi_encode_k3h4_envelope(
    const RuntimeNetcodeK3h4Output *payload,
    RuntimeNetcodeContractEnvelopeHeader *out_header)
{
    int payload_bytes;

    if (!payload || !out_header)
        return -1;

    payload_bytes = runtime_netcode_abi_k3h4_payload_bytes();

    out_header->contract_version = RUNTIME_NETCODE_K3H4_CONTRACT_VERSION;
    out_header->byte_order_tag = RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG;
    out_header->payload_bytes = payload_bytes;
    out_header->payload_crc32 = (int)crc32_compute((const uint8_t *)payload, (size_t)payload_bytes);
    return 0;
}

static int k3h4_has_nonfinite(const RuntimeNetcodeK3h4Output *payload)
{
    int i;

    for (i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
    {
        if (!isfinite(payload->nodes[i].x) || !isfinite(payload->nodes[i].y) ||
            !isfinite(payload->nodes[i].z) || !isfinite(payload->nodes[i].inradius) ||
            !isfinite(payload->nodes[i].nearest_neighbor_distance))
        {
            return 1;
        }
    }

    return 0;
}

RuntimeNetcodeContractStatus runtime_netcode_abi_validate_k3h4_envelope(
    const RuntimeNetcodeContractEnvelopeHeader *header,
    const RuntimeNetcodeK3h4Output *payload,
    int allow_byte_swapped_tag)
{
    int expected_payload_bytes;
    uint32_t expected_crc;

    if (!header || !payload)
        return RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD;

    if (header->contract_version != RUNTIME_NETCODE_K3H4_CONTRACT_VERSION)
        return RUNTIME_NETCODE_CONTRACT_UNSUPPORTED_VERSION;

    if (header->byte_order_tag != RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG)
    {
        if (!(allow_byte_swapped_tag &&
              header->byte_order_tag == RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG_SWAPPED))
        {
            return RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD;
        }
    }

    expected_payload_bytes = runtime_netcode_abi_k3h4_payload_bytes();
    if (header->payload_bytes != expected_payload_bytes)
        return RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD;

    /* CRC is computed over the exact serialized body bytes, so any change in
     * Q16 centers, radii, scores, or observability trips validation.
     */
    expected_crc = crc32_compute((const uint8_t *)payload, (size_t)header->payload_bytes);
    if ((int)expected_crc != header->payload_crc32)
        return RUNTIME_NETCODE_CONTRACT_CRC_MISMATCH;

    /* Non-finite geometry is rejected after CRC so the validator reports the
     * precise structural error rather than a checksum-only mismatch.
     */
    if (k3h4_has_nonfinite(payload))
        return RUNTIME_NETCODE_CONTRACT_NONFINITE_VALUE;

    return RUNTIME_NETCODE_CONTRACT_OK;
}

void runtime_netcode_abi_reset(void)
{
    runtime_netcode_contract_reset();
}

void runtime_netcode_abi_record_node_tap(RuntimeNetcodeNode node)
{
    runtime_netcode_contract_record_node_tap(node);
}

void runtime_netcode_abi_record_action(RuntimeNetcodeAction action)
{
    runtime_netcode_contract_record_action(action);
}

RuntimeNetcodeNode runtime_netcode_abi_get_focus_node(void)
{
    return runtime_netcode_contract_get_focus_node();
}

int runtime_netcode_abi_get_ledger(RuntimeNetcodeInteractionLedger *out_ledger)
{
    return runtime_netcode_contract_get_ledger(out_ledger);
}

int runtime_netcode_abi_build_learning(RuntimeNetcodeSignalInput signal_input,
                                       RuntimeNetcodeLearningOutput *out_output)
{
    RuntimeNetcodeInteractionLedger ledger;
    RuntimeNetcodeLearningInput learning_input;

    if (!out_output)
        return -1;

    if (runtime_netcode_contract_get_ledger(&ledger) != 0)
        return -1;

    memset(&learning_input, 0, sizeof(learning_input));
    learning_input.ledger = ledger;
    learning_input.call_density = signal_input.call_density;
    learning_input.quest_percent = signal_input.quest_percent;
    learning_input.combo_streak = signal_input.combo_streak;
    learning_input.branch_pressure = signal_input.branch_pressure;
    learning_input.workflow_depth = signal_input.workflow_depth;

    return runtime_netcode_model_build(&learning_input, out_output);
}

int runtime_netcode_abi_build_reward(RuntimeNetcodeSignalInput signal_input,
                                     int interaction_signal,
                                     RuntimeNetcodeRewardOutput *out_output)
{
    RuntimeNetcodeRewardInput reward_input;

    if (!out_output)
        return -1;

    memset(&reward_input, 0, sizeof(reward_input));
    reward_input.call_density = signal_input.call_density;
    reward_input.quest_percent = signal_input.quest_percent;
    reward_input.combo_streak = signal_input.combo_streak;
    reward_input.branch_pressure = signal_input.branch_pressure;
    reward_input.workflow_depth = signal_input.workflow_depth;
    reward_input.interaction_signal = interaction_signal;

    return runtime_netcode_reward_build(&reward_input, out_output);
}

int runtime_netcode_abi_build_link(RuntimeNetcodeLinkSignalInput signal_input,
                                   RuntimeNetcodeLinkOutput *out_output)
{
    RuntimeNetcodeLinkInput link_input;

    if (!out_output)
        return -1;

    memset(&link_input, 0, sizeof(link_input));
    link_input.call_density = signal_input.call_density;
    link_input.quest_percent = signal_input.quest_percent;
    link_input.player_level = signal_input.player_level;
    link_input.combo_streak = signal_input.combo_streak;
    link_input.branch_pressure = signal_input.branch_pressure;
    link_input.dependency_pulse = signal_input.dependency_pulse;
    link_input.interaction_signal = signal_input.interaction_signal;

    return runtime_netcode_link_build(&link_input, out_output);
}

int runtime_netcode_abi_build_vector(RuntimeNetcodeVectorSignalInput signal_input,
                                     RuntimeNetcodeVectorOutput *out_output)
{
    RuntimeNetcodeVectorInput vector_input;

    if (!out_output)
        return -1;

    memset(&vector_input, 0, sizeof(vector_input));
    vector_input.call_density = signal_input.call_density;
    vector_input.quest_percent = signal_input.quest_percent;
    vector_input.player_level = signal_input.player_level;
    vector_input.combo_streak = signal_input.combo_streak;
    vector_input.branch_pressure = signal_input.branch_pressure;
    vector_input.dependency_pulse = signal_input.dependency_pulse;
    vector_input.workflow_depth = signal_input.workflow_depth;
    vector_input.neural_relevance_score = signal_input.neural_relevance_score;
    vector_input.network_dimensions = signal_input.network_dimensions;
    vector_input.model_confidence = signal_input.model_confidence;
    vector_input.policy_momentum = signal_input.policy_momentum;

    return runtime_netcode_vector_build(&vector_input, out_output);
}

int runtime_netcode_abi_build_k3h4(RuntimeNetcodeVectorSignalInput signal_input,
                                   RuntimeNetcodeK3h4Output *out_output)
{
    RuntimeNetcodeVectorInput vector_input;
    RuntimeNetcodeVectorOutput vector_output;

    if (!out_output)
        return -1;

    if (signal_input.hardware_byte_order_tag != RUNTIME_K3H4_BYTE_ORDER_TAG ||
        signal_input.hardware_dtype_tag != RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED ||
        signal_input.hardware_alignment_bytes != RUNTIME_K3H4_ALIGNMENT_BYTES_4)
    {
        memset(out_output, 0, sizeof(*out_output));
        out_output->envelope.contract_version = RUNTIME_NETCODE_K3H4_CONTRACT_VERSION;
        out_output->envelope.byte_order_tag = RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG;
        out_output->envelope.payload_bytes = runtime_netcode_abi_k3h4_payload_bytes();
        out_output->envelope.payload_crc32 = 0;
        out_output->envelope.contract_status = RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD;
        return out_output->envelope.contract_status;
    }

    memset(&vector_input, 0, sizeof(vector_input));
    vector_input.call_density = signal_input.call_density;
    vector_input.quest_percent = signal_input.quest_percent;
    vector_input.player_level = signal_input.player_level;
    vector_input.combo_streak = signal_input.combo_streak;
    vector_input.branch_pressure = signal_input.branch_pressure;
    vector_input.dependency_pulse = signal_input.dependency_pulse;
    vector_input.workflow_depth = signal_input.workflow_depth;
    vector_input.neural_relevance_score = signal_input.neural_relevance_score;
    vector_input.network_dimensions = signal_input.network_dimensions;
    vector_input.model_confidence = signal_input.model_confidence;
    vector_input.policy_momentum = signal_input.policy_momentum;

    memset(&vector_output, 0, sizeof(vector_output));

    if (runtime_netcode_vector_build(&vector_input, &vector_output) != 0)
        return -1;

    /* Build K3H4 metrics from vector output with explicit mode control. */
    if (runtime_netcode_k3h4_build_with_config(
            &vector_output,
            out_output,
            signal_input.assignment_family,
            signal_input.spectral_mode) != 0)
    {
        return -1;
    }

    if (runtime_netcode_abi_encode_k3h4_envelope(out_output, (RuntimeNetcodeContractEnvelopeHeader *)&out_output->envelope) != 0)
        return -1;

    out_output->envelope.contract_status = runtime_netcode_abi_validate_k3h4_envelope(
        (const RuntimeNetcodeContractEnvelopeHeader *)&out_output->envelope,
        out_output,
        1);

    return out_output->envelope.contract_status;
}

