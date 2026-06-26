#include "k3h4_metrics_orchestration_layer.h"

#include "application/k3h4_metrics_application_service.h"
#include "infrastructure/k3h4_metrics_infrastructure_adapter.h"

/* Resolve the concrete port once per call so the layered path stays explicit. */
static BananaNativeK3h4DomainPort resolve_domain_port(void)
{
    return banana_native_k3h4_infrastructure_create_domain_port();
}

int banana_native_k3h4_layer_build_learning(RuntimeK3h4SignalInput signal_input,
                                            RuntimeNetcodeLearningOutput *out_output)
{
    /* The orchestration layer does no math itself; it wires ABI-shaped inputs
     * into the application/domain path so the architecture mirrors API layers.
     */
    BananaNativeK3h4DomainPort domain_port = resolve_domain_port();
    return banana_native_k3h4_application_build_learning(
        &domain_port,
        signal_input,
        out_output);
}

int banana_native_k3h4_layer_build_reward(RuntimeK3h4SignalInput signal_input,
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

int banana_native_k3h4_layer_build_link(RuntimeK3h4LinkSignalInput signal_input,
                                        RuntimeNetcodeLinkOutput *out_output)
{
    BananaNativeK3h4DomainPort domain_port = resolve_domain_port();
    return banana_native_k3h4_application_build_link(
        &domain_port,
        signal_input,
        out_output);
}

int banana_native_k3h4_layer_build_vector(RuntimeK3h4VectorSignalInput signal_input,
                                          RuntimeNetcodeVectorOutput *out_output)
{
    BananaNativeK3h4DomainPort domain_port = resolve_domain_port();
    return banana_native_k3h4_application_build_vector(
        &domain_port,
        signal_input,
        out_output);
}

int banana_native_k3h4_layer_build_k3h4(RuntimeK3h4VectorSignalInput signal_input,
                                               RuntimeNetcodeK3h4Output *out_output)
{
    BananaNativeK3h4DomainPort domain_port = resolve_domain_port();
    return banana_native_k3h4_application_build_k3h4(
        &domain_port,
        signal_input,
        out_output);
}
