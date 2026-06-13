#include "runtime/netcode/hypersphere/netcode_hypersphere_pipeline_internal.h"
#include "runtime/netcode/hypersphere/netcode_hypersphere_pipeline_setup.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-hypersphere-pipeline-setup] %s\n", message);
    return 1;
}

static void seed_valid_vector_output(RuntimeNetcodeVectorOutput *output)
{
    memset(output, 0, sizeof(*output));
    output->dimensions = 6;
    output->kmeans_cluster_count = 99;
}

static void seed_low_cluster_vector_output(RuntimeNetcodeVectorOutput *output)
{
    memset(output, 0, sizeof(*output));
    output->dimensions = 6;
    output->kmeans_cluster_count = -5;
}

int main(void)
{
    RuntimeNetcodeHyperspherePipelineContext context;
    RuntimeNetcodeVectorOutput vector_output;
    RuntimeNetcodeHypersphereOutput hypersphere_output;

    seed_valid_vector_output(&vector_output);

    if (runtime_netcode_hypersphere_initialize_pipeline_context(
            NULL,
            &vector_output,
            &hypersphere_output) == 0)
        return fail("expected NULL context to fail");

    if (runtime_netcode_hypersphere_initialize_pipeline_context(
            &context,
            NULL,
            &hypersphere_output) == 0)
        return fail("expected NULL input to fail");

    if (runtime_netcode_hypersphere_initialize_pipeline_context(
            &context,
            &vector_output,
            NULL) == 0)
        return fail("expected NULL output to fail");

    vector_output.dimensions = 0;
    if (runtime_netcode_hypersphere_initialize_pipeline_context(
            &context,
            &vector_output,
            &hypersphere_output) == 0)
        return fail("expected zero dimensions to fail");

    vector_output.dimensions = RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS + 1;
    if (runtime_netcode_hypersphere_initialize_pipeline_context(
            &context,
            &vector_output,
            &hypersphere_output) == 0)
        return fail("expected oversized dimensions to fail");

    seed_valid_vector_output(&vector_output);
    memset(&hypersphere_output, 0x7F, sizeof(hypersphere_output));
    if (runtime_netcode_hypersphere_initialize_pipeline_context(
            &context,
            &vector_output,
            &hypersphere_output) != 0)
        return fail("expected valid pipeline setup to succeed");

    if (context.dimensions != 6)
        return fail("expected dimensions to be copied");

    if (context.cluster_count != RUNTIME_NETCODE_VECTOR_NODE_COUNT)
        return fail("expected cluster count to be clamped to node count");

    if (context.radius_floor_q16 != 64)
        return fail("expected q16 radius floor default");

    if (context.output != &hypersphere_output)
        return fail("expected output pointer to be bound into context");

    if (hypersphere_output.dimensions != 6)
        return fail("expected output dimensions to be initialized");

    if (hypersphere_output.cluster_count != RUNTIME_NETCODE_VECTOR_NODE_COUNT)
        return fail("expected output cluster count to be initialized with clamped value");

    if (hypersphere_output.vector_count != RUNTIME_NETCODE_VECTOR_NODE_COUNT)
        return fail("expected output vector_count to be initialized");

    if (hypersphere_output.alignment != 0 || hypersphere_output.radial_stability != 0)
        return fail("expected output scalar fields to be zero-initialized");

    seed_low_cluster_vector_output(&vector_output);
    if (runtime_netcode_hypersphere_initialize_pipeline_context(
            &context,
            &vector_output,
            &hypersphere_output) != 0)
        return fail("expected low cluster-count setup to succeed");

    if (context.cluster_count != 1)
        return fail("expected cluster count lower-bound clamp to 1");

    if (hypersphere_output.cluster_count != 1)
        return fail("expected output cluster count lower-bound clamp to 1");

    return 0;
}
