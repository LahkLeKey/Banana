#include "ai/state_machine.h"
#include "../../support/test_support.h"
#include <string.h>

static void count_enter(void *ctx)
{
    int *counter = (int *)ctx;
    *counter += 1;
}

static void count_update(void *ctx, float dt)
{
    int *counter = (int *)ctx;
    *counter += (int)dt;
}

static void test_fsm_add_state_and_trigger(void **state)
{
    (void)state;
    StateMachine fsm;
    int counter = 0;
    int idle_idx = -1;
    int patrol_idx = -1;

    fsm_init(&fsm, &counter);
    idle_idx = fsm_add_state(&fsm, "idle", count_enter, count_update, NULL);
    patrol_idx = fsm_add_state(&fsm, "patrol", count_enter, count_update, NULL);
    fsm_add_transition(&fsm, idle_idx, patrol_idx, "start_patrol");

    BANANA_TEST_ASSERT_INT_EQ(idle_idx, 0, "fsm state registration order must be stable");
    BANANA_TEST_ASSERT_INT_EQ(patrol_idx, 1, "fsm state registration order must be stable");
    BANANA_TEST_ASSERT_STR_EQ(fsm_current_state_name(&fsm), "idle",
                              "fsm should start on the first registered state");
    BANANA_TEST_ASSERT_INT_EQ(counter, 1, "fsm initial state enter callback should run once");

    fsm_trigger(&fsm, "start_patrol");
    BANANA_TEST_ASSERT_STR_EQ(fsm_current_state_name(&fsm), "patrol",
                              "fsm trigger should transition to linked state");
    BANANA_TEST_ASSERT_INT_EQ(counter, 2, "fsm transition enter callback should run once");

    fsm_update(&fsm, 1.5f);
    BANANA_TEST_ASSERT_INT_EQ(counter, 3,
                              "fsm update should advance state context while active");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_fsm_add_state_and_trigger)
)
