#ifndef BANANA_NATIVE_K3H4_NATIVE_ORCHESTRATOR_H
#define BANANA_NATIVE_K3H4_NATIVE_ORCHESTRATOR_H

#include "runtime/abi/netcode/netcode_abi.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int banana_native_k3h4_build_learning(RuntimeK3h4SignalInput signal_input,
                                          RuntimeNetcodeLearningOutput *out_output);

    int banana_native_k3h4_build_reward(RuntimeK3h4SignalInput signal_input,
                                        int interaction_signal,
                                        RuntimeNetcodeRewardOutput *out_output);

    int banana_native_k3h4_build_link(RuntimeK3h4LinkSignalInput signal_input,
                                      RuntimeNetcodeLinkOutput *out_output);

    int banana_native_k3h4_build_vector(RuntimeK3h4VectorSignalInput signal_input,
                                        RuntimeNetcodeVectorOutput *out_output);

    int banana_native_k3h4_build_k3h4(RuntimeK3h4VectorSignalInput signal_input,
                                             RuntimeNetcodeK3h4Output *out_output);

#ifdef __cplusplus
}
#endif

#endif
