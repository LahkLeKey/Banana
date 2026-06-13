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

int runtime_netcode_abi_hypersphere_payload_bytes(void)
{
    return (int)offsetof(RuntimeNetcodeHypersphereOutput, envelope);
}

int runtime_netcode_abi_encode_hypersphere_envelope(
    const RuntimeNetcodeHypersphereOutput *payload,
    RuntimeNetcodeContractEnvelopeHeader *out_header)
{
    int payload_bytes;

    if (!payload || !out_header)
        return -1;

    payload_bytes = runtime_netcode_abi_hypersphere_payload_bytes();

    out_header->contract_version = RUNTIME_NETCODE_HYPERSPHERE_KMEANS_CONTRACT_VERSION;
    out_header->byte_order_tag = RUNTIME_NETCODE_HYPERSPHERE_KMEANS_BYTE_ORDER_TAG;
    out_header->payload_bytes = payload_bytes;
    out_header->payload_crc32 = (int)crc32_compute((const uint8_t *)payload, (size_t)payload_bytes);
    return 0;
}

static int hypersphere_has_nonfinite(const RuntimeNetcodeHypersphereOutput *payload)
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

RuntimeNetcodeContractStatus runtime_netcode_abi_validate_hypersphere_envelope(
    const RuntimeNetcodeContractEnvelopeHeader *header,
    const RuntimeNetcodeHypersphereOutput *payload,
    int allow_byte_swapped_tag)
{
    int expected_payload_bytes;
    uint32_t expected_crc;

    if (!header || !payload)
        return RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD;

    if (header->contract_version != RUNTIME_NETCODE_HYPERSPHERE_KMEANS_CONTRACT_VERSION)
        return RUNTIME_NETCODE_CONTRACT_UNSUPPORTED_VERSION;

    if (header->byte_order_tag != RUNTIME_NETCODE_HYPERSPHERE_KMEANS_BYTE_ORDER_TAG)
    {
        if (!(allow_byte_swapped_tag &&
              header->byte_order_tag == RUNTIME_NETCODE_HYPERSPHERE_KMEANS_BYTE_ORDER_TAG_SWAPPED))
        {
            return RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD;
        }
    }

    expected_payload_bytes = runtime_netcode_abi_hypersphere_payload_bytes();
    if (header->payload_bytes != expected_payload_bytes)
        return RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD;

    expected_crc = crc32_compute((const uint8_t *)payload, (size_t)header->payload_bytes);
    if ((int)expected_crc != header->payload_crc32)
        return RUNTIME_NETCODE_CONTRACT_CRC_MISMATCH;

    if (hypersphere_has_nonfinite(payload))
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
    RuntimeNetcodeLearningInput input;

    if (!out_output)
        return -1;

    if (runtime_netcode_contract_get_ledger(&input.ledger) != 0)
        return -1;

    input.call_density = signal_input.call_density;
    input.quest_percent = signal_input.quest_percent;
    input.combo_streak = signal_input.combo_streak;
    input.branch_pressure = signal_input.branch_pressure;
    input.workflow_depth = signal_input.workflow_depth;

    return runtime_netcode_model_build(&input, out_output);
}

int runtime_netcode_abi_build_reward(RuntimeNetcodeSignalInput signal_input,
                                     int interaction_signal,
                                     RuntimeNetcodeRewardOutput *out_output)
{
    RuntimeNetcodeRewardInput input;

    if (!out_output)
        return -1;

    input.call_density = signal_input.call_density;
    input.quest_percent = signal_input.quest_percent;
    input.combo_streak = signal_input.combo_streak;
    input.branch_pressure = signal_input.branch_pressure;
    input.workflow_depth = signal_input.workflow_depth;
    input.interaction_signal = interaction_signal;

    return runtime_netcode_reward_build(&input, out_output);
}

int runtime_netcode_abi_build_link(RuntimeNetcodeLinkSignalInput signal_input,
                                   RuntimeNetcodeLinkOutput *out_output)
{
    RuntimeNetcodeLinkInput input;

    if (!out_output)
        return -1;

    input.call_density = signal_input.call_density;
    input.quest_percent = signal_input.quest_percent;
    input.player_level = signal_input.player_level;
    input.combo_streak = signal_input.combo_streak;
    input.branch_pressure = signal_input.branch_pressure;
    input.dependency_pulse = signal_input.dependency_pulse;
    input.interaction_signal = signal_input.interaction_signal;

    return runtime_netcode_link_build(&input, out_output);
}

int runtime_netcode_abi_build_vector(RuntimeNetcodeVectorSignalInput signal_input,
                                     RuntimeNetcodeVectorOutput *out_output)
{
    RuntimeNetcodeVectorInput input;

    if (!out_output)
        return -1;

    input.call_density = signal_input.call_density;
    input.quest_percent = signal_input.quest_percent;
    input.player_level = signal_input.player_level;
    input.combo_streak = signal_input.combo_streak;
    input.branch_pressure = signal_input.branch_pressure;
    input.dependency_pulse = signal_input.dependency_pulse;
    input.workflow_depth = signal_input.workflow_depth;
    input.neural_relevance_score = signal_input.neural_relevance_score;
    input.network_dimensions = signal_input.network_dimensions;
    input.model_confidence = signal_input.model_confidence;
    input.policy_momentum = signal_input.policy_momentum;

    return runtime_netcode_vector_build(&input, out_output);
}

int runtime_netcode_abi_build_hypersphere(RuntimeNetcodeVectorSignalInput signal_input,
                                          RuntimeNetcodeHypersphereOutput *out_output)
{
    RuntimeNetcodeVectorOutput vector_output;

    if (!out_output)
        return -1;

    memset(&vector_output, 0, sizeof(vector_output));
    memset(out_output, 0, sizeof(*out_output));

    if (runtime_netcode_abi_build_vector(signal_input, &vector_output) != 0)
        return -1;

    if (runtime_netcode_hypersphere_build(&vector_output, out_output) != 0)
        return -1;

    if (runtime_netcode_abi_encode_hypersphere_envelope(out_output, (RuntimeNetcodeContractEnvelopeHeader *)&out_output->envelope) != 0)
        return -1;

    out_output->envelope.contract_status = runtime_netcode_abi_validate_hypersphere_envelope(
        (const RuntimeNetcodeContractEnvelopeHeader *)&out_output->envelope,
        out_output,
        1);

    return out_output->envelope.contract_status;
}
