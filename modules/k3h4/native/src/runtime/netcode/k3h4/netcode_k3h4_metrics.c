#include "netcode_k3h4_metrics.h"
#include "netcode_k3h4_pipeline.h"

/* Default builder: resolve configuration from the pipeline defaults. */
int runtime_netcode_k3h4_build(const RuntimeNetcodeVectorOutput *input,
                                      RuntimeNetcodeK3h4Output *out_output)
{
    return runtime_netcode_k3h4_pipeline_execute(input, out_output);
}

/* Explicit builder: caller chooses the scoring family and spectral mode. */
int runtime_netcode_k3h4_build_with_config(
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeK3h4Output *out_output,
    int assignment_family,
    int spectral_mode)
{
    return runtime_netcode_k3h4_pipeline_execute_with_config(
        input,
        out_output,
        assignment_family,
        spectral_mode);
}
