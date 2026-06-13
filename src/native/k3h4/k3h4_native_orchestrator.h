#ifndef BANANA_NATIVE_K3H4_NATIVE_ORCHESTRATOR_H
#define BANANA_NATIVE_K3H4_NATIVE_ORCHESTRATOR_H

#include "runtime/abi/netcode/netcode_abi.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int banana_native_k3h4_build_learning(RuntimeNetcodeSignalInput signal_input,
                                          RuntimeNetcodeLearningOutput *out_output);

    int banana_native_k3h4_build_reward(RuntimeNetcodeSignalInput signal_input,
                                        int interaction_signal,
                                        RuntimeNetcodeRewardOutput *out_output);

    int banana_native_k3h4_build_link(RuntimeNetcodeLinkSignalInput signal_input,
                                      RuntimeNetcodeLinkOutput *out_output);

    int banana_native_k3h4_build_vector(RuntimeNetcodeVectorSignalInput signal_input,
                                        RuntimeNetcodeVectorOutput *out_output);

    int banana_native_k3h4_build_hypersphere(RuntimeNetcodeVectorSignalInput signal_input,
                                             RuntimeNetcodeHypersphereOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
