#ifndef BANANA_NATIVE_K3H4_METRICS_ORCHESTRATION_LAYER_H
#define BANANA_NATIVE_K3H4_METRICS_ORCHESTRATION_LAYER_H

#include "k3h4_native_orchestrator.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Native orchestration facade for the layered k3h4 metrics stack.
     *
     * These entry points sit above the application/domain/infrastructure
     * adapters and expose the same ABI-friendly shapes used by upstream native
     * callers that want the layered path instead of the direct facade.
     */
    int banana_native_k3h4_layer_build_learning(RuntimeK3h4SignalInput signal_input,
                                                RuntimeNetcodeLearningOutput *out_output);

    /* Layered reward builder for native callers that need interaction scoring. */
    int banana_native_k3h4_layer_build_reward(RuntimeK3h4SignalInput signal_input,
                                              int interaction_signal,
                                              RuntimeNetcodeRewardOutput *out_output);

    /* Layered link builder for native callers that need graph projection output. */
    int banana_native_k3h4_layer_build_link(RuntimeK3h4LinkSignalInput signal_input,
                                            RuntimeNetcodeLinkOutput *out_output);

    /* Layered vector builder feeding the final k3h4 projection stage. */
    int banana_native_k3h4_layer_build_vector(RuntimeK3h4VectorSignalInput signal_input,
                                              RuntimeNetcodeVectorOutput *out_output);

    /* Layered k3h4 projection builder returning the full envelope-bearing payload. */
    int banana_native_k3h4_layer_build_k3h4(RuntimeK3h4VectorSignalInput signal_input,
                                                   RuntimeNetcodeK3h4Output *out_output);

#ifdef __cplusplus
}
#endif

#endif
