#include "netcode_hypersphere.h"
#include "netcode_hypersphere_pipeline.h"

int runtime_netcode_hypersphere_build(const RuntimeNetcodeVectorOutput *input,
                                      RuntimeNetcodeHypersphereOutput *out_output)
{
    return runtime_netcode_hypersphere_pipeline_execute(input, out_output);
}
