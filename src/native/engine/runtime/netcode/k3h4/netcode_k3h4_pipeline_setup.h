#ifndef BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_PIPELINE_SETUP_H
#define BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_PIPELINE_SETUP_H

#include "netcode_k3h4_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_netcode_k3h4_initialize_pipeline_context(
        RuntimeNetcodeHyperspherePipelineContext *context,
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output);

#ifdef __cplusplus
}
#endif

#endif
