#include "ai/combat_controller.h"   /* Combat controller API under test. */
#include "ai/controller.h"          /* Generic controller registry/update/signal seam. */

#include <math.h>                   /* fabsf for numeric delta checks. */
#include <stdio.h>                  /* fprintf for assertion diagnostics. */
#include <string.h>                 /* strcmp/strcpy for mode and identity checks. */

/*
 * Mirror the internal combat-controller state layout so the regression test can
 * inspect the cached k3h4 bias and cooldown fields used by gameplay updates.
 */
typedef enum CombatControllerModeTest
{
    COMBAT_CONTROLLER_MODE_TEST_IDLE = 0,
    COMBAT_CONTROLLER_MODE_TEST_COMBAT = 1,
} CombatControllerModeTest;

typedef struct CombatControllerStateTest
{
    float target[3];
    float combat_timer;
    CombatControllerModeTest mode;
    float k3h4_bias;
    float k3h4_bias_cooldown;
} CombatControllerStateTest;

/*
 * This file is a gameplay-oriented regression test for the combat controller.
 * It covers: controller creation, bias safety, combat engagement, movement update,
 * and fail-closed behavior after a type-name mutation that should invalidate the
 * controller identity used by the AI helpers.
 */

/*
 * Assertion helper for the combat controller regression test.
 * It keeps the expected gameplay invariants readable in the test body and prints
 * a concrete error message when a scenario regresses.
 */
static int fail_if(int condition, const char *message)
{
    if (condition)
    {
        /* Print the failing gameplay invariant to stderr so CI output points to the issue. */
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

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
    return fail_if(!(fabsf(controller->position[0] - expected_x) < 0.0001f &&
                     fabsf(controller->position[2] - expected_z) < 0.0001f),
                   "controller position did not move to the engagement target");
}

/*
 * Verify the bias helper returns a safe zero value for a null controller handle.
 * This protects gameplay and diagnostics callers from accidental dereferences.
 */
static int test_null_bias_is_safe(void)
{
    return fail_if(!(combat_controller_k3h4_bias(NULL) == 0.0f),
                   "null controller should yield a safe zero bias");
}

/*
 * Check that an idle combat controller still yields a bounded k3h4 bias.
 * The value should remain in the valid [0, 1] range before any combat intent is applied.
 */
static int test_initial_bias_is_bounded(ControllerInstance *controller)
{
    float bias = combat_controller_k3h4_bias(controller);
    return fail_if(!(bias >= 0.0f && bias <= 1.0f),
                   "initial combat bias must stay in [0, 1]");
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
    return fail_if(mode == NULL || strcmp(mode, "combat") != 0,
                   "battle_engage must switch debug mode to combat");
}

/*
 * Verify the bias helper still returns a bounded value after engagement starts.
 * This ensures the AI hint remains usable once combat state is active.
 */
static int test_engaged_bias_is_bounded(ControllerInstance *controller)
{
    float bias = combat_controller_k3h4_bias(controller);
    return fail_if(!(bias >= 0.0f && bias <= 1.0f),
                   "engaged combat bias must stay in [0, 1]");
}

/*
 * Verify that engagement initializes the cached bias and cooldown immediately.
 * This guards the production contract that callers should not observe a dead
 * zero-bias state just because the controller has entered combat.
 */
static int test_engage_initializes_cache(ControllerInstance *controller, const float target[3])
{
    CombatControllerStateTest *state = NULL;
    float bias = 0.0f;

    if (!controller || !controller->state)
        return 1;

    controller_signal(controller, "battle_engage", target);
    state = (CombatControllerStateTest *)controller->state;
    bias = combat_controller_k3h4_bias(controller);

    if (fail_if(!(state->k3h4_bias == bias),
               "engage should keep the cached getter aligned with the stored bias"))
        return 1;

    return fail_if(!(state->k3h4_bias_cooldown > 0.0f),
                   "engage should start the cached-bias cooldown window immediately");
}

/*
 * Snapshot the mutable combat state so the cache-throttle scenario can be
 * restored after each assertion without leaking state between helper checks.
 */
static void snapshot_controller_state(ControllerInstance *controller,
                                      CombatControllerStateTest *state,
                                      float previous_target[3],
                                      float previous_position[3],
                                      float *previous_timer,
                                      CombatControllerModeTest *previous_mode)
{
    if (!controller || !state)
        return;

    previous_target[0] = state->target[0];
    previous_target[1] = state->target[1];
    previous_target[2] = state->target[2];
    previous_position[0] = controller->position[0];
    previous_position[1] = controller->position[1];
    previous_position[2] = controller->position[2];
    *previous_timer = state->combat_timer;
    *previous_mode = state->mode;
}

/*
 * Reset the combat state back to its snapshot after a cache-throttle assertion.
 */
static void restore_controller_state(ControllerInstance *controller,
                                    CombatControllerStateTest *state,
                                    const float previous_target[3],
                                    const float previous_position[3],
                                    float previous_timer,
                                    CombatControllerModeTest previous_mode)
{
    if (!controller || !state)
        return;

    state->target[0] = previous_target[0];
    state->target[1] = previous_target[1];
    state->target[2] = previous_target[2];
    controller->position[0] = previous_position[0];
    controller->position[1] = previous_position[1];
    controller->position[2] = previous_position[2];
    state->combat_timer = previous_timer;
    state->mode = previous_mode;
}

/*
 * Prepare the controller for a cache-throttle regression scenario.
 */
static int prime_cache_throttle_scenario(ControllerInstance *controller,
                                         CombatControllerStateTest *state)
{
    if (!controller || !state)
        return 1;

    state->mode = COMBAT_CONTROLLER_MODE_TEST_COMBAT;
    state->target[0] = 10.0f;
    state->target[1] = 0.0f;
    state->target[2] = 10.0f;
    state->combat_timer = 1.0f;
    state->k3h4_bias = 1.0f;
    state->k3h4_bias_cooldown = 1.0f;
    return 0;
}

/*
 * Assert that cached bias remains stable while the cooldown window is active.
 */
static int assert_cached_bias_remains_stable(CombatControllerStateTest *state,
                                            float initial_bias)
{
    return fail_if(!(state->k3h4_bias == initial_bias),
                   "update should keep the cached bias value while the cooldown window is active");
}

/*
 * Assert that the cooldown timer is consumed during the update tick.
 */
static int assert_cooldown_is_consumed(CombatControllerStateTest *state,
                                       float initial_cooldown)
{
    return fail_if(!(state->k3h4_bias_cooldown < initial_cooldown),
                   "update should consume the cached-bias cooldown timer");
}

/*
 * Verify the cached bias and cooldown fields are actually consumed during combat updates.
 * The helper now orchestrates smaller assertion blocks so the scenario is easier to read.
 */
static int test_bias_cache_is_throttled(ControllerInstance *controller)
{
    CombatControllerStateTest *state = NULL;
    float initial_bias = 0.0f;
    float initial_cooldown = 0.0f;
    float previous_target[3] = {0.0f, 0.0f, 0.0f};
    float previous_position[3] = {0.0f, 0.0f, 0.0f};
    float previous_timer = 0.0f;
    CombatControllerModeTest previous_mode = COMBAT_CONTROLLER_MODE_TEST_IDLE;

    if (!controller || !controller->state)
        return 1;

    state = (CombatControllerStateTest *)controller->state;
    snapshot_controller_state(controller, state, previous_target, previous_position,
                              &previous_timer, &previous_mode);

    if (prime_cache_throttle_scenario(controller, state) != 0)
        return 1;

    initial_bias = state->k3h4_bias;
    initial_cooldown = state->k3h4_bias_cooldown;

    controller_update(controller, 0.1f);

    if (assert_cached_bias_remains_stable(state, initial_bias) ||
        assert_cooldown_is_consumed(state, initial_cooldown))
    {
        restore_controller_state(controller, state, previous_target, previous_position,
                                 previous_timer, previous_mode);
        return 1;
    }

    restore_controller_state(controller, state, previous_target, previous_position,
                             previous_timer, previous_mode);
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
 * Simulate a stale or corrupted controller identity and confirm the helpers fail closed.
 * This guards memory-safety and intent integrity when gameplay state is mutated unexpectedly.
 */
static int test_mutated_identity_fails_closed(ControllerInstance *controller)
{
    float bias = 0.0f;

    strcpy(controller->type_name, "other");
    bias = combat_controller_k3h4_bias(controller);
    if (fail_if(!(bias == 0.0f), "mutated controller identity must fail closed for k3h4 bias"))
        return 1;

    return fail_if(combat_controller_debug_mode(controller) != NULL,
                   "mutated controller identity must fail closed for debug mode");
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
    return fail_if(mode == NULL || strcmp(mode, "idle") != 0,
                   "death must switch debug mode to idle");
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
static int run_identity_contract(ControllerInstance *controller)
{
    return test_mutated_identity_fails_closed(controller) ||
           test_mutated_destroy_is_safe();
}

/*
 * Main orchestration path for the combat-controller test.
 * The body now reads as a sequence of thin helper-driven checks rather than one monolithic block.
 */
int main(void)
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
        run_identity_contract(controller) ||
        test_death_resets_mode(controller))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_destroy(controller);
    return 0;
}
