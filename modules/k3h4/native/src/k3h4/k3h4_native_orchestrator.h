#ifndef BANANA_NATIVE_K3H4_NATIVE_ORCHESTRATOR_H
#define BANANA_NATIVE_K3H4_NATIVE_ORCHESTRATOR_H

#include "runtime/abi/netcode/netcode_abi.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Thin native facade for the runtime k3h4/netcode builders.
     *
     * These entry points preserve the ABI shape expected by non-native callers
     * while forwarding directly into the runtime netcode implementation.
     * Callers own the output buffers and must pass writable pointers.
     */
    int banana_native_k3h4_build_learning(RuntimeK3h4SignalInput signal_input,
                                          RuntimeNetcodeLearningOutput *out_output);

    /* Builds reward-layer output for a signal frame plus interaction score. */
    int banana_native_k3h4_build_reward(RuntimeK3h4SignalInput signal_input,
                                        int interaction_signal,
                                        RuntimeNetcodeRewardOutput *out_output);

    /* Builds the link graph projection consumed by downstream API layers. */
    int banana_native_k3h4_build_link(RuntimeK3h4LinkSignalInput signal_input,
                                      RuntimeNetcodeLinkOutput *out_output);

    /* Builds the dense vector payload used as the k3h4 input surface. */
    int banana_native_k3h4_build_vector(RuntimeK3h4VectorSignalInput signal_input,
                                        RuntimeNetcodeVectorOutput *out_output);

    /* Builds the full k3h4 projection payload, including envelope metadata. */
    int banana_native_k3h4_build_k3h4(RuntimeK3h4VectorSignalInput signal_input,
                                             RuntimeNetcodeK3h4Output *out_output);

#ifdef __cplusplus
}
#endif

#endif
