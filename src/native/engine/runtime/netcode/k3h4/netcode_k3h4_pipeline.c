#include "netcode_k3h4_pipeline.h"
#include "netcode_k3h4_cluster_scoring.h"
#include "netcode_k3h4_geometry.h"
#include "netcode_k3h4_observability.h"
#include "netcode_k3h4_pipeline_internal.h"
#include "netcode_k3h4_pipeline_setup.h"

/*
 * Pipeline order matters:
 *   1. setup picks configuration and zeroes output
 *   2. geometry normalizes/project vectors and builds centroid summaries
 *   3. cluster models publish centers plus inverse covariances
 *   4. radii/spectral derive neighborhood structure
 *      r_q16 = max(radius_floor_q16, nearest_neighbor_q16 / 2)
 *   5. weighted scores evaluate every vector/cluster pair
 *      multiplicative: score_q16 ~= d_q16 / r_q16
 *      power:          score_q16 = d_q16^2 - r_q16^2
 *   6. finalizers emit display summaries and deterministic observability
 */
int runtime_netcode_k3h4_pipeline_execute(
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeK3h4Output *out_output)
{
    RuntimeNetcodeK3h4PipelineContext context;

    /* Default execution resolves assignment/spectral configuration from the
     * pipeline setup layer before running the fixed stage sequence. Stage
     * order is intentionally stable so deterministic hash outputs remain
     * reproducible for equivalent inputs and config.
     */
    if (runtime_netcode_k3h4_initialize_pipeline_context(
            &context,
            input,
            out_output) != 0)
        return -1;

    runtime_netcode_k3h4_prepare_geometry(&context);
    runtime_netcode_k3h4_build_cluster_models(&context);
    runtime_netcode_k3h4_build_projection_nodes(&context);
    runtime_netcode_k3h4_build_cluster_radii_and_spectral(&context);
    runtime_netcode_k3h4_build_weighted_voronoi_scores(&context);
    runtime_netcode_k3h4_finalize_geometry(&context);
    runtime_netcode_k3h4_finalize_observability(&context);

    return 0;
}

int runtime_netcode_k3h4_pipeline_execute_with_config(
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeK3h4Output *out_output,
    int assignment_family,
    int spectral_mode)
{
    RuntimeNetcodeK3h4PipelineContext context;

    /* Explicit execution keeps the stage order identical while swapping the
     * two runtime knobs that affect score and spectral semantics, so callers
     * can compare families without changing geometry or payload layout.
     */
    if (runtime_netcode_k3h4_initialize_pipeline_context_with_config(
            &context,
            input,
            out_output,
            assignment_family,
            spectral_mode) != 0)
        return -1;

    runtime_netcode_k3h4_prepare_geometry(&context);
    runtime_netcode_k3h4_build_cluster_models(&context);
    runtime_netcode_k3h4_build_projection_nodes(&context);
    runtime_netcode_k3h4_build_cluster_radii_and_spectral(&context);
    runtime_netcode_k3h4_build_weighted_voronoi_scores(&context);
    runtime_netcode_k3h4_finalize_geometry(&context);
    runtime_netcode_k3h4_finalize_observability(&context);

    return 0;
}
