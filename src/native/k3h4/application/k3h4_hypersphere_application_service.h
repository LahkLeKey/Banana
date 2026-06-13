#ifndef BANANA_NATIVE_K3H4_APPLICATION_SERVICE_H
#define BANANA_NATIVE_K3H4_APPLICATION_SERVICE_H

#include "../domain/k3h4_hypersphere_domain_port.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int banana_native_k3h4_application_build_learning(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeNetcodeSignalInput signal_input,
        RuntimeNetcodeLearningOutput *out_output);

    int banana_native_k3h4_application_build_reward(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeNetcodeSignalInput signal_input,
        int interaction_signal,
        RuntimeNetcodeRewardOutput *out_output);

    int banana_native_k3h4_application_build_link(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeNetcodeLinkSignalInput signal_input,
        RuntimeNetcodeLinkOutput *out_output);

    int banana_native_k3h4_application_build_vector(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeNetcodeVectorSignalInput signal_input,
        RuntimeNetcodeVectorOutput *out_output);

    int banana_native_k3h4_application_build_hypersphere(
        const BananaNativeK3h4DomainPort *domain_port,
        RuntimeNetcodeVectorSignalInput signal_input,
        RuntimeNetcodeHypersphereOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
