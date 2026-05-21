#include "ml_agent_controller.h"

#include "controller.h"

#include <stdlib.h>
#include <string.h>

typedef struct
{
    float reserved;
} MlAgentState;

static void ml_agent_update(ControllerInstance *self, float dt)
{
    (void)self;
    (void)dt;
}

static void ml_agent_signal(ControllerInstance *self, const char *signal, const void *data)
{
    (void)self;
    (void)signal;
    (void)data;
}

static void ml_agent_destroy(ControllerInstance *self)
{
    if (!self)
        return;
    free(self->state);
    self->state = NULL;
}

static ControllerInstance *ml_agent_factory(float x, float y, float z)
{
    ControllerInstance *controller = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    MlAgentState *state = (MlAgentState *)calloc(1, sizeof(MlAgentState));

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
    strncpy(controller->type_name, "ml-agent", sizeof(controller->type_name) - 1);
    controller->update = ml_agent_update;
    controller->on_signal = ml_agent_signal;
    controller->destroy = ml_agent_destroy;

    return controller;
}

void ml_agent_controller_register(void)
{
    controller_register("ml-agent", ml_agent_factory);
}