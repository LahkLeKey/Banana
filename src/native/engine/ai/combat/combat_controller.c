#include "combat_controller.h"

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