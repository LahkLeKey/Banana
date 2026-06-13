#include "k3h4_native_orchestrator.h"

int banana_native_k3h4_build_learning(RuntimeK3h4SignalInput signal_input,
                                      RuntimeNetcodeLearningOutput *out_output)
{
    return runtime_k3h4_abi_build_learning(signal_input, out_output);
}

int banana_native_k3h4_build_reward(RuntimeK3h4SignalInput signal_input,
                                    int interaction_signal,
                                    RuntimeNetcodeRewardOutput *out_output)
{
    return runtime_k3h4_abi_build_reward(signal_input, interaction_signal, out_output);
}

int banana_native_k3h4_build_link(RuntimeK3h4LinkSignalInput signal_input,
                                  RuntimeNetcodeLinkOutput *out_output)
{
    return runtime_k3h4_abi_build_link(signal_input, out_output);
}

int banana_native_k3h4_build_vector(RuntimeK3h4VectorSignalInput signal_input,
                                    RuntimeNetcodeVectorOutput *out_output)
{
    return runtime_k3h4_abi_build_vector(signal_input, out_output);
}

int banana_native_k3h4_build_hypersphere(RuntimeK3h4VectorSignalInput signal_input,
                                         RuntimeNetcodeHypersphereOutput *out_output)
{
    return runtime_k3h4_abi_build_hypersphere(signal_input, out_output);
}
