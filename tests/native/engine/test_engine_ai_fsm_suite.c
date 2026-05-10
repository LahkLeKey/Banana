#include "test_engine_domain_suite_framework.h"

#include <string.h>

#include "../../../src/native/engine/ai/state_machine.h"

typedef struct FSMCtx
{
    int enter_count;
    int update_count;
} FSMCtx;

static void on_enter_idle(void *c) { ((FSMCtx *)c)->enter_count++; }
static void on_update_idle(void *c, float d)
{
    (void)d;
    ((FSMCtx *)c)->update_count++;
}
static void on_exit_idle(void *c) { (void)c; }
static void on_enter_patrol(void *c) { ((FSMCtx *)c)->enter_count++; }

static void test_fsm_initial_state(DomainSuiteStats *stats)
{
    FSMCtx ctx = {0};
    StateMachine fsm;

    SUITE_TEST("fsm: first added state becomes initial state");
    fsm_init(&fsm, &ctx);
    fsm_add_state(&fsm, "idle", on_enter_idle, on_update_idle, on_exit_idle);
    fsm_add_state(&fsm, "patrol", on_enter_patrol, NULL, NULL);
    SUITE_ASSERT_TRUE(stats, strcmp(fsm_current_state_name(&fsm), "idle") == 0);
    SUITE_ASSERT_INT_EQ(stats, ctx.enter_count, 1);
    SUITE_PASS(stats);
done:
    return;
}

static void test_fsm_transition(DomainSuiteStats *stats)
{
    FSMCtx ctx = {0};
    StateMachine fsm;

    SUITE_TEST("fsm_trigger: transitions to correct state on trigger");
    fsm_init(&fsm, &ctx);
    int s_idle = fsm_add_state(&fsm, "idle", on_enter_idle, on_update_idle, on_exit_idle);
    int s_patrol = fsm_add_state(&fsm, "patrol", on_enter_patrol, NULL, NULL);
    fsm_add_transition(&fsm, s_idle, s_patrol, "start_patrol");
    fsm_trigger(&fsm, "start_patrol");
    SUITE_ASSERT_TRUE(stats, strcmp(fsm_current_state_name(&fsm), "patrol") == 0);
    SUITE_ASSERT_INT_EQ(stats, ctx.enter_count, 2);
    SUITE_PASS(stats);
done:
    return;
}

static void test_fsm_update(DomainSuiteStats *stats)
{
    FSMCtx ctx = {0};
    StateMachine fsm;

    SUITE_TEST("fsm_update: calls on_update for current state");
    fsm_init(&fsm, &ctx);
    fsm_add_state(&fsm, "idle", on_enter_idle, on_update_idle, on_exit_idle);
    fsm_update(&fsm, 0.016f);
    fsm_update(&fsm, 0.016f);
    SUITE_ASSERT_INT_EQ(stats, ctx.update_count, 2);
    SUITE_PASS(stats);
done:
    return;
}

int run_engine_ai_fsm_suite(void)
{
    DomainSuiteStats stats = {0, 0};

    printf("-- AI FSM Suite --\n");
    test_fsm_initial_state(&stats);
    test_fsm_transition(&stats);
    test_fsm_update(&stats);

    printf("AI FSM: %d passed, %d failed\n\n", stats.pass, stats.fail);
    return stats.fail;
}
