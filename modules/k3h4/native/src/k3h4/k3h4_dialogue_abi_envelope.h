#ifndef BANANA_NATIVE_K3H4_DIALOGUE_ABI_ENVELOPE_H
#define BANANA_NATIVE_K3H4_DIALOGUE_ABI_ENVELOPE_H

#include "k3h4_dialogue_spectral_transition_graph.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    enum
    {
        BANANA_K3H4_DIALOGUE_ABI_MAGIC = 0x4b334834u,
        BANANA_K3H4_DIALOGUE_ABI_CONTRACT_VERSION = 1u,
        BANANA_K3H4_DIALOGUE_ABI_BYTE_ORDER_LITTLE_ENDIAN = 1u,
        BANANA_K3H4_DIALOGUE_ABI_BYTE_ORDER_BYTE_SWAPPED = 2u,
        BANANA_K3H4_DIALOGUE_ABI_DECODE_PATH_LITTLE_ENDIAN = 1u,
        BANANA_K3H4_DIALOGUE_ABI_DECODE_PATH_BYTE_SWAPPED = 2u,
    };

    typedef enum BananaK3h4DialogueAbiStatus
    {
        BANANA_K3H4_DIALOGUE_ABI_OK = 0,
        BANANA_K3H4_DIALOGUE_ABI_INVALID_ARGUMENT = -1,
        BANANA_K3H4_DIALOGUE_ABI_BUFFER_TOO_SMALL = -2,
        BANANA_K3H4_DIALOGUE_ABI_CONTRACT_VERSION_MISMATCH = -3001,
        BANANA_K3H4_DIALOGUE_ABI_ENDIANNESS_MISMATCH = -3002,
        BANANA_K3H4_DIALOGUE_ABI_PAYLOAD_SIZE_MISMATCH = -3003,
        BANANA_K3H4_DIALOGUE_ABI_CRC_MISMATCH = -3004,
        BANANA_K3H4_DIALOGUE_ABI_MAGIC_MISMATCH = -3005,
    } BananaK3h4DialogueAbiStatus;

    typedef struct BananaK3h4DialogueAbiTelemetry
    {
        int payload_bytes;
        int deterministic_hash;
        int endianness_decode_path;
    } BananaK3h4DialogueAbiTelemetry;

    typedef struct BananaK3h4DialogueAbiEnvelope
    {
        uint32_t magic;
        uint32_t contract_version;
        uint32_t byte_order_tag;
        uint32_t payload_bytes;
        uint32_t crc32;
        BananaK3h4DialogueAbiTelemetry telemetry;
        BananaK3h4DialogueSpectralTransitionOutput payload;
    } BananaK3h4DialogueAbiEnvelope;

    size_t banana_native_k3h4_dialogue_abi_payload_size(void);

    int banana_native_k3h4_dialogue_abi_encode(const BananaK3h4DialogueSpectralTransitionOutput *payload,
                                                uint8_t *out_buffer,
                                                size_t out_buffer_size,
                                                size_t *out_written_bytes);

    int banana_native_k3h4_dialogue_abi_decode(const uint8_t *buffer,
                                                size_t buffer_size,
                                                BananaK3h4DialogueAbiEnvelope *out_envelope);

#ifdef __cplusplus
}
#endif

#endif
