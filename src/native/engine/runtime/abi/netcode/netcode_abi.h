#ifndef BANANA_ENGINE_RUNTIME_NETCODE_ABI_H
#define BANANA_ENGINE_RUNTIME_NETCODE_ABI_H

#include "../../netcode/model/netcode_model.h"
#include "../../netcode/reward/netcode_reward.h"
#include "../../netcode/link/netcode_link.h"
#include "../../netcode/vector/netcode_vector.h"
#include "../../netcode/k3h4/netcode_k3h4_metrics.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Contract constants shared by native, API, and persisted artifacts. */
    #define RUNTIME_K3H4_CONTRACT_VERSION 1
    #define RUNTIME_K3H4_BYTE_ORDER_TAG 0x01020304
    #define RUNTIME_K3H4_BYTE_ORDER_TAG_SWAPPED 0x04030201
    #define RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED 1
    #define RUNTIME_K3H4_ALIGNMENT_BYTES_4 4

    /* Envelope validation result codes surfaced back through API decoders. */
    typedef enum RuntimeK3h4ContractStatus
    {
        RUNTIME_K3H4_CONTRACT_OK = 0,
        RUNTIME_K3H4_CONTRACT_UNSUPPORTED_VERSION = -3001,
        RUNTIME_K3H4_CONTRACT_INVALID_PAYLOAD = -3002,
        RUNTIME_K3H4_CONTRACT_NONFINITE_VALUE = -3003,
        RUNTIME_K3H4_CONTRACT_CRC_MISMATCH = -3004
    } RuntimeK3h4ContractStatus;

    /* Envelope footer appended after the fixed-width k3h4 payload body. */
    typedef struct RuntimeK3h4ContractEnvelopeHeader
    {
        int contract_version;
        int byte_order_tag;
        int payload_bytes;
        int payload_crc32;
    } RuntimeK3h4ContractEnvelopeHeader;

    typedef struct RuntimeK3h4SignalInput
    {
        int call_density;
        int quest_percent;
        int combo_streak;
        int branch_pressure;
        int workflow_depth;
    } RuntimeK3h4SignalInput;

    typedef struct RuntimeK3h4LinkSignalInput
    {
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int interaction_signal;
    } RuntimeK3h4LinkSignalInput;

    typedef struct RuntimeK3h4VectorSignalInput
    {
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int workflow_depth;
        int neural_relevance_score;
        int network_dimensions;
        int model_confidence;
        int policy_momentum;
        int assignment_family;
        int spectral_mode;
        int hardware_byte_order_tag;
        int hardware_dtype_tag;
        int hardware_alignment_bytes;
    } RuntimeK3h4VectorSignalInput;

    /* Compatibility aliases for existing netcode-prefixed declarations. */
    #define RUNTIME_NETCODE_K3H4_CONTRACT_VERSION RUNTIME_K3H4_CONTRACT_VERSION
    #define RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG RUNTIME_K3H4_BYTE_ORDER_TAG
    #define RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG_SWAPPED RUNTIME_K3H4_BYTE_ORDER_TAG_SWAPPED

    typedef RuntimeK3h4ContractStatus RuntimeNetcodeContractStatus;
    typedef RuntimeK3h4ContractEnvelopeHeader RuntimeNetcodeContractEnvelopeHeader;
    typedef RuntimeK3h4SignalInput RuntimeNetcodeSignalInput;
    typedef RuntimeK3h4LinkSignalInput RuntimeNetcodeLinkSignalInput;
    typedef RuntimeK3h4VectorSignalInput RuntimeNetcodeVectorSignalInput;

    #define RUNTIME_NETCODE_CONTRACT_OK RUNTIME_K3H4_CONTRACT_OK
    #define RUNTIME_NETCODE_CONTRACT_UNSUPPORTED_VERSION RUNTIME_K3H4_CONTRACT_UNSUPPORTED_VERSION
    #define RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD RUNTIME_K3H4_CONTRACT_INVALID_PAYLOAD
    #define RUNTIME_NETCODE_CONTRACT_NONFINITE_VALUE RUNTIME_K3H4_CONTRACT_NONFINITE_VALUE
    #define RUNTIME_NETCODE_CONTRACT_CRC_MISMATCH RUNTIME_K3H4_CONTRACT_CRC_MISMATCH

    void runtime_netcode_abi_reset(void);

    void runtime_netcode_abi_record_node_tap(RuntimeNetcodeNode node);

    void runtime_netcode_abi_record_action(RuntimeNetcodeAction action);

    RuntimeNetcodeNode runtime_netcode_abi_get_focus_node(void);

    int runtime_netcode_abi_get_ledger(RuntimeNetcodeInteractionLedger *out_ledger);

    int runtime_netcode_abi_build_learning(RuntimeNetcodeSignalInput signal_input,
                                           RuntimeNetcodeLearningOutput *out_output);

    int runtime_netcode_abi_build_reward(RuntimeNetcodeSignalInput signal_input,
                                         int interaction_signal,
                                         RuntimeNetcodeRewardOutput *out_output);

    int runtime_netcode_abi_build_link(RuntimeNetcodeLinkSignalInput signal_input,
                                       RuntimeNetcodeLinkOutput *out_output);

    int runtime_netcode_abi_build_vector(RuntimeNetcodeVectorSignalInput signal_input,
                                         RuntimeNetcodeVectorOutput *out_output);

    int runtime_netcode_abi_build_k3h4(RuntimeNetcodeVectorSignalInput signal_input,
                                       RuntimeNetcodeK3h4Output *out_output);

    /* Returns the payload size before envelope metadata is appended. */
    int runtime_netcode_abi_k3h4_payload_bytes(void);

    /* Computes the envelope header for a completed k3h4 payload. */
    int runtime_netcode_abi_encode_k3h4_envelope(
        const RuntimeNetcodeK3h4Output *payload,
        RuntimeNetcodeContractEnvelopeHeader *out_header);

    /* Validates version, endianness, payload size, CRC, and finiteness. */
    RuntimeNetcodeContractStatus runtime_netcode_abi_validate_k3h4_envelope(
        const RuntimeNetcodeContractEnvelopeHeader *header,
        const RuntimeNetcodeK3h4Output *payload,
        int allow_byte_swapped_tag);

    /*
     * K3H4 wrapper helpers are preferred call sites.
     * Netcode-prefixed function symbols remain the ABI implementation.
     */

    static inline void runtime_k3h4_abi_reset(void)
    {
        runtime_netcode_abi_reset();
    }

    static inline void runtime_k3h4_abi_record_node_tap(RuntimeNetcodeNode node)
    {
        runtime_netcode_abi_record_node_tap(node);
    }

    static inline void runtime_k3h4_abi_record_action(RuntimeNetcodeAction action)
    {
        runtime_netcode_abi_record_action(action);
    }

    static inline RuntimeNetcodeNode runtime_k3h4_abi_get_focus_node(void)
    {
        return runtime_netcode_abi_get_focus_node();
    }

    static inline int runtime_k3h4_abi_get_ledger(RuntimeNetcodeInteractionLedger *out_ledger)
    {
        return runtime_netcode_abi_get_ledger(out_ledger);
    }

    static inline int runtime_k3h4_abi_build_learning(RuntimeK3h4SignalInput signal_input,
                                                      RuntimeNetcodeLearningOutput *out_output)
    {
        return runtime_netcode_abi_build_learning(signal_input, out_output);
    }

    static inline int runtime_k3h4_abi_build_reward(RuntimeK3h4SignalInput signal_input,
                                                    int interaction_signal,
                                                    RuntimeNetcodeRewardOutput *out_output)
    {
        return runtime_netcode_abi_build_reward(signal_input, interaction_signal, out_output);
    }

    static inline int runtime_k3h4_abi_build_link(RuntimeK3h4LinkSignalInput signal_input,
                                                  RuntimeNetcodeLinkOutput *out_output)
    {
        return runtime_netcode_abi_build_link(signal_input, out_output);
    }

    static inline int runtime_k3h4_abi_build_vector(RuntimeK3h4VectorSignalInput signal_input,
                                                    RuntimeNetcodeVectorOutput *out_output)
    {
        return runtime_netcode_abi_build_vector(signal_input, out_output);
    }

    static inline int runtime_k3h4_abi_build_k3h4(RuntimeK3h4VectorSignalInput signal_input,
                                                  RuntimeNetcodeK3h4Output *out_output)
    {
        return runtime_netcode_abi_build_k3h4(signal_input, out_output);
    }

    static inline int runtime_k3h4_abi_k3h4_payload_bytes(void)
    {
        return runtime_netcode_abi_k3h4_payload_bytes();
    }

    static inline int runtime_k3h4_abi_encode_k3h4_envelope(
        const RuntimeNetcodeK3h4Output *payload,
        RuntimeK3h4ContractEnvelopeHeader *out_header)
    {
        return runtime_netcode_abi_encode_k3h4_envelope(payload, out_header);
    }

    static inline RuntimeK3h4ContractStatus runtime_k3h4_abi_validate_k3h4_envelope(
        const RuntimeK3h4ContractEnvelopeHeader *header,
        const RuntimeNetcodeK3h4Output *payload,
        int allow_byte_swapped_tag)
    {
        return runtime_netcode_abi_validate_k3h4_envelope(header, payload, allow_byte_swapped_tag);
    }

    /*
     * RuntimeNetcodeK3h4Output now carries deterministic K-means
     * sections (centers/radii/scores/spectral/observability) through the ABI
     * bridge. The bridge keeps these fields as plain data pass-through.
     */

#ifdef __cplusplus
}
#endif

#endif
