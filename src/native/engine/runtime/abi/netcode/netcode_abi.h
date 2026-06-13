#ifndef BANANA_ENGINE_RUNTIME_NETCODE_ABI_H
#define BANANA_ENGINE_RUNTIME_NETCODE_ABI_H

#include "../../netcode/model/netcode_model.h"
#include "../../netcode/reward/netcode_reward.h"
#include "../../netcode/link/netcode_link.h"
#include "../../netcode/vector/netcode_vector.h"
#include "../../netcode/hypersphere/netcode_hypersphere.h"

#ifdef __cplusplus
extern "C"
{
#endif

    #define RUNTIME_NETCODE_K3H4_CONTRACT_VERSION 1
    #define RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG 0x01020304
    #define RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG_SWAPPED 0x04030201

    typedef enum RuntimeNetcodeContractStatus
    {
        RUNTIME_NETCODE_CONTRACT_OK = 0,
        RUNTIME_NETCODE_CONTRACT_UNSUPPORTED_VERSION = -3001,
        RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD = -3002,
        RUNTIME_NETCODE_CONTRACT_NONFINITE_VALUE = -3003,
        RUNTIME_NETCODE_CONTRACT_CRC_MISMATCH = -3004
    } RuntimeNetcodeContractStatus;

    typedef struct RuntimeNetcodeContractEnvelopeHeader
    {
        int contract_version;
        int byte_order_tag;
        int payload_bytes;
        int payload_crc32;
    } RuntimeNetcodeContractEnvelopeHeader;

    typedef struct RuntimeNetcodeSignalInput
    {
        int call_density;
        int quest_percent;
        int combo_streak;
        int branch_pressure;
        int workflow_depth;
    } RuntimeNetcodeSignalInput;

    typedef struct RuntimeNetcodeLinkSignalInput
    {
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int interaction_signal;
    } RuntimeNetcodeLinkSignalInput;

    typedef struct RuntimeNetcodeVectorSignalInput
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
    } RuntimeNetcodeVectorSignalInput;

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

    int runtime_netcode_abi_build_hypersphere(RuntimeNetcodeVectorSignalInput signal_input,
                                              RuntimeNetcodeHypersphereOutput *out_output);

    int runtime_netcode_abi_hypersphere_payload_bytes(void);

    int runtime_netcode_abi_encode_hypersphere_envelope(
        const RuntimeNetcodeHypersphereOutput *payload,
        RuntimeNetcodeContractEnvelopeHeader *out_header);

    RuntimeNetcodeContractStatus runtime_netcode_abi_validate_hypersphere_envelope(
        const RuntimeNetcodeContractEnvelopeHeader *header,
        const RuntimeNetcodeHypersphereOutput *payload,
        int allow_byte_swapped_tag);

    /*
     * K3H4 ABI aliases keep the public ABI language aligned to the
     * hypersphere contract while preserving existing netcode symbols.
     */
    #define RUNTIME_K3H4_CONTRACT_VERSION RUNTIME_NETCODE_K3H4_CONTRACT_VERSION
    #define RUNTIME_K3H4_BYTE_ORDER_TAG RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG
    #define RUNTIME_K3H4_BYTE_ORDER_TAG_SWAPPED RUNTIME_NETCODE_K3H4_BYTE_ORDER_TAG_SWAPPED

    typedef RuntimeNetcodeContractStatus RuntimeK3h4ContractStatus;
    typedef RuntimeNetcodeContractEnvelopeHeader RuntimeK3h4ContractEnvelopeHeader;
    typedef RuntimeNetcodeSignalInput RuntimeK3h4SignalInput;
    typedef RuntimeNetcodeLinkSignalInput RuntimeK3h4LinkSignalInput;
    typedef RuntimeNetcodeVectorSignalInput RuntimeK3h4VectorSignalInput;

    #define RUNTIME_K3H4_CONTRACT_OK RUNTIME_NETCODE_CONTRACT_OK
    #define RUNTIME_K3H4_CONTRACT_UNSUPPORTED_VERSION RUNTIME_NETCODE_CONTRACT_UNSUPPORTED_VERSION
    #define RUNTIME_K3H4_CONTRACT_INVALID_PAYLOAD RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD
    #define RUNTIME_K3H4_CONTRACT_NONFINITE_VALUE RUNTIME_NETCODE_CONTRACT_NONFINITE_VALUE
    #define RUNTIME_K3H4_CONTRACT_CRC_MISMATCH RUNTIME_NETCODE_CONTRACT_CRC_MISMATCH

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

    static inline int runtime_k3h4_abi_build_hypersphere(RuntimeK3h4VectorSignalInput signal_input,
                                                         RuntimeNetcodeHypersphereOutput *out_output)
    {
        return runtime_netcode_abi_build_hypersphere(signal_input, out_output);
    }

    static inline int runtime_k3h4_abi_hypersphere_payload_bytes(void)
    {
        return runtime_netcode_abi_hypersphere_payload_bytes();
    }

    static inline int runtime_k3h4_abi_encode_hypersphere_envelope(
        const RuntimeNetcodeHypersphereOutput *payload,
        RuntimeK3h4ContractEnvelopeHeader *out_header)
    {
        return runtime_netcode_abi_encode_hypersphere_envelope(payload, out_header);
    }

    static inline RuntimeK3h4ContractStatus runtime_k3h4_abi_validate_hypersphere_envelope(
        const RuntimeK3h4ContractEnvelopeHeader *header,
        const RuntimeNetcodeHypersphereOutput *payload,
        int allow_byte_swapped_tag)
    {
        return runtime_netcode_abi_validate_hypersphere_envelope(header, payload, allow_byte_swapped_tag);
    }

    /*
     * RuntimeNetcodeHypersphereOutput now carries deterministic K-means
     * sections (centers/radii/scores/spectral/observability) through the ABI
     * bridge. The bridge keeps these fields as plain data pass-through.
     */

#ifdef __cplusplus
}
#endif

#endif
