#include "ai/controller.h"
#include "ai/controller_system.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int s_update_count = 0;
static int s_signal_count = 0;

typedef struct
{
    int touched;
} TestState;

static void test_update(ControllerInstance *self, float dt)
{
    (void)dt;
    if (self && self->state)
        ((TestState *)self->state)->touched += 1;
    s_update_count += 1;
}

static void test_signal(ControllerInstance *self, const char *signal, const void *data)
{
    (void)self;
    (void)signal;
    (void)data;
    s_signal_count += 1;
}

static void test_destroy(ControllerInstance *self)
{
    if (self && self->state)
    {
        free(self->state);
        self->state = NULL;
    }
}

static ControllerInstance *test_factory(float x, float y, float z)
{
    ControllerInstance *inst = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    TestState *state = (TestState *)calloc(1, sizeof(TestState));
    if (!inst || !state)
    {
        free(inst);
        free(state);
        return NULL;
    }

    inst->state = state;
    inst->update = test_update;
    inst->on_signal = test_signal;
    inst->destroy = test_destroy;
    strncpy(inst->type_name, "test_ctrl", sizeof(inst->type_name) - 1);
    inst->position[0] = x;
    inst->position[1] = y;
    inst->position[2] = z;
    return inst;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    controller_register("test_ctrl", test_factory);

    ControllerSystem *sys = controller_system_create();
    if (!expect_int("system created", sys != NULL ? 1 : 0, 1))
        return 1;

    if (!expect_int("spawn a", controller_system_spawn(sys, "test_ctrl", 0, 0, 0) != 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("spawn b", controller_system_spawn(sys, "test_ctrl", 1, 0, 0) != 0 ? 1 : 0, 1))
        return 1;

    controller_system_update(sys, 1.0f / 60.0f);
    if (!expect_int("update count", s_update_count, 2))
        return 1;

    controller_system_signal_all(sys, "poke", NULL);
    if (!expect_int("signal all", s_signal_count, 2))
        return 1;

    controller_system_signal_type(sys, "test_ctrl", "poke", NULL);
    if (!expect_int("signal type", s_signal_count, 4))
        return 1;

    controller_system_destroy(sys);
    return 0;
}
