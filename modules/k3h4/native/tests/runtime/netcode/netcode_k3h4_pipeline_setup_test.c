#include "runtime/netcode/k3h4/netcode_k3h4_pipeline_internal.h"
#include "runtime/netcode/k3h4/netcode_k3h4_pipeline_setup.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-k3h4-pipeline-setup] %s\n", message);
    return 1;
}

static void seed_valid_vector_output(RuntimeNetcodeVectorOutput *output)
{
    memset(output, 0, sizeof(*output));
    output->dimensions = 6;
    /* Deliberately overspecify cluster count to verify upper-bound clamping. */
    output->k3h4_cluster_count = 99;
}

static void seed_low_cluster_vector_output(RuntimeNetcodeVectorOutput *output)
{
    memset(output, 0, sizeof(*output));
    output->dimensions = 6;
    /* Negative values should collapse to the minimum supported cluster count. */
    output->k3h4_cluster_count = -5;
}

int main(void)
{
    RuntimeNetcodeK3h4PipelineContext context;
    RuntimeNetcodeVectorOutput vector_output;
    RuntimeNetcodeK3h4Output k3h4_output;

    seed_valid_vector_output(&vector_output);

    if (runtime_netcode_k3h4_initialize_pipeline_context(
            NULL,
            &vector_output,
            &k3h4_output) == 0)
        return fail("expected NULL context to fail");

    if (runtime_netcode_k3h4_initialize_pipeline_context(
            &context,
            NULL,
            &k3h4_output) == 0)
        return fail("expected NULL input to fail");

    if (runtime_netcode_k3h4_initialize_pipeline_context(
            &context,
            &vector_output,
            NULL) == 0)
        return fail("expected NULL output to fail");

    vector_output.dimensions = 0;
    if (runtime_netcode_k3h4_initialize_pipeline_context(
            &context,
            &vector_output,
            &k3h4_output) == 0)
        return fail("expected zero dimensions to fail");

    vector_output.dimensions = RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS + 1;
    if (runtime_netcode_k3h4_initialize_pipeline_context(
            &context,
            &vector_output,
            &k3h4_output) == 0)
        return fail("expected oversized dimensions to fail");

    seed_valid_vector_output(&vector_output);
    memset(&k3h4_output, 0x7F, sizeof(k3h4_output));
    if (runtime_netcode_k3h4_initialize_pipeline_context(
            &context,
            &vector_output,
            &k3h4_output) != 0)
        return fail("expected valid pipeline setup to succeed");

    if (context.dimensions != 6)
        return fail("expected dimensions to be copied");

    if (context.cluster_count != RUNTIME_NETCODE_VECTOR_NODE_COUNT)
        return fail("expected cluster count to be clamped to node count");

    /* 64 Q16 units is the radius floor that prevents divide-by-near-zero
     * score blowups in multiplicative mode.
     */
    if (context.radius_floor_q16 != 64)
        return fail("expected q16 radius floor default");

    if (context.output != &k3h4_output)
        return fail("expected output pointer to be bound into context");

    if (k3h4_output.dimensions != 6)
        return fail("expected output dimensions to be initialized");

    if (k3h4_output.cluster_count != RUNTIME_NETCODE_VECTOR_NODE_COUNT)
        return fail("expected output cluster count to be initialized with clamped value");

    if (k3h4_output.vector_count != RUNTIME_NETCODE_VECTOR_NODE_COUNT)
        return fail("expected output vector_count to be initialized");

    if (k3h4_output.alignment != 0 || k3h4_output.radial_stability != 0)
        return fail("expected output scalar fields to be zero-initialized");

    seed_low_cluster_vector_output(&vector_output);
    if (runtime_netcode_k3h4_initialize_pipeline_context(
            &context,
            &vector_output,
            &k3h4_output) != 0)
        return fail("expected low cluster-count setup to succeed");

    if (context.cluster_count != 1)
        return fail("expected cluster count lower-bound clamp to 1");

    if (k3h4_output.cluster_count != 1)
        return fail("expected output cluster count lower-bound clamp to 1");

    return 0;
}
