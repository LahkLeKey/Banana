#include "player_controller.h"

#include <stdlib.h>
#include <string.h>

typedef struct
{
    float move_x;
    float move_z;
    int has_move_input;
    float last_action_x;
    float last_action_y;
    int has_action;
} PlayerControllerState;

static int parse_move_payload(const char *data, float *move_x, float *move_z)
{
    char *end = NULL;
    float parsed_x = 0.f;
    float parsed_z = 0.f;

    if (!data || !move_x || !move_z)
        return 0;

    parsed_x = strtof(data, &end);
    if (end == data)
        return 0;
    if (*end == ',')
        end++;
    parsed_z = strtof(end, &end);
    if (end == data)
        return 0;

    *move_x = parsed_x;
    *move_z = parsed_z;
    return 1;
}

static void player_update(ControllerInstance *self, float dt)
{
    (void)self;
    (void)dt;
}

static void player_on_signal(ControllerInstance *self, const char *signal, const void *data)
{
    PlayerControllerState *state = self ? (PlayerControllerState *)self->state : NULL;
    const char *payload = (const char *)data;

    if (!state || !signal)
        return;

    if (strcmp(signal, "move_input") == 0)
    {
        float move_x = 0.f;
        float move_z = 0.f;
        if (parse_move_payload(payload, &move_x, &move_z))
        {
            state->move_x = move_x;
            state->move_z = move_z;
            state->has_move_input = 1;
        }
        return;
    }

    if (strcmp(signal, "clear_input") == 0)
    {
        state->move_x = 0.f;
        state->move_z = 0.f;
        state->has_move_input = 0;
        state->has_action = 0;
        return;
    }

    if (strcmp(signal, "interact") == 0)
    {
        float action_x = 0.f;
        float action_y = 0.f;
        if (parse_move_payload(payload, &action_x, &action_y))
        {
            state->last_action_x = action_x;
            state->last_action_y = action_y;
        }
        state->has_action = 1;
        return;
    }
}

static void player_destroy(ControllerInstance *self)
{
    if (!self)
        return;
    free(self->state);
    self->state = NULL;
}

static ControllerInstance *player_factory(float x, float y, float z)
{
    ControllerInstance *controller = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    PlayerControllerState *state = (PlayerControllerState *)calloc(1, sizeof(PlayerControllerState));

    if (!controller || !state)
    {
        free(controller);
        free(state);
        return NULL;
    }

    controller->state = state;
    controller->position[0] = x;
    controller->position[1] = y;
    controller->position[2] = z;
    strncpy(controller->type_name, "player", sizeof(controller->type_name) - 1);
    controller->update = player_update;
    controller->on_signal = player_on_signal;
    controller->destroy = player_destroy;
    return controller;
}

void player_controller_register(void)
{
    controller_register("player", player_factory);
}

void player_controller_set_move_input(ControllerInstance *self, float move_x, float move_z)
{
    PlayerControllerState *state = self ? (PlayerControllerState *)self->state : NULL;

    if (!state)
        return;

    state->move_x = move_x;
    state->move_z = move_z;
    state->has_move_input = 1;
}

int player_controller_get_move_input(const ControllerInstance *self, float *move_x,
                                     float *move_z)
{
    const PlayerControllerState *state = self ? (const PlayerControllerState *)self->state : NULL;

    if (!state || !move_x || !move_z)
        return 0;

    *move_x = state->move_x;
    *move_z = state->move_z;
    return 1;
}