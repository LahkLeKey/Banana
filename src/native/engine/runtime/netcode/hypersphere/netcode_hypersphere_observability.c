#include "netcode_hypersphere_observability.h"

#include "netcode_hypersphere_cluster_scoring.h"

void runtime_netcode_hypersphere_finalize_observability(
    RuntimeNetcodeHyperspherePipelineContext *context)
{
    context->output->observability.convergence_status = context->input->kmeans_convergence_status;
    context->output->observability.iteration_count = context->input->kmeans_iteration_count;
    context->output->observability.assignment_changes_last_iteration =
        context->input->kmeans_assignment_changes_last_iteration;
    context->output->observability.endianness_decode_path = RUNTIME_NETCODE_ENDIANNESS_LITTLE_ENDIAN;
    context->output->observability.deterministic_hash =
        runtime_netcode_hypersphere_build_deterministic_hash(context->output);
}
