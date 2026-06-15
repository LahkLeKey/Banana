#include "k3h4_native_orchestrator.h"

/*
 * This file is intentionally thin: it keeps the public native k3h4 facade in
 * one place while delegating the actual computation to the runtime ABI layer.
 * That separation lets higher layers depend on stable symbols without caring
 * whether the underlying implementation comes from direct runtime code or a
 * later adapter stack.
 */
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

int banana_native_k3h4_build_k3h4(RuntimeK3h4VectorSignalInput signal_input,
                                         RuntimeNetcodeK3h4Output *out_output)
{
    return runtime_k3h4_abi_build_k3h4(signal_input, out_output);
}
