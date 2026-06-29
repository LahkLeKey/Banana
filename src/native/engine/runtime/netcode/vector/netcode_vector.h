#ifndef BANANA_ENGINE_RUNTIME_NETCODE_VECTOR_H
#define BANANA_ENGINE_RUNTIME_NETCODE_VECTOR_H

#define RUNTIME_NETCODE_VECTOR_NODE_COUNT 4
#define RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS 16

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeNetcodeK3h4Result RuntimeNetcodeK3h4Result;

    typedef struct RuntimeNetcodeVectorInput
    {
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int workflow_depth;
        int neural_relevance_score;
        int network_dimensions;
        int model_confidence;
        int policy_momentum;
    } RuntimeNetcodeVectorInput;

    typedef struct RuntimeNetcodeVectorOutput
    {
        int dimensions;
        float node_vectors[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
        int contract_strength[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int k3h4_cluster_count;
        int k3h4_iteration_count;
        int k3h4_convergence_status;
        int k3h4_assignment_changes_last_iteration;
        int k3h4_assignments[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int k3h4_member_counts[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int k3h4_centers_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    } RuntimeNetcodeVectorOutput;

    typedef int (*RuntimeNetcodeK3h4ComputeFn)(
        const float vectors[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS],
        int vector_count,
        int dimensions,
        int cluster_count,
        int max_iterations,
        int convergence_threshold_q16,
        RuntimeNetcodeK3h4Result *out_result);

    void runtime_netcode_vector_set_k3h4_compute_hook(RuntimeNetcodeK3h4ComputeFn hook);
    void runtime_netcode_vector_reset_k3h4_compute_hook(void);

    int runtime_netcode_vector_build(const RuntimeNetcodeVectorInput *input,
                                     RuntimeNetcodeVectorOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
