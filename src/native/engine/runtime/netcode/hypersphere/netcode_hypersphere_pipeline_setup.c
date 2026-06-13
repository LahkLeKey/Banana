#include "netcode_hypersphere_pipeline_setup.h"

#include <string.h>

static int clamp_cluster_count(int value)
{
    if (value < 1) return 1;
    if (value > RUNTIME_NETCODE_VECTOR_NODE_COUNT) return RUNTIME_NETCODE_VECTOR_NODE_COUNT;
    return value;
}

int runtime_netcode_hypersphere_initialize_pipeline_context(
    RuntimeNetcodeHyperspherePipelineContext *context,
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeHypersphereOutput *out_output)
{
    if (!context || !input || !out_output)
        return -1;

    if (input->dimensions <= 0 || input->dimensions > RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS)
        return -1;

    memset(context, 0, sizeof(*context));
    memset(out_output, 0, sizeof(*out_output));

    context->input = input;
    context->output = out_output;
    context->dimensions = input->dimensions;
    context->cluster_count = clamp_cluster_count(input->kmeans_cluster_count);
    context->radius_floor_q16 = 64;

    context->output->dimensions = context->dimensions;
    context->output->cluster_count = context->cluster_count;
    context->output->vector_count = RUNTIME_NETCODE_VECTOR_NODE_COUNT;

    return 0;
}
