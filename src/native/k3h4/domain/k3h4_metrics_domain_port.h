#ifndef BANANA_NATIVE_K3H4_DOMAIN_PORT_H
#define BANANA_NATIVE_K3H4_DOMAIN_PORT_H

#include "runtime/abi/netcode/netcode_abi.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Domain port for the native k3h4 metrics application layer.
     *
     * The port carries one function pointer per build stage so the application
     * layer can stay decoupled from the concrete facade or ABI implementation.
     */
    typedef struct BananaNativeK3h4DomainPort
    {
        int (*build_learning)(RuntimeK3h4SignalInput signal_input,
                              RuntimeNetcodeLearningOutput *out_output);
        int (*build_reward)(RuntimeK3h4SignalInput signal_input,
                            int interaction_signal,
                            RuntimeNetcodeRewardOutput *out_output);
        int (*build_link)(RuntimeK3h4LinkSignalInput signal_input,
                          RuntimeNetcodeLinkOutput *out_output);
        int (*build_vector)(RuntimeK3h4VectorSignalInput signal_input,
                            RuntimeNetcodeVectorOutput *out_output);
        int (*build_k3h4)(RuntimeK3h4VectorSignalInput signal_input,
                                 RuntimeNetcodeK3h4Output *out_output);
    } BananaNativeK3h4DomainPort;

#ifdef __cplusplus
}
#endif

#endif
