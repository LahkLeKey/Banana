#ifndef BANANA_NATIVE_K3H4_APPLICATION_SERVICE_H
#define BANANA_NATIVE_K3H4_APPLICATION_SERVICE_H

#include "../domain/k3h4_metrics_domain_port.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Application-layer forwarding helpers for the native k3h4 metrics flow.
     * Each function validates the supplied domain port and then delegates the
     * actual stage build to the bound implementation.
     */
    int banana_native_k3h4_application_build_learning(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeK3h4SignalInput signal_input,
        RuntimeNetcodeLearningOutput *out_output);

    int banana_native_k3h4_application_build_reward(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeK3h4SignalInput signal_input,
        int interaction_signal,
        RuntimeNetcodeRewardOutput *out_output);

    int banana_native_k3h4_application_build_link(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeK3h4LinkSignalInput signal_input,
        RuntimeNetcodeLinkOutput *out_output);

    int banana_native_k3h4_application_build_vector(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeK3h4VectorSignalInput signal_input,
        RuntimeNetcodeVectorOutput *out_output);

    int banana_native_k3h4_application_build_k3h4(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeK3h4VectorSignalInput signal_input,
        RuntimeNetcodeK3h4Output *out_output);

#ifdef __cplusplus
}
#endif

#endif
