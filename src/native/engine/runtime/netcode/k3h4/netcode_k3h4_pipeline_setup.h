#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_SETUP_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_SETUP_H

#include "netcode_k3h4_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Initializes pipeline scratch state using environment-default config. */
    int runtime_netcode_k3h4_initialize_pipeline_context(
        RuntimeNetcodeK3h4PipelineContext *context,
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output);

    /* Initializes pipeline scratch state with explicit assignment/spectral config. */
    int runtime_netcode_k3h4_initialize_pipeline_context_with_config(
        RuntimeNetcodeK3h4PipelineContext *context,
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output,
        int assignment_family,
        int spectral_mode);

#ifdef __cplusplus
}
#endif

#endif
