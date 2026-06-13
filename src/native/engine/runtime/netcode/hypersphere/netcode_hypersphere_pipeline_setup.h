#ifndef BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_PIPELINE_SETUP_H
#define BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_PIPELINE_SETUP_H

#include "netcode_hypersphere_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_netcode_hypersphere_initialize_pipeline_context(
        RuntimeNetcodeHyperspherePipelineContext *context,
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeHypersphereOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
