#include "../controller.h"
#include "../combat_controller.h"

#include "runtime/abi/netcode/netcode_abi.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/*
 * Combat controller state machine.
 * The controller is intentionally lightweight: it tracks the current combat target,
 * the remaining engagement timer, and the cached k3h4 bias used to steer gameplay.
 */
typedef enum CombatControllerMode
{
    COMBAT_CONTROLLER_MODE_IDLE = 0,
    COMBAT_CONTROLLER_MODE_COMBAT = 1,
} CombatControllerMode;

/*
 * Runtime state owned by each combat controller instance.
 * The cached bias values are updated on a short cooldown so the AI path remains
 * responsive without recomputing expensive k3h4 output on every frame.
 */
typedef struct CombatControllerState
{
    float target[3];
    float combat_timer;
    CombatControllerMode mode;
    float k3h4_bias;
    float k3h4_bias_cooldown;
} CombatControllerState;

/*
 * Helper: return non-zero when the controller instance identifies as the "combat" controller.
 * Uses a bounded compare so the check stays safe even if type_name is not null-terminated.
 */
static int combat_controller_is_combat_type(const ControllerInstance *controller)
{
    return controller != NULL &&
           strncmp(controller->type_name, "combat", sizeof("combat")) == 0;
}

static float combat_controller_compute_bias(const CombatControllerState *state)
{
    RuntimeK3h4VectorSignalInput input;
    RuntimeNetcodeK3h4Output output;
    float normalized = 0.0f;

    if (!state)
        return 0.0f;  /* Defensive guard: never derive bias from a null state. */

    /* Zero out the ABI request/response payloads before filling the fields.
     * This keeps the k3h4 input deterministic and avoids stale bytes from reused stack memory. */
    memset(&input, 0, sizeof(input));
    memset(&output, 0, sizeof(output));

    /* Convert the current gameplay situation into the compact k3h4 input vector.
     * These values intentionally map the target distance, timer pressure, and mode
     * into the same deterministic signal shape expected by the ABI path. */
    input.call_density = (int)fminf(255.0f, fabsf(state->target[0]) + fabsf(state->target[2]));
    input.quest_percent = (int)fminf(100.0f, state->combat_timer * 10.0f);
    input.player_level = 1;
    input.combo_streak = state->mode == COMBAT_CONTROLLER_MODE_COMBAT ? 1 : 0;
    input.branch_pressure = (int)fminf(100.0f, state->combat_timer * 8.0f);
    input.dependency_pulse = 1;
    input.workflow_depth = 2;
    input.neural_relevance_score = 12;
    input.network_dimensions = 3;
    input.model_confidence = 75;
    input.policy_momentum = 5;
    input.assignment_family = RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE;
    input.spectral_mode = RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED;
    input.hardware_byte_order_tag = RUNTIME_K3H4_BYTE_ORDER_TAG;
    input.hardware_dtype_tag = RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED;
    input.hardware_alignment_bytes = RUNTIME_K3H4_ALIGNMENT_BYTES_4;

    /* Ask the ABI layer to synthesize a deterministic k3h4 output from the signal.
     * If the ABI contract fails, return a safe zero bias rather than propagating
     * garbage into gameplay logic. */
    if (runtime_k3h4_abi_build_k3h4(input, &output) != 0)
        return 0.0f;

    /* Normalize the deterministic hash into a bounded [0, 1] bias value.
     * This keeps the bias usable by the movement heuristic without introducing
     * additional branching or sign logic in the controller loop. */
    normalized = (float)(output.observability.deterministic_hash & 0xFFFF) / 65535.0f;
    return normalized;
}

/*
 * Gameplay update path for the combat controller.
 * It validates the controller identity, exits early when the instance is idle,
 * and moves the combatant toward the target with a small k3h4 bias influence.
 */
static void combat_update(ControllerInstance *self, float dt)
{
    CombatControllerState *state = NULL;

    /* Reject invalid runtime pointers before touching controller memory. */
    if (!self || !self->state)
        return;

    /* Only the combat controller type is allowed to enter this update path.
     * This protects tests and gameplay code from accidentally mutating a different
     * controller implementation through the same callback entry point. */
    if (!combat_controller_is_combat_type(self))
        return;

    state = (CombatControllerState *)self->state;

    /* Idle combatants should not move or refresh k3h4 bias during a frame. */
    if (state->mode != COMBAT_CONTROLLER_MODE_COMBAT)
        return;

    {
        float dx = state->target[0] - self->position[0];
        float dz = state->target[2] - self->position[2];
        float distance_sq = (dx * dx) + (dz * dz);
        float distance = 0.0f;
        float step = 8.0f * dt;
        float orchestration_bias = state->k3h4_bias;

        /* Drain the cooldown timer on every update tick so cached k3h4 bias stays fresh
         * even when the combatant is already at the target or movement is otherwise suppressed. */
        state->k3h4_bias_cooldown -= dt;
        if (state->k3h4_bias_cooldown <= 0.0f)
        {
            state->k3h4_bias_cooldown = 0.25f;
            orchestration_bias = combat_controller_compute_bias(state);
            state->k3h4_bias = orchestration_bias;
        }

        /* Only move when the target is meaningfully far away.
         * This avoids tiny numerical jitter when the combatant is already in range. */
        if (distance_sq > 0.0001f)
        {
            distance = sqrtf(distance_sq);

            /* Give high-bias engagements a stronger movement push.
             * This is the gameplay-facing effect of the k3h4 signal. */
            if (orchestration_bias > 0.5f)
                step += orchestration_bias * 2.0f;

            /* Clamp the movement step so the combatant never overshoots the target. */
            if (step > distance)
                step = distance;

            /* Apply the movement delta along the target direction. */
            self->position[0] += (dx / distance) * step;
            self->position[2] += (dz / distance) * step;
        }
    }

    /* Continue the engagement timer and return to idle once the combat window expires. */
    state->combat_timer -= dt;
    if (state->combat_timer <= 0.0f)
        state->mode = COMBAT_CONTROLLER_MODE_IDLE;
}

/*
 * Handle gameplay signals that change the controller's intent.
 * "battle_engage" starts combat mode and resets the cached bias state, while
 * "death" clears the active engagement and returns the controller to idle.
 */
static void combat_signal(ControllerInstance *self, const char *signal, const void *data)
{
    CombatControllerState *state = NULL;

    /* Defensive checks before mutating any controller state. */
    if (!self || !self->state || !signal)
        return;

    /* Only the combat controller instance may process these gameplay signals. */
    if (!combat_controller_is_combat_type(self))
        return;

    state = (CombatControllerState *)self->state;

    /* Start a combat engagement by copying the target coordinates into the state.
     * This is the gameplay intent that drives the update loop during the next frames. */
    if (strcmp(signal, "battle_engage") == 0 && data)
    {
        const float *target = (const float *)data;
        state->target[0] = target[0];
        state->target[1] = target[1];
        state->target[2] = target[2];
        state->combat_timer = 6.0f;
        state->mode = COMBAT_CONTROLLER_MODE_COMBAT;

        /* Prime the cached bias at engage time so callers observe the current
         * k3h4 hint immediately, rather than a misleading zero until the next
         * update tick refreshes the cache. */
        state->k3h4_bias = combat_controller_compute_bias(state);
        state->k3h4_bias_cooldown = 0.25f;
    }
    /* Clear the active engagement when the unit dies.
     * This resets the controller to idle and removes any stale bias state. */
    else if (strcmp(signal, "death") == 0)
    {
        state->mode = COMBAT_CONTROLLER_MODE_IDLE;
        state->combat_timer = 0.0f;
        state->k3h4_bias = 0.0f;
        state->k3h4_bias_cooldown = 0.0f;
    }
}

/*
 * Free the per-instance combat state.
 * This must stay safe even if the controller instance has been mutated or
 * miswired with a mismatched type_name, because the outer destroy path owns the
 * instance lifetime and should not leak heap state on cleanup.
 */
static void combat_destroy(ControllerInstance *self)
{
    if (!self)
        return;

    if (self->state)
    {
        free(self->state);
        self->state = NULL;
    }
}

/*
 * Create a fresh combat controller instance.
 * The factory wires together the runtime callbacks, sets the combat identity,
 * and initializes the controller position for gameplay use.
 */
static ControllerInstance *combat_factory(float x, float y, float z)
{
    ControllerInstance *instance = NULL;
    CombatControllerState *state = NULL;

    instance = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    state = (CombatControllerState *)calloc(1, sizeof(CombatControllerState));
    if (!instance || !state)
    {
        free(instance);
        free(state);
        return NULL;
    }

    state->mode = COMBAT_CONTROLLER_MODE_IDLE;
    instance->state = state;
    instance->update = combat_update;
    instance->on_signal = combat_signal;
    instance->destroy = combat_destroy;
    strncpy(instance->type_name, "combat", sizeof(instance->type_name) - 1);
    instance->position[0] = x;
    instance->position[1] = y;
    instance->position[2] = z;

    return instance;
}

/*
 * Register the combat controller type with the runtime registry.
 * This is the entry point used by gameplay code to instantiate combat AI.
 */
void combat_controller_register(void)
{
    controller_register("combat", combat_factory);
}

/*
 * Snapshot the combat controller's debug-visible state for regression tests/instrumentation.
 */
void combat_controller_debug_snapshot(const ControllerInstance *controller,
                                      CombatControllerDebugSnapshot *snapshot)
{
    const CombatControllerState *state = NULL;

    if (!controller || !snapshot)
        return;

    memset(snapshot, 0, sizeof(*snapshot));

    if (!combat_controller_is_combat_type(controller))
        return;

    state = (const CombatControllerState *)controller->state;
    if (!state)
        return;
    snapshot->target[0] = state->target[0];
    snapshot->target[1] = state->target[1];
    snapshot->target[2] = state->target[2];
    snapshot->combat_timer = state->combat_timer;
    snapshot->mode = combat_controller_debug_mode(controller);
    snapshot->k3h4_bias = state->k3h4_bias;
    snapshot->k3h4_bias_cooldown = state->k3h4_bias_cooldown;
}

void combat_controller_debug_restore(ControllerInstance *controller,
                                     const CombatControllerDebugSnapshot *snapshot)
{
    CombatControllerState *state = NULL;

    if (!controller || !snapshot)
        return;

    if (!combat_controller_is_combat_type(controller))
        return;

    state = (CombatControllerState *)controller->state;
    if (!state)
        return;
    state->target[0] = snapshot->target[0];
    state->target[1] = snapshot->target[1];
    state->target[2] = snapshot->target[2];
    state->combat_timer = snapshot->combat_timer;
    state->k3h4_bias = snapshot->k3h4_bias;
    state->k3h4_bias_cooldown = snapshot->k3h4_bias_cooldown;

    if (snapshot->mode && strcmp(snapshot->mode, "combat") == 0)
        state->mode = COMBAT_CONTROLLER_MODE_COMBAT;
    else
        state->mode = COMBAT_CONTROLLER_MODE_IDLE;
}

float combat_controller_k3h4_bias(const ControllerInstance *controller)
{
    const CombatControllerState *state = NULL;

    if (!controller || !controller->state)
        return 0.0f;

    if (!combat_controller_is_combat_type(controller))
        return 0.0f;

    state = (const CombatControllerState *)controller->state;
    return state->k3h4_bias;
}

/*
 * Return a debug label for the current combat mode.
 * This is used by tests and instrumentation to verify whether the controller is
 * actively engaged or has returned to idle.
 */
const char *combat_controller_debug_mode(const ControllerInstance *controller)
{
    const CombatControllerState *state = NULL;

    if (!controller)
        return NULL;

    if (!combat_controller_is_combat_type(controller))
        return NULL;

    state = (const CombatControllerState *)controller->state;
    if (!state)
        return NULL;

    return state->mode == COMBAT_CONTROLLER_MODE_COMBAT ? "combat" : "idle";
}