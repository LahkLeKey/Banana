#include "../controller.h"
#include "../combat_controller.h"

#include "runtime/abi/netcode/netcode_abi.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef enum CombatControllerMode
{
    COMBAT_CONTROLLER_MODE_IDLE = 0,
    COMBAT_CONTROLLER_MODE_COMBAT = 1,
} CombatControllerMode;

typedef struct CombatControllerState
{
    float target[3];
    float combat_timer;
    CombatControllerMode mode;
} CombatControllerState;

static float combat_controller_compute_bias(const CombatControllerState *state)
{
    RuntimeK3h4VectorSignalInput input;
    RuntimeNetcodeK3h4Output output;
    float normalized = 0.0f;

    if (!state)
        return 0.0f;

    memset(&input, 0, sizeof(input));
    memset(&output, 0, sizeof(output));

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

    if (runtime_k3h4_abi_build_k3h4(input, &output) != 0)
        return 0.0f;

    normalized = (float)(output.observability.deterministic_hash & 0xFFFF) / 65535.0f;
    return normalized;
}

static void combat_update(ControllerInstance *self, float dt)
{
    CombatControllerState *state = NULL;

    if (!self || !self->state)
        return;

    state = (CombatControllerState *)self->state;
    if (state->mode != COMBAT_CONTROLLER_MODE_COMBAT)
        return;

    {
        float dx = state->target[0] - self->position[0];
        float dz = state->target[2] - self->position[2];
        float distance_sq = (dx * dx) + (dz * dz);

        if (distance_sq > 0.0001f)
        {
            float distance = sqrtf(distance_sq);
            float step = 8.0f * dt;
            float orchestration_bias = combat_controller_compute_bias(state);

            if (orchestration_bias > 0.5f)
                step += orchestration_bias * 2.0f;

            if (step > distance)
                step = distance;

            self->position[0] += (dx / distance) * step;
            self->position[2] += (dz / distance) * step;
        }
    }

    state->combat_timer -= dt;
    if (state->combat_timer <= 0.0f)
        state->mode = COMBAT_CONTROLLER_MODE_IDLE;
}

static void combat_signal(ControllerInstance *self, const char *signal, const void *data)
{
    CombatControllerState *state = NULL;

    if (!self || !self->state || !signal)
        return;

    state = (CombatControllerState *)self->state;

    if (strcmp(signal, "battle_engage") == 0 && data)
    {
        const float *target = (const float *)data;
        state->target[0] = target[0];
        state->target[1] = target[1];
        state->target[2] = target[2];
        state->combat_timer = 6.0f;
        state->mode = COMBAT_CONTROLLER_MODE_COMBAT;
    }
    else if (strcmp(signal, "death") == 0)
    {
        state->mode = COMBAT_CONTROLLER_MODE_IDLE;
        state->combat_timer = 0.0f;
    }
}

static void combat_destroy(ControllerInstance *self)
{
    if (!self)
        return;

    free(self->state);
    self->state = NULL;
}

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

void combat_controller_register(void)
{
    controller_register("combat", combat_factory);
}

float combat_controller_k3h4_bias(const ControllerInstance *controller)
{
    const CombatControllerState *state = NULL;

    if (!controller || !controller->state)
        return 0.0f;

    state = (const CombatControllerState *)controller->state;
    return combat_controller_compute_bias(state);
}

const char *combat_controller_debug_mode(const ControllerInstance *controller)
{
    const CombatControllerState *state = NULL;

    if (!controller)
        return NULL;

    if (strncmp(controller->type_name, "combat", sizeof(controller->type_name)) != 0)
        return NULL;

    state = (const CombatControllerState *)controller->state;
    if (!state)
        return NULL;

    return state->mode == COMBAT_CONTROLLER_MODE_COMBAT ? "combat" : "idle";
}