#include "ai/combat_controller.h"   /* Combat controller API under test. */
#include "ai/controller.h"          /* Generic controller registry/update/signal seam. */
#include "../../support/test_support.h"

#include <math.h>                   /* fabsf for numeric delta checks. */
#include <stdio.h>                  /* fprintf for assertion diagnostics. */
#include <string.h>                 /* strcmp/strcpy for mode and identity checks. */

#if defined(BANANA_USE_CMOCKA)
#define COMBAT_TEST_ASSERT_TRUE(condition, message)                                                \
    do                                                                                              \
    {                                                                                               \
        if (!(condition))                                                                           \
            fail_msg("%s", (message));                                                             \
    } while (0)
#else
#define COMBAT_TEST_ASSERT_TRUE(condition, message)                                                \
    do                                                                                              \
    {                                                                                               \
        if (!(condition))                                                                           \
        {                                                                                           \
            fprintf(stderr, "%s\n", (message));                                                    \
            return 1;                                                                                \
        }                                                                                           \
    } while (0)
#endif

#undef BANANA_TEST_ASSERT_TRUE
#define BANANA_TEST_ASSERT_TRUE COMBAT_TEST_ASSERT_TRUE

/*
 * This file is a gameplay-oriented regression test for the combat controller.
 * It covers: controller creation, bias safety, combat engagement, movement update,
 * and fail-closed behavior after a type-name mutation that should invalidate the
 * controller identity used by the AI helpers.
 */

/*
 * Verify that the update loop changes the combatant position.
 * This is a gameplay-level proof that the controller is not just changing timers
 * or cached state; it is actually advancing the actor toward its target.
 */
static int expect_position_delta(const ControllerInstance *controller, float expected_x, float expected_z)
{
    if (!controller)
        return 1;  /* Fail fast if the test was handed an invalid controller handle. */

    /* Compare the current coordinates to the expected target position with a tiny epsilon.
     * This avoids false negatives from floating-point noise while still catching real regressions. */
    BANANA_TEST_ASSERT_TRUE(fabsf(controller->position[0] - expected_x) < 0.0001f &&
                            fabsf(controller->position[2] - expected_z) < 0.0001f,
                            "controller position did not move to the engagement target");
    return 0;
}

/*
 * Verify the bias helper returns a safe zero value for a null controller handle.
 * This protects gameplay and diagnostics callers from accidental dereferences.
 */
static int test_null_bias_is_safe(void)
{
    BANANA_TEST_ASSERT_TRUE(combat_controller_k3h4_bias(NULL) == 0.0f,
                            "null controller should yield a safe zero bias");
    return 0;
}

/*
 * Check that an idle combat controller still yields a bounded k3h4 bias.
 * The value should remain in the valid [0, 1] range before any combat intent is applied.
 */
static int test_initial_bias_is_bounded(ControllerInstance *controller)
{
    float bias = combat_controller_k3h4_bias(controller);
    BANANA_TEST_ASSERT_TRUE(bias >= 0.0f && bias <= 1.0f,
                            "initial combat bias must stay in [0, 1]");
    return 0;
}

/*
 * Exercise the battle_engage signal and confirm the controller enters combat mode.
 * This validates the gameplay intent transition that drives the update loop.
 */
static int test_engage_transition(ControllerInstance *controller, const float target[3])
{
    const char *mode = NULL;

    controller_signal(controller, "battle_engage", target);
    mode = combat_controller_debug_mode(controller);
    BANANA_TEST_ASSERT_TRUE(mode != NULL && strcmp(mode, "combat") == 0,
                            "battle_engage must switch debug mode to combat");
    return 0;
}

/*
 * Verify the bias helper still returns a bounded value after engagement starts.
 * This ensures the AI hint remains usable once combat state is active.
 */
static int test_engaged_bias_is_bounded(ControllerInstance *controller)
{
    float bias = combat_controller_k3h4_bias(controller);
    BANANA_TEST_ASSERT_TRUE(bias >= 0.0f && bias <= 1.0f,
                            "engaged combat bias must stay in [0, 1]");
    return 0;
}

/*
 * Verify that engagement initializes the cached bias and cooldown immediately.
 * This guards the production contract that callers should not observe a dead
 * zero-bias state just because the controller has entered combat.
 */
static int test_engage_initializes_cache(ControllerInstance *controller, const float target[3])
{
    CombatControllerDebugSnapshot snapshot;
    float bias = 0.0f;

    if (!controller)
        return 1;

    controller_signal(controller, "battle_engage", target);
    combat_controller_debug_snapshot(controller, &snapshot);
    bias = combat_controller_k3h4_bias(controller);

    BANANA_TEST_ASSERT_TRUE(snapshot.k3h4_bias == bias,
                            "engage should keep cached getter aligned with stored bias");
    BANANA_TEST_ASSERT_TRUE(snapshot.k3h4_bias_cooldown > 0.0f,
                            "engage should start cached-bias cooldown immediately");
    return 0;
}

/*
 * Snapshot the mutable combat state so the cache-throttle scenario can be
 * restored after each assertion without leaking state between helper checks.
 */
static void snapshot_controller_state(ControllerInstance *controller,
                                      CombatControllerDebugSnapshot *snapshot,
                                      float previous_target[3],
                                      float previous_position[3],
                                      float *previous_timer,
                                      const char **previous_mode,
                                      float *previous_bias,
                                      float *previous_cooldown)
{
    if (!controller || !snapshot)
        return;

    previous_target[0] = snapshot->target[0];
    previous_target[1] = snapshot->target[1];
    previous_target[2] = snapshot->target[2];
    previous_position[0] = controller->position[0];
    previous_position[1] = controller->position[1];
    previous_position[2] = controller->position[2];
    *previous_timer = snapshot->combat_timer;
    *previous_mode = snapshot->mode;
    *previous_bias = snapshot->k3h4_bias;
    *previous_cooldown = snapshot->k3h4_bias_cooldown;
}

/*
 * Reset the combat state back to its snapshot after a cache-throttle assertion.
 */
static void restore_controller_state(ControllerInstance *controller,
                                    const float previous_target[3],
                                    const float previous_position[3],
                                    float previous_timer,
                                    const char *previous_mode,
                                    float previous_bias,
                                    float previous_cooldown)
{
    CombatControllerDebugSnapshot snapshot;

    if (!controller)
        return;

    snapshot.target[0] = previous_target[0];
    snapshot.target[1] = previous_target[1];
    snapshot.target[2] = previous_target[2];
    snapshot.combat_timer = previous_timer;
    snapshot.mode = previous_mode;
    snapshot.k3h4_bias = previous_bias;
    snapshot.k3h4_bias_cooldown = previous_cooldown;

    controller->position[0] = previous_position[0];
    controller->position[1] = previous_position[1];
    controller->position[2] = previous_position[2];

    combat_controller_debug_restore(controller, &snapshot);
}

/*
 * Prepare the controller for a cache-throttle regression scenario.
 */
static int prime_cache_throttle_scenario(ControllerInstance *controller)
{
    CombatControllerDebugSnapshot snapshot;

    if (!controller)
        return 1;

    snapshot.target[0] = 10.0f;
    snapshot.target[1] = 0.0f;
    snapshot.target[2] = 10.0f;
    snapshot.combat_timer = 1.0f;
    snapshot.mode = "combat";
    snapshot.k3h4_bias = 1.0f;
    snapshot.k3h4_bias_cooldown = 1.0f;

    combat_controller_debug_restore(controller, &snapshot);
    return 0;
}

/*
 * Assert that cached bias remains stable while the cooldown window is active.
 */
static int assert_cached_bias_remains_stable(ControllerInstance *controller,
                                            float initial_bias)
{
    CombatControllerDebugSnapshot snapshot;

    combat_controller_debug_snapshot(controller, &snapshot);
    BANANA_TEST_ASSERT_TRUE(snapshot.k3h4_bias == initial_bias,
                            "update should keep cached bias during cooldown window");
    return 0;
}

/*
 * Assert that the cooldown timer is consumed during the update tick.
 */
static int assert_cooldown_is_consumed(ControllerInstance *controller,
                                       float initial_cooldown)
{
    CombatControllerDebugSnapshot snapshot;

    combat_controller_debug_snapshot(controller, &snapshot);
    BANANA_TEST_ASSERT_TRUE(snapshot.k3h4_bias_cooldown < initial_cooldown,
                            "update should consume cached-bias cooldown timer");
    return 0;
}

/*
 * Verify the cached bias and cooldown fields are actually consumed during combat updates.
 * The helper now orchestrates smaller assertion blocks so the scenario is easier to read.
 */
static int test_bias_cache_is_throttled(ControllerInstance *controller)
{
    CombatControllerDebugSnapshot snapshot;
    float initial_bias = 0.0f;
    float initial_cooldown = 0.0f;
    float previous_target[3] = {0.0f, 0.0f, 0.0f};
    float previous_position[3] = {0.0f, 0.0f, 0.0f};
    float previous_timer = 0.0f;
    const char *previous_mode = NULL;
    float previous_bias = 0.0f;
    float previous_cooldown = 0.0f;

    if (!controller)
        return 1;

    combat_controller_debug_snapshot(controller, &snapshot);
    snapshot_controller_state(controller, &snapshot, previous_target, previous_position,
                              &previous_timer, &previous_mode,
                              &previous_bias, &previous_cooldown);

    if (prime_cache_throttle_scenario(controller) != 0)
        return 1;

    combat_controller_debug_snapshot(controller, &snapshot);
    initial_bias = snapshot.k3h4_bias;
    initial_cooldown = snapshot.k3h4_bias_cooldown;

    controller_update(controller, 0.1f);

    if (assert_cached_bias_remains_stable(controller, initial_bias) ||
        assert_cooldown_is_consumed(controller, initial_cooldown))
    {
        restore_controller_state(controller, previous_target, previous_position,
                                 previous_timer, previous_mode,
                                 previous_bias, previous_cooldown);
        return 1;
    }

    restore_controller_state(controller, previous_target, previous_position,
                             previous_timer, previous_mode,
                             previous_bias, previous_cooldown);
    return 0;
}

/*
 * Confirm that one update tick moves the controller toward its combat target.
 * This makes the test exercise real gameplay movement rather than only state mutation.
 */
static int test_update_moves_toward_target(ControllerInstance *controller)
{
    controller_update(controller, 1.0f);
    return expect_position_delta(controller, 4.0f, 6.0f);
}

/*
 * Verify that the debug snapshot captures engaged combat state, including the
 * engaged target coordinates and bounded cached bias/cooldown values.
 */
static int test_debug_snapshot_captures_cached_state(ControllerInstance *controller, const float target[3])
{
    CombatControllerDebugSnapshot snapshot;

    if (!controller)
        return 1;

    controller_signal(controller, "battle_engage", target);
    combat_controller_debug_snapshot(controller, &snapshot);

    BANANA_TEST_ASSERT_TRUE(snapshot.mode != NULL && strcmp(snapshot.mode, "combat") == 0,
                            "debug snapshot should report active combat mode");

    BANANA_TEST_ASSERT_TRUE(snapshot.target[0] == target[0] &&
                            snapshot.target[1] == target[1] &&
                            snapshot.target[2] == target[2],
                            "debug snapshot should capture engaged target coordinates");

    BANANA_TEST_ASSERT_TRUE(snapshot.k3h4_bias >= 0.0f && snapshot.k3h4_bias <= 1.0f &&
                            snapshot.k3h4_bias_cooldown > 0.0f,
                            "debug snapshot should expose bounded bias and active cooldown");
    return 0;
}

static int test_debug_restore_rehydrates_state(ControllerInstance *controller)
{
    CombatControllerDebugSnapshot snapshot;

    if (!controller)
        return 1;

    snapshot.target[0] = 3.0f;
    snapshot.target[1] = -2.0f;
    snapshot.target[2] = 5.0f;
    snapshot.combat_timer = 2.5f;
    snapshot.mode = "combat";
    snapshot.k3h4_bias = 0.42f;
    snapshot.k3h4_bias_cooldown = 0.75f;

    combat_controller_debug_restore(controller, &snapshot);

    combat_controller_debug_snapshot(controller, &snapshot);
    BANANA_TEST_ASSERT_TRUE(snapshot.target[0] == 3.0f &&
                            snapshot.target[1] == -2.0f &&
                            snapshot.target[2] == 5.0f,
                            "debug restore should rehydrate target coordinates");

    BANANA_TEST_ASSERT_TRUE(snapshot.combat_timer == 2.5f,
                            "debug restore should rehydrate engagement timer");

    BANANA_TEST_ASSERT_TRUE(snapshot.k3h4_bias == 0.42f && snapshot.k3h4_bias_cooldown == 0.75f,
                            "debug restore should rehydrate cached bias and cooldown");

    BANANA_TEST_ASSERT_TRUE(snapshot.mode != NULL && strcmp(snapshot.mode, "combat") == 0,
                            "debug restore should restore combat mode label");
    return 0;
}

static int test_exact_identity_with_trailing_bytes_is_accepted(ControllerInstance *controller)
{
    CombatControllerDebugSnapshot snapshot;
    const char *mode = NULL;

    if (!controller)
        return 1;

    snapshot.target[0] = 0.0f;
    snapshot.target[1] = 0.0f;
    snapshot.target[2] = 0.0f;
    snapshot.combat_timer = 0.0f;
    snapshot.mode = "idle";
    snapshot.k3h4_bias = 0.0f;
    snapshot.k3h4_bias_cooldown = 0.0f;
    combat_controller_debug_restore(controller, &snapshot);

    memset(controller->type_name, 0, sizeof(controller->type_name));
    strncpy(controller->type_name, "combat", sizeof(controller->type_name) - 1);
    controller->type_name[7] = 'x';

    mode = combat_controller_debug_mode(controller);
    BANANA_TEST_ASSERT_TRUE(mode != NULL,
                            "exact combat identity must be accepted with trailing bytes");
    BANANA_TEST_ASSERT_TRUE(strcmp(mode, "idle") == 0,
                            "exact combat identity should still resolve to idle mode");
    return 0;
}

static int test_mutated_identity_fails_closed(ControllerInstance *controller)
{
    float bias = 0.0f;

    strcpy(controller->type_name, "other");
    bias = combat_controller_k3h4_bias(controller);
    BANANA_TEST_ASSERT_TRUE(bias == 0.0f,
                            "mutated controller identity must fail closed for k3h4 bias");
    BANANA_TEST_ASSERT_TRUE(combat_controller_debug_mode(controller) == NULL,
                            "mutated controller identity must fail closed for debug mode");
    return 0;
}

/*
 * Verify the destroy path still frees owned state after the controller identity
 * has been corrupted. This guards the real ownership contract used by cleanup.
 */
static int test_mutated_destroy_is_safe(void)
{
    ControllerInstance *controller = controller_create("combat", 0.0f, 0.0f, 0.0f);

    if (!controller)
        return 1;

    strcpy(controller->type_name, "other");
    controller_destroy(controller);
    return 0;
}

/*
 * Verify the death signal resets the controller back to idle.
 * This confirms that combat state can be cleared cleanly after an engagement ends.
 */
static int test_death_resets_mode(ControllerInstance *controller)
{
    const char *mode = NULL;

    strcpy(controller->type_name, "combat");
    controller_signal(controller, "death", NULL);
    mode = combat_controller_debug_mode(controller);
    BANANA_TEST_ASSERT_TRUE(mode != NULL && strcmp(mode, "idle") == 0,
                            "death must switch debug mode to idle");
    return 0;
}

/*
 * Orchestrate the engage-related combat contract with small, readable helpers.
 * Each helper covers one gameplay invariant so the larger scenario stays easy to reason about.
 */
static int run_engage_contract(ControllerInstance *controller, const float target[3])
{
    return test_engage_transition(controller, target) ||
           test_engaged_bias_is_bounded(controller) ||
           test_engage_initializes_cache(controller, target);
}

/*
 * Orchestrate the movement and cache-throttle behavior with a dedicated helper.
 */
static int run_update_contract(ControllerInstance *controller)
{
    return test_bias_cache_is_throttled(controller) ||
           test_update_moves_toward_target(controller);
}

/*
 * Orchestrate the fail-closed and cleanup invariants with a dedicated helper.
 */
static int run_debug_contract(ControllerInstance *controller, const float target[3])
{
    return test_debug_snapshot_captures_cached_state(controller, target) ||
           test_debug_restore_rehydrates_state(controller);
}

static int run_identity_contract(ControllerInstance *controller)
{
    return test_exact_identity_with_trailing_bytes_is_accepted(controller) ||
           test_mutated_identity_fails_closed(controller) ||
           test_mutated_destroy_is_safe();
}

/*
 * Main orchestration path for the combat-controller test.
 * The body now reads as a sequence of thin helper-driven checks rather than one monolithic block.
 */
static int run_all_combat_tests(void)
{
    ControllerInstance *controller = NULL;
    float target[3] = {4.0f, 0.0f, 6.0f};

    combat_controller_register();
    controller = controller_create("combat", 0.0f, 0.0f, 0.0f);
    if (!controller)
        return 1;

    if (test_null_bias_is_safe() ||
        test_initial_bias_is_bounded(controller) ||
        run_engage_contract(controller, target) ||
        run_update_contract(controller) ||
        run_debug_contract(controller, target) ||
        run_identity_contract(controller) ||
        test_death_resets_mode(controller))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_destroy(controller);
    return 0;
}

static void test_combat_controller_coverage(void **state)
{
    (void)state;
    if (run_all_combat_tests() != 0)
    {
        BANANA_TEST_FAIL("combat controller coverage contract must pass");
    }
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_combat_controller_coverage)
)
