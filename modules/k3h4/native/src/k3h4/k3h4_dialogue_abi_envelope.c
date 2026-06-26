#include "k3h4_dialogue_abi_envelope.h"

#include <string.h>

enum
{
    BANANA_K3H4_DIALOGUE_ABI_HEADER_BYTES = 20,
};

static uint32_t crc32_iso3309(const uint8_t *bytes, size_t length)
{
    uint32_t crc = 0xffffffffu;
    size_t index;

    for (index = 0; index < length; ++index)
    {
        uint32_t value = (crc ^ bytes[index]) & 0xffu;
        int bit;

        for (bit = 0; bit < 8; ++bit)
        {
            if ((value & 1u) != 0u)
                value = (value >> 1) ^ 0xedb88320u;
            else
                value >>= 1;
        }

        crc = (crc >> 8) ^ value;
    }

    return crc ^ 0xffffffffu;
}

static uint32_t crc32_update(uint32_t crc, const uint8_t *bytes, size_t length)
{
    size_t index;

    for (index = 0; index < length; ++index)
    {
        uint32_t value = (crc ^ bytes[index]) & 0xffu;
        int bit;

        for (bit = 0; bit < 8; ++bit)
        {
            if ((value & 1u) != 0u)
                value = (value >> 1) ^ 0xedb88320u;
            else
                value >>= 1;
        }

        crc = (crc >> 8) ^ value;
    }

    return crc;
}

static uint32_t crc32_envelope_without_crc(const uint8_t *buffer, size_t payload_bytes)
{
    uint32_t crc = 0xffffffffu;

    crc = crc32_update(crc, buffer, 16);
    crc = crc32_update(crc, buffer + BANANA_K3H4_DIALOGUE_ABI_HEADER_BYTES, payload_bytes);
    return crc ^ 0xffffffffu;
}

static uint32_t deterministic_hash_from_payload(const BananaK3h4DialogueSpectralTransitionOutput *payload)
{
    uint32_t hash = 2166136261u;
    const uint32_t fields[] = {
        (uint32_t)payload->graph_version,
        (uint32_t)payload->current_state,
        (uint32_t)payload->next_state,
        (uint32_t)payload->candidate_count,
        (uint32_t)payload->transition_allowed,
        (uint32_t)payload->deny_reason,
        (uint32_t)payload->deny_reason_code,
    };
    size_t index;

    for (index = 0; index < sizeof(fields) / sizeof(fields[0]); ++index)
    {
        hash ^= fields[index];
        hash *= 16777619u;
    }

    return hash;
}

static void write_u32_le(uint8_t *target, uint32_t value)
{
    target[0] = (uint8_t)(value & 0xffu);
    target[1] = (uint8_t)((value >> 8) & 0xffu);
    target[2] = (uint8_t)((value >> 16) & 0xffu);
    target[3] = (uint8_t)((value >> 24) & 0xffu);
}

static uint32_t read_u32_le(const uint8_t *source)
{
    return (uint32_t)source[0] |
           ((uint32_t)source[1] << 8) |
           ((uint32_t)source[2] << 16) |
           ((uint32_t)source[3] << 24);
}

size_t banana_native_k3h4_dialogue_abi_payload_size(void)
{
    return sizeof(BananaK3h4DialogueSpectralTransitionOutput);
}

int banana_native_k3h4_dialogue_abi_encode(const BananaK3h4DialogueSpectralTransitionOutput *payload,
                                            uint8_t *out_buffer,
                                            size_t out_buffer_size,
                                            size_t *out_written_bytes)
{
    size_t payload_bytes = banana_native_k3h4_dialogue_abi_payload_size();
    size_t required = BANANA_K3H4_DIALOGUE_ABI_HEADER_BYTES + payload_bytes;
    uint32_t crc;

    if (payload == NULL || out_buffer == NULL || out_written_bytes == NULL)
        return BANANA_K3H4_DIALOGUE_ABI_INVALID_ARGUMENT;

    if (out_buffer_size < required)
        return BANANA_K3H4_DIALOGUE_ABI_BUFFER_TOO_SMALL;

    memset(out_buffer, 0, required);

    write_u32_le(out_buffer + 0, BANANA_K3H4_DIALOGUE_ABI_MAGIC);
    write_u32_le(out_buffer + 4, BANANA_K3H4_DIALOGUE_ABI_CONTRACT_VERSION);
    write_u32_le(out_buffer + 8, BANANA_K3H4_DIALOGUE_ABI_BYTE_ORDER_LITTLE_ENDIAN);
    write_u32_le(out_buffer + 12, (uint32_t)payload_bytes);

    memcpy(out_buffer + BANANA_K3H4_DIALOGUE_ABI_HEADER_BYTES, payload, payload_bytes);

    crc = crc32_envelope_without_crc(out_buffer, payload_bytes);
    write_u32_le(out_buffer + 16, crc);

    *out_written_bytes = required;
    return BANANA_K3H4_DIALOGUE_ABI_OK;
}

int banana_native_k3h4_dialogue_abi_decode(const uint8_t *buffer,
                                            size_t buffer_size,
                                            BananaK3h4DialogueAbiEnvelope *out_envelope)
{
    size_t payload_bytes = banana_native_k3h4_dialogue_abi_payload_size();
    size_t required = BANANA_K3H4_DIALOGUE_ABI_HEADER_BYTES + payload_bytes;
    BananaK3h4DialogueAbiEnvelope parsed;
    uint32_t expected_crc;

    if (buffer == NULL || out_envelope == NULL)
        return BANANA_K3H4_DIALOGUE_ABI_INVALID_ARGUMENT;

    if (buffer_size < required)
        return BANANA_K3H4_DIALOGUE_ABI_BUFFER_TOO_SMALL;

    memset(&parsed, 0, sizeof(parsed));

    parsed.magic = read_u32_le(buffer + 0);
    parsed.contract_version = read_u32_le(buffer + 4);
    parsed.byte_order_tag = read_u32_le(buffer + 8);
    parsed.payload_bytes = read_u32_le(buffer + 12);
    parsed.crc32 = read_u32_le(buffer + 16);

    if (parsed.magic != BANANA_K3H4_DIALOGUE_ABI_MAGIC)
        return BANANA_K3H4_DIALOGUE_ABI_MAGIC_MISMATCH;

    if (parsed.contract_version != BANANA_K3H4_DIALOGUE_ABI_CONTRACT_VERSION)
        return BANANA_K3H4_DIALOGUE_ABI_CONTRACT_VERSION_MISMATCH;

    if (parsed.byte_order_tag != BANANA_K3H4_DIALOGUE_ABI_BYTE_ORDER_LITTLE_ENDIAN)
        return BANANA_K3H4_DIALOGUE_ABI_ENDIANNESS_MISMATCH;

    if (parsed.payload_bytes != payload_bytes)
        return BANANA_K3H4_DIALOGUE_ABI_PAYLOAD_SIZE_MISMATCH;

    expected_crc = crc32_envelope_without_crc(buffer, payload_bytes);
    if (expected_crc != parsed.crc32)
        return BANANA_K3H4_DIALOGUE_ABI_CRC_MISMATCH;

    memcpy(&parsed.payload,
           buffer + BANANA_K3H4_DIALOGUE_ABI_HEADER_BYTES,
           payload_bytes);
    parsed.telemetry.payload_bytes = (int)parsed.payload_bytes;
    parsed.telemetry.deterministic_hash = (int)deterministic_hash_from_payload(&parsed.payload);
    parsed.telemetry.endianness_decode_path = BANANA_K3H4_DIALOGUE_ABI_DECODE_PATH_LITTLE_ENDIAN;

    *out_envelope = parsed;
    return BANANA_K3H4_DIALOGUE_ABI_OK;
}
