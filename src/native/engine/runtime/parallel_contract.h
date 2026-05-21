#ifndef BANANA_ENGINE_RUNTIME_PARALLEL_CONTRACT_H
#define BANANA_ENGINE_RUNTIME_PARALLEL_CONTRACT_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeParallelBudget
    {
        const char *kernel_id;
        const char *source_path;
        int source_line;
        double serial_fraction_budget;
    } RuntimeParallelBudget;

    const RuntimeParallelBudget *runtime_parallel_contract_entries(int *count);
    int runtime_parallel_contract_expected_inventory_size(void);

#ifdef __cplusplus
}
#endif

#endif
