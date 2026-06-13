#ifndef BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_PIPELINE_H
#define BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_PIPELINE_H

#include "netcode_hypersphere.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_netcode_hypersphere_pipeline_execute(
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeHypersphereOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
