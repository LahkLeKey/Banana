#include "k3h4_metrics_application_service.h"

int banana_native_k3h4_application_build_learning(
    const BananaNativeK3h4DomainPort *domain_port,
    RuntimeK3h4SignalInput signal_input,
    RuntimeNetcodeLearningOutput *out_output)
{
    if (!domain_port || !domain_port->build_learning)
        return -1;
    return domain_port->build_learning(signal_input, out_output);
}

int banana_native_k3h4_application_build_reward(
    const BananaNativeK3h4DomainPort *domain_port,
    RuntimeK3h4SignalInput signal_input,
    int interaction_signal,
    RuntimeNetcodeRewardOutput *out_output)
{
    if (!domain_port || !domain_port->build_reward)
        return -1;
    return domain_port->build_reward(signal_input, interaction_signal, out_output);
}

int banana_native_k3h4_application_build_link(
    const BananaNativeK3h4DomainPort *domain_port,
    RuntimeK3h4LinkSignalInput signal_input,
    RuntimeNetcodeLinkOutput *out_output)
{
    if (!domain_port || !domain_port->build_link)
        return -1;
    return domain_port->build_link(signal_input, out_output);
}

int banana_native_k3h4_application_build_vector(
    const BananaNativeK3h4DomainPort *domain_port,
    RuntimeK3h4VectorSignalInput signal_input,
    RuntimeNetcodeVectorOutput *out_output)
{
    if (!domain_port || !domain_port->build_vector)
        return -1;
    return domain_port->build_vector(signal_input, out_output);
}

int banana_native_k3h4_application_build_k3h4(
    const BananaNativeK3h4DomainPort *domain_port,
    RuntimeK3h4VectorSignalInput signal_input,
    RuntimeNetcodeK3h4Output *out_output)
{
    if (!domain_port || !domain_port->build_k3h4)
        return -1;
    return domain_port->build_k3h4(signal_input, out_output);
}
