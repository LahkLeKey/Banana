#include "k3h4_hypersphere_orchestration_layer.h"

#include "application/k3h4_hypersphere_application_service.h"
#include "infrastructure/k3h4_hypersphere_infrastructure_adapter.h"

static BananaNativeK3h4DomainPort resolve_domain_port(void)
{
    return banana_native_k3h4_infrastructure_create_domain_port();
}

int banana_native_k3h4_layer_build_learning(RuntimeNetcodeSignalInput signal_input,
                                            RuntimeNetcodeLearningOutput *out_output)
{
    BananaNativeK3h4DomainPort domain_port = resolve_domain_port();
    return banana_native_k3h4_application_build_learning(
        &domain_port,
        signal_input,
        out_output);
}

int banana_native_k3h4_layer_build_reward(RuntimeNetcodeSignalInput signal_input,
                                          int interaction_signal,
                                          RuntimeNetcodeRewardOutput *out_output)
{
    BananaNativeK3h4DomainPort domain_port = resolve_domain_port();
    return banana_native_k3h4_application_build_reward(
        &domain_port,
        signal_input,
        interaction_signal,
        out_output);
}

int banana_native_k3h4_layer_build_link(RuntimeNetcodeLinkSignalInput signal_input,
                                        RuntimeNetcodeLinkOutput *out_output)
{
    BananaNativeK3h4DomainPort domain_port = resolve_domain_port();
    return banana_native_k3h4_application_build_link(
        &domain_port,
        signal_input,
        out_output);
}

int banana_native_k3h4_layer_build_vector(RuntimeNetcodeVectorSignalInput signal_input,
                                          RuntimeNetcodeVectorOutput *out_output)
{
    BananaNativeK3h4DomainPort domain_port = resolve_domain_port();
    return banana_native_k3h4_application_build_vector(
        &domain_port,
        signal_input,
        out_output);
}

int banana_native_k3h4_layer_build_hypersphere(RuntimeNetcodeVectorSignalInput signal_input,
                                               RuntimeNetcodeHypersphereOutput *out_output)
{
    BananaNativeK3h4DomainPort domain_port = resolve_domain_port();
    return banana_native_k3h4_application_build_hypersphere(
        &domain_port,
        signal_input,
        out_output);
}
