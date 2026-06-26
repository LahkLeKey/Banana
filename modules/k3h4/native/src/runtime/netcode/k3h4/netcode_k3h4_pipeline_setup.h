#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_SETUP_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_SETUP_H

#include "netcode_k3h4_pipeline_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Initializes pipeline scratch state using environment-default config.
     *
     * This path resolves assignment-family and spectral-mode from runtime
     * environment overrides, then seeds context constants such as the Q16
     * radius floor used by later score/radius calculations.
     */
    int runtime_netcode_k3h4_initialize_pipeline_context(
        RuntimeNetcodeK3h4PipelineContext *context,
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output);

    /*
     * Initializes pipeline scratch state with explicit assignment/spectral config.
     *
     * assignment_family chooses one of two score families:
     *   multiplicative: d_q16 / r_q16
     *   power:          d_q16^2 - r_q16^2
     *
     * spectral_mode controls whether radius-derived affinity proxies are
     * produced or left disabled.
     */
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
