#include "runtime/parallel/contract/parallel_contract.h"

#include "runtime/support/test_support.h"

static void test_parallel_contract_entries_expose_inventory(void **state)
{
    const RuntimeParallelBudget *entries = NULL;
    int count = -1;

    (void)state;

    entries = runtime_parallel_contract_entries(&count);

    BANANA_TEST_ASSERT_TRUE(entries != NULL,
                            "parallel contract should expose a static inventory table");
    BANANA_TEST_ASSERT_TRUE(count > 0,
                            "parallel contract count should be positive");
    BANANA_TEST_ASSERT_INT_EQ(runtime_parallel_contract_expected_inventory_size(),
                              count,
                              "expected inventory size should match entry count");
    BANANA_TEST_ASSERT_TRUE(entries[0].kernel_id != NULL,
                            "first budget entry should include a kernel id");
    BANANA_TEST_ASSERT_TRUE(entries[0].source_path != NULL,
                            "first budget entry should include source path");
    BANANA_TEST_ASSERT_TRUE(entries[0].source_line > 0,
                            "first budget entry should include source line");
    BANANA_TEST_ASSERT_TRUE(entries[0].serial_fraction_budget > 0.0,
                            "first budget entry should include positive serial budget");
}

static void test_parallel_contract_entries_support_null_count_pointer(void **state)
{
    const RuntimeParallelBudget *entries = NULL;

    (void)state;

    entries = runtime_parallel_contract_entries(NULL);
    BANANA_TEST_ASSERT_TRUE(entries != NULL,
                            "entries should still be returned with null count output pointer");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_parallel_contract_entries_expose_inventory),
    BANANA_TEST_CASE(test_parallel_contract_entries_support_null_count_pointer)
)
