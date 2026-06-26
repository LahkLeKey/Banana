#include "netcode_k3h4_observability.h"

#include "netcode_k3h4_cluster_scoring.h"

void runtime_netcode_k3h4_finalize_observability(
    RuntimeNetcodeK3h4PipelineContext *context)
{
    /*
     * Observability mixes upstream clustering facts from the vector stage
     * (iterations, convergence status, assignment churn) with a downstream hash
     * over the final exported k3h4 payload. The decode path is currently always
     * little-endian because the runtime writes native-order payloads locally.
     *
     * The deterministic hash is computed after geometry/radii/scores are fully
     * materialized, so it fingerprints the exact Q16-valued contract sections
     * consumed by API/frontend parity checks.
     */
    context->output->observability.convergence_status = context->input->k3h4_convergence_status;
    context->output->observability.iteration_count = context->input->k3h4_iteration_count;
    context->output->observability.assignment_changes_last_iteration =
        context->input->k3h4_assignment_changes_last_iteration;
    context->output->observability.endianness_decode_path = RUNTIME_NETCODE_ENDIANNESS_LITTLE_ENDIAN;
    context->output->observability.deterministic_hash =
        runtime_netcode_k3h4_build_deterministic_hash(context->output);
}
