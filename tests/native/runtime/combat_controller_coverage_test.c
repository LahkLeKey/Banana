#include "ai/combat_controller.h"   /* Combat controller API under test. */
#include "ai/controller.h"          /* Generic controller registry/update/signal seam. */

#include <math.h>                   /* fabsf for numeric delta checks. */
#include <stdio.h>                  /* fprintf for assertion diagnostics. */
#include <string.h>                 /* strcmp/strcpy for mode and identity checks. */

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
 * Main orchestration path for the combat-controller test.
 * The body now reads as a sequence of unit-style checks rather than one monolithic block.
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
        test_engage_transition(controller, target) ||
        test_engaged_bias_is_bounded(controller) ||
        test_update_moves_toward_target(controller) ||
        test_mutated_identity_fails_closed(controller) ||
        test_death_resets_mode(controller))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_destroy(controller);
    return 0;
}
