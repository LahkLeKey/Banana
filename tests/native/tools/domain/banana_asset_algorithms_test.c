#include "tools/domain/banana_asset_algorithms.h"

#include "../../runtime/support/test_support.h"

static void test_default_contract_reports_expected_algorithm_metadata(void **state)
{
    const banana_asset_algorithm_contract_t *contract = banana_default_asset_algorithm_contract();

    (void)state;

    BANANA_TEST_ASSERT_TRUE(contract != NULL,
                            "default asset algorithm contract should exist");
    BANANA_TEST_ASSERT_STR_EQ(contract->tile_generation_name,
                              "wave-function-collapse",
                              "default tile-generation algorithm name should match contract");
    BANANA_TEST_ASSERT_STR_EQ(contract->post_process_name,
                              "cellular-automata",
                              "default post-process algorithm name should match contract");
}

static void test_default_contract_exposes_algorithm_callbacks(void **state)
{
    const banana_asset_algorithm_contract_t *contract = banana_default_asset_algorithm_contract();

    (void)state;

    BANANA_TEST_ASSERT_TRUE(contract != NULL,
                            "default asset algorithm contract should exist");
    BANANA_TEST_ASSERT_TRUE(contract->generate_tiles == banana_generate_tiles_wfc,
                            "default contract should expose WFC generation callback");
    BANANA_TEST_ASSERT_TRUE(contract->post_process_tiles == banana_apply_cellular_automata,
                            "default contract should expose cellular-automata callback");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_default_contract_reports_expected_algorithm_metadata),
    BANANA_TEST_CASE(test_default_contract_exposes_algorithm_callbacks)
)
