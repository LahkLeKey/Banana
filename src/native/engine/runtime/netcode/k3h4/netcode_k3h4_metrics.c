#include "netcode_k3h4_metrics.h"
#include "netcode_k3h4_pipeline.h"

int runtime_netcode_k3h4_build(const RuntimeNetcodeVectorOutput *input,
                                      RuntimeNetcodeK3h4Output *out_output)
{
    return runtime_netcode_k3h4_pipeline_execute(input, out_output);
}
