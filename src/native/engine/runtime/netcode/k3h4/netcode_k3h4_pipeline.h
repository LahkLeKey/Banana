#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_H

#include "netcode_k3h4_metrics.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_netcode_k3h4_pipeline_execute(
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output);

#ifdef __cplusplus
}
#endif

#endif
