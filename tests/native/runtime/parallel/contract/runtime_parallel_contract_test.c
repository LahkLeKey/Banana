#include "runtime/parallel/contract/parallel_contract.h"
#include "runtime/parallel/model/parallel_model.h"

#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    const RuntimeParallelBudget *entries = NULL;
    const int expected_inventory = runtime_parallel_contract_expected_inventory_size();
    const int workers_p4 = 4;
    const int workers_p8 = 8;
    const double min_speedup_p4 = 1.15;
    const double min_speedup_p8 = 1.25;
    const double min_efficiency_p8 = 0.15;
    double sum_speedup_p8 = 0.0;
    int count = 0;
    int i = 0;

    entries = runtime_parallel_contract_entries(&count);
    if (!expect_true("contract entries", entries != NULL))
        return 1;
    if (!expect_true("inventory size stable", count == expected_inventory))
        return 1;
    if (!expect_true("inventory includes all parallel kernels", count == 21))
        return 1;

    for (i = 0; i < count; ++i)
    {
        const RuntimeParallelBudget *entry = &entries[i];
        const double speedup_p4 = runtime_parallel_amdahl_speedup(entry->serial_fraction_budget, workers_p4);
        const double speedup_p8 = runtime_parallel_amdahl_speedup(entry->serial_fraction_budget, workers_p8);
        const double efficiency_p8 = runtime_parallel_amdahl_efficiency(entry->serial_fraction_budget, workers_p8);

        if (!expect_true("kernel id present", entry->kernel_id != NULL && entry->kernel_id[0] != '\0'))
            return 1;
        if (!expect_true("source path present", entry->source_path != NULL && entry->source_path[0] != '\0'))
            return 1;
        if (!expect_true("source line valid", entry->source_line > 0))
            return 1;
        if (!expect_true("serial budget in range", entry->serial_fraction_budget >= 0.0 && entry->serial_fraction_budget < 1.0))
            return 1;
        if (!expect_true("speedup improves from p4", speedup_p4 >= min_speedup_p4))
            return 1;
        if (!expect_true("speedup improves from p8", speedup_p8 >= min_speedup_p8))
            return 1;
        if (!expect_true("speedup monotonic", speedup_p8 >= speedup_p4))
            return 1;
        if (!expect_true("efficiency floor p8", efficiency_p8 >= min_efficiency_p8))
            return 1;

        sum_speedup_p8 += speedup_p8;
    }

    if (!expect_true("portfolio p8 mean speedup floor", (sum_speedup_p8 / (double)count) >= 1.6))
        return 1;

    return 0;
}
