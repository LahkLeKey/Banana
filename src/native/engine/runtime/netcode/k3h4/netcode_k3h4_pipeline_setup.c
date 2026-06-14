#include "netcode_k3h4_pipeline_setup.h"

#include <stdlib.h>
#include <string.h>

static int clamp_cluster_count(int value)
{
    if (value < 1) return 1;
    if (value > RUNTIME_NETCODE_VECTOR_NODE_COUNT) return RUNTIME_NETCODE_VECTOR_NODE_COUNT;
    return value;
}

static int resolve_assignment_family(void)
{
    const char *assignment_family = getenv("BANANA_K3H4_ASSIGNMENT_FAMILY");
    if (!assignment_family)
        return RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE;

    if (strcmp(assignment_family, "power") == 0 || strcmp(assignment_family, "POWER") == 0)
        return RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER;

    return RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE;
}

static int normalize_assignment_family(int assignment_family)
{
    if (assignment_family == RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER)
        return RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER;
    return RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE;
}

static int resolve_spectral_mode(void)
{
    const char *spectral_mode = getenv("BANANA_K3H4_SPECTRAL_MODE");
    if (!spectral_mode)
        return RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH;

    if (strcmp(spectral_mode, "affinity-graph") == 0 ||
        strcmp(spectral_mode, "AFFINITY_GRAPH") == 0)
    {
        return RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH;
    }

    return RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED;
}

static int normalize_spectral_mode(int spectral_mode)
{
    if (spectral_mode == RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH)
        return RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH;
    return RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED;
}

int runtime_netcode_k3h4_initialize_pipeline_context_with_config(
    RuntimeNetcodeK3h4PipelineContext *context,
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeK3h4Output *out_output,
    int assignment_family,
    int spectral_mode)
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
    context->cluster_count = clamp_cluster_count(input->k3h4_cluster_count);
    context->radius_floor_q16 = 64;
    context->assignment_family = normalize_assignment_family(assignment_family);
    context->spectral_enabled =
        normalize_spectral_mode(spectral_mode) == RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH;

    context->output->dimensions = context->dimensions;
    context->output->cluster_count = context->cluster_count;
    context->output->vector_count = RUNTIME_NETCODE_VECTOR_NODE_COUNT;

    return 0;
}

int runtime_netcode_k3h4_initialize_pipeline_context(
    RuntimeNetcodeK3h4PipelineContext *context,
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeK3h4Output *out_output)
{
    return runtime_netcode_k3h4_initialize_pipeline_context_with_config(
        context,
        input,
        out_output,
        resolve_assignment_family(),
        resolve_spectral_mode());
}
