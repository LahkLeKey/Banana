#include "runtime/parallel/model/parallel_model.h"

#include "../../support/test_support.h"

static void test_amdahl_speedup_and_efficiency_bounds(void **state)
{
    (void)state;
    double speedup = runtime_parallel_amdahl_speedup(0.25, 8);
    double efficiency = runtime_parallel_amdahl_efficiency(0.25, 8);

    BANANA_TEST_ASSERT_TRUE(speedup > 1.0,
                            "amdahl speedup should exceed 1.0 when worker count is greater than one");
    BANANA_TEST_ASSERT_TRUE(efficiency > 0.0 && efficiency <= 1.0,
                            "amdahl efficiency should remain within (0, 1]");

    BANANA_TEST_ASSERT_TRUE(runtime_parallel_amdahl_speedup(-2.0, 1) == 1.0,
                            "negative serial fraction should clamp to fully parallel lower bound");
    BANANA_TEST_ASSERT_TRUE(runtime_parallel_amdahl_speedup(2.0, 8) == 1.0,
                            "serial fraction above one should clamp to fully serial behavior");
}

static void test_amdahl_serial_fraction_inverse(void **state)
{
    (void)state;
    double serial = runtime_parallel_amdahl_serial_fraction_from_speedup(2.0, 4);

    BANANA_TEST_ASSERT_TRUE(serial >= 0.0 && serial <= 1.0,
                            "inverse serial fraction must clamp to [0, 1]");
    BANANA_TEST_ASSERT_TRUE(runtime_parallel_amdahl_serial_fraction_from_speedup(0.0, 4) == 1.0,
                            "non-positive speedup should map to serial fallback");
    BANANA_TEST_ASSERT_TRUE(runtime_parallel_amdahl_serial_fraction_from_speedup(2.0, 1) == 1.0,
                            "single-worker inverse should map to serial fallback");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_amdahl_speedup_and_efficiency_bounds),
    BANANA_TEST_CASE(test_amdahl_serial_fraction_inverse)
)
