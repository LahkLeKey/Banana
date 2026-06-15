#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_H

#include "netcode_k3h4_metrics.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Executes the default geometry/scoring pipeline over vector output. */
    int runtime_netcode_k3h4_pipeline_execute(
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output);

    /* Executes the pipeline with explicit assignment-family and spectral mode. */
    int runtime_netcode_k3h4_pipeline_execute_with_config(
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output,
        int assignment_family,
        int spectral_mode);

#ifdef __cplusplus
}
#endif

#endif
