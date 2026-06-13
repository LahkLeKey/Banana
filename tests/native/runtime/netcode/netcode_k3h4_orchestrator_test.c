#include "runtime/abi/netcode/netcode_abi.h"
#include "runtime/netcode/k3h4/netcode_k3h4_orchestrator.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "[netcode-k3h4-orchestrator] %s\n", message);
    return 1;
}

static int floats_equal(float lhs, float rhs)
{
    return fabsf(lhs - rhs) < 0.0001f;
}

int main(void)
{
    RuntimeNetcodeSignalInput signal_input;
    RuntimeNetcodeLinkSignalInput link_input;
    RuntimeNetcodeVectorSignalInput vector_input;
    RuntimeNetcodeLearningOutput abi_learning;
    RuntimeNetcodeRewardOutput abi_reward;
    RuntimeNetcodeLinkOutput abi_link;
    RuntimeNetcodeVectorOutput abi_vector;
    RuntimeNetcodeHypersphereOutput abi_hypersphere;
    RuntimeNetcodeInteractionLedger ledger;
    RuntimeNetcodeK3h4Request request;
    RuntimeNetcodeK3h4FullOutput orchestrated;
    int node;
    int dim;
    int cluster;
    int score;

    runtime_netcode_abi_reset();
    runtime_netcode_abi_record_node_tap(RUNTIME_NETCODE_NODE_INTEL);
    runtime_netcode_abi_record_node_tap(RUNTIME_NETCODE_NODE_PLAYER);
    runtime_netcode_abi_record_action(RUNTIME_NETCODE_ACTION_ROUTE);
    runtime_netcode_abi_record_action(RUNTIME_NETCODE_ACTION_TRAIN);

    memset(&signal_input, 0, sizeof(signal_input));
    signal_input.call_density = 44;
    signal_input.quest_percent = 57;
    signal_input.combo_streak = 8;
    signal_input.branch_pressure = 31;
    signal_input.workflow_depth = 4;

    memset(&link_input, 0, sizeof(link_input));
    link_input.call_density = signal_input.call_density;
    link_input.quest_percent = signal_input.quest_percent;
    link_input.player_level = 19;
    link_input.combo_streak = signal_input.combo_streak;
    link_input.branch_pressure = signal_input.branch_pressure;
    link_input.dependency_pulse = 39;
    link_input.interaction_signal = 27;

    memset(&vector_input, 0, sizeof(vector_input));
    vector_input.call_density = signal_input.call_density;
    vector_input.quest_percent = signal_input.quest_percent;
    vector_input.player_level = link_input.player_level;
    vector_input.combo_streak = signal_input.combo_streak;
    vector_input.branch_pressure = signal_input.branch_pressure;
    vector_input.dependency_pulse = link_input.dependency_pulse;
    vector_input.workflow_depth = signal_input.workflow_depth;
    vector_input.neural_relevance_score = 71;
    vector_input.network_dimensions = 10;
    vector_input.model_confidence = 76;
    vector_input.policy_momentum = 63;

    if (runtime_netcode_abi_build_learning(signal_input, &abi_learning) != 0)
        return fail("runtime_netcode_abi_build_learning failed");
    if (runtime_netcode_abi_build_reward(signal_input, link_input.interaction_signal, &abi_reward) != 0)
        return fail("runtime_netcode_abi_build_reward failed");
    if (runtime_netcode_abi_build_link(link_input, &abi_link) != 0)
        return fail("runtime_netcode_abi_build_link failed");
    if (runtime_netcode_abi_build_vector(vector_input, &abi_vector) != 0)
        return fail("runtime_netcode_abi_build_vector failed");
    if (runtime_netcode_abi_build_hypersphere(vector_input, &abi_hypersphere) != 0)
        return fail("runtime_netcode_abi_build_hypersphere failed");

    if (runtime_netcode_abi_get_ledger(&ledger) != 0)
        return fail("runtime_netcode_abi_get_ledger failed");

    memset(&request, 0, sizeof(request));
    request.ledger = ledger;
    request.call_density = vector_input.call_density;
    request.quest_percent = vector_input.quest_percent;
    request.player_level = vector_input.player_level;
    request.combo_streak = vector_input.combo_streak;
    request.branch_pressure = vector_input.branch_pressure;
    request.dependency_pulse = vector_input.dependency_pulse;
    request.workflow_depth = vector_input.workflow_depth;
    request.interaction_signal = link_input.interaction_signal;
    request.neural_relevance_score = vector_input.neural_relevance_score;
    request.network_dimensions = vector_input.network_dimensions;
    request.model_confidence = vector_input.model_confidence;
    request.policy_momentum = vector_input.policy_momentum;

    if (runtime_netcode_k3h4_orchestrate_full(&request, &orchestrated) != 0)
        return fail("runtime_netcode_k3h4_orchestrate_full failed");

    if (abi_learning.model_confidence != orchestrated.learning.model_confidence ||
        abi_learning.training_accuracy != orchestrated.learning.training_accuracy ||
        abi_learning.policy_momentum != orchestrated.learning.policy_momentum ||
        abi_learning.recommended_node != orchestrated.learning.recommended_node ||
        abi_learning.recommended_action != orchestrated.learning.recommended_action)
    {
        return fail("learning output mismatch against orchestrator");
    }

    for (node = 0; node < RUNTIME_NETCODE_NODE_COUNT; node++)
    {
        if (abi_learning.node_weights[node] != orchestrated.learning.node_weights[node])
            return fail("learning node_weights mismatch");
    }
    for (node = 0; node < RUNTIME_NETCODE_ACTION_COUNT; node++)
    {
        if (abi_learning.xp_by_action[node] != orchestrated.learning.xp_by_action[node])
            return fail("learning xp_by_action mismatch");
    }

    if (abi_reward.neural_relevance_score != orchestrated.reward.neural_relevance_score ||
        abi_reward.projected_reward_xp != orchestrated.reward.projected_reward_xp ||
        abi_reward.reward_tier != orchestrated.reward.reward_tier)
    {
        return fail("reward output mismatch against orchestrator");
    }

    if (abi_link.intel != orchestrated.link.intel ||
        abi_link.objectives != orchestrated.link.objectives ||
        abi_link.player != orchestrated.link.player ||
        abi_link.ops != orchestrated.link.ops)
    {
        return fail("link output mismatch against orchestrator");
    }

    if (abi_vector.dimensions != orchestrated.vector.dimensions ||
        abi_vector.k3h4_cluster_count != orchestrated.vector.k3h4_cluster_count ||
        abi_vector.k3h4_iteration_count != orchestrated.vector.k3h4_iteration_count ||
        abi_vector.k3h4_convergence_status != orchestrated.vector.k3h4_convergence_status ||
        abi_vector.k3h4_assignment_changes_last_iteration != orchestrated.vector.k3h4_assignment_changes_last_iteration)
    {
        return fail("vector metadata mismatch against orchestrator");
    }

    for (node = 0; node < RUNTIME_NETCODE_VECTOR_NODE_COUNT; node++)
    {
        if (abi_vector.contract_strength[node] != orchestrated.vector.contract_strength[node] ||
            abi_vector.k3h4_assignments[node] != orchestrated.vector.k3h4_assignments[node] ||
            abi_vector.k3h4_member_counts[node] != orchestrated.vector.k3h4_member_counts[node])
        {
            return fail("vector contract/assignment mismatch");
        }

        for (dim = 0; dim < abi_vector.dimensions; dim++)
        {
            if (!floats_equal(abi_vector.node_vectors[node][dim], orchestrated.vector.node_vectors[node][dim]))
                return fail("vector node_vectors mismatch");
            if (abi_vector.k3h4_centers_q16[node][dim] != orchestrated.vector.k3h4_centers_q16[node][dim])
                return fail("vector k3h4_centers_q16 mismatch");
        }
    }

    if (abi_hypersphere.dimensions != orchestrated.hypersphere.dimensions ||
        abi_hypersphere.alignment != orchestrated.hypersphere.alignment ||
        abi_hypersphere.radial_stability != orchestrated.hypersphere.radial_stability ||
        abi_hypersphere.cluster_count != orchestrated.hypersphere.cluster_count ||
        abi_hypersphere.vector_count != orchestrated.hypersphere.vector_count)
    {
        return fail("hypersphere metadata mismatch against orchestrator");
    }

    for (node = 0; node < RUNTIME_NETCODE_VECTOR_NODE_COUNT; node++)
    {
        if (!floats_equal(abi_hypersphere.nodes[node].x, orchestrated.hypersphere.nodes[node].x) ||
            !floats_equal(abi_hypersphere.nodes[node].y, orchestrated.hypersphere.nodes[node].y) ||
            !floats_equal(abi_hypersphere.nodes[node].z, orchestrated.hypersphere.nodes[node].z) ||
            abi_hypersphere.nodes[node].coherence != orchestrated.hypersphere.nodes[node].coherence ||
            !floats_equal(abi_hypersphere.nodes[node].inradius, orchestrated.hypersphere.nodes[node].inradius) ||
            !floats_equal(abi_hypersphere.nodes[node].nearest_neighbor_distance,
                          orchestrated.hypersphere.nodes[node].nearest_neighbor_distance))
        {
            return fail("hypersphere node mismatch");
        }
    }

    for (cluster = 0; cluster < abi_hypersphere.cluster_count; cluster++)
    {
        if (abi_hypersphere.centers[cluster].cluster_id != orchestrated.hypersphere.centers[cluster].cluster_id ||
            abi_hypersphere.centers[cluster].member_count != orchestrated.hypersphere.centers[cluster].member_count ||
            abi_hypersphere.radii[cluster].cluster_id != orchestrated.hypersphere.radii[cluster].cluster_id ||
            abi_hypersphere.radii[cluster].nearest_neighbor_distance_q16 != orchestrated.hypersphere.radii[cluster].nearest_neighbor_distance_q16 ||
            abi_hypersphere.radii[cluster].inscribed_radius_q16 != orchestrated.hypersphere.radii[cluster].inscribed_radius_q16 ||
            abi_hypersphere.radii[cluster].radius_state != orchestrated.hypersphere.radii[cluster].radius_state ||
            abi_hypersphere.spectral_proxy[cluster].cluster_id != orchestrated.hypersphere.spectral_proxy[cluster].cluster_id ||
            abi_hypersphere.spectral_proxy[cluster].frequency_proxy_q16 != orchestrated.hypersphere.spectral_proxy[cluster].frequency_proxy_q16 ||
            abi_hypersphere.spectral_proxy[cluster].amplitude_proxy_q16 != orchestrated.hypersphere.spectral_proxy[cluster].amplitude_proxy_q16 ||
            abi_hypersphere.spectral_proxy[cluster].spectral_state != orchestrated.hypersphere.spectral_proxy[cluster].spectral_state)
        {
            return fail("hypersphere cluster metrics mismatch");
        }

        for (dim = 0; dim < abi_hypersphere.dimensions; dim++)
        {
            if (abi_hypersphere.centers[cluster].center_q16[dim] !=
                orchestrated.hypersphere.centers[cluster].center_q16[dim])
            {
                return fail("hypersphere center_q16 mismatch");
            }
        }
    }

    for (score = 0; score < abi_hypersphere.vector_count * abi_hypersphere.cluster_count; score++)
    {
        if (abi_hypersphere.weighted_voronoi_scores[score].vector_id !=
                orchestrated.hypersphere.weighted_voronoi_scores[score].vector_id ||
            abi_hypersphere.weighted_voronoi_scores[score].cluster_id !=
                orchestrated.hypersphere.weighted_voronoi_scores[score].cluster_id ||
            abi_hypersphere.weighted_voronoi_scores[score].distance_to_center_q16 !=
                orchestrated.hypersphere.weighted_voronoi_scores[score].distance_to_center_q16 ||
            abi_hypersphere.weighted_voronoi_scores[score].weighted_score_q16 !=
                orchestrated.hypersphere.weighted_voronoi_scores[score].weighted_score_q16 ||
            abi_hypersphere.weighted_voronoi_scores[score].score_validity !=
                orchestrated.hypersphere.weighted_voronoi_scores[score].score_validity)
        {
            return fail("hypersphere weighted_voronoi_scores mismatch");
        }
    }

    if (abi_hypersphere.observability.convergence_status != orchestrated.hypersphere.observability.convergence_status ||
        abi_hypersphere.observability.iteration_count != orchestrated.hypersphere.observability.iteration_count ||
        abi_hypersphere.observability.assignment_changes_last_iteration !=
            orchestrated.hypersphere.observability.assignment_changes_last_iteration ||
        abi_hypersphere.observability.deterministic_hash != orchestrated.hypersphere.observability.deterministic_hash ||
        abi_hypersphere.observability.endianness_decode_path !=
            orchestrated.hypersphere.observability.endianness_decode_path)
    {
        return fail("hypersphere observability mismatch");
    }

    return 0;
}
