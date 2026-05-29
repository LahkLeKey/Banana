#include "ai/controller.h"
#include "runtime/controller/runtime/controller_runtime.h"

#include <stdio.h>
#include <string.h>

typedef struct ControllerWarSignalState
{
    int signal_count;
    char last_signal[32];
    float last_target[3];
} ControllerWarSignalState;

static void test_update(ControllerInstance *self, float dt)
{
    (void)self;
    (void)dt;
}

static void test_signal(ControllerInstance *self, const char *signal, const void *data)
{
    ControllerWarSignalState *state = NULL;

    if (!self || !self->state)
        return;

    state = (ControllerWarSignalState *)self->state;
    state->signal_count += 1;

    if (signal)
    {
        strncpy(state->last_signal, signal, sizeof(state->last_signal) - 1);
        state->last_signal[sizeof(state->last_signal) - 1] = '\0';
    }

    if (data)
    {
        const float *target = (const float *)data;
        state->last_target[0] = target[0];
        state->last_target[1] = target[1];
        state->last_target[2] = target[2];
    }
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
    ControllerInstance controller_a;
    ControllerInstance controller_b;
    ControllerInstance controller_c;
    ControllerWarSignalState state_a;
    ControllerWarSignalState state_b;
    ControllerWarSignalState state_c;
    ControllerInstance *controllers[3] = {&controller_a, &controller_b, &controller_c};

    memset(&controller_a, 0, sizeof(controller_a));
    memset(&controller_b, 0, sizeof(controller_b));
    memset(&controller_c, 0, sizeof(controller_c));
    memset(&state_a, 0, sizeof(state_a));
    memset(&state_b, 0, sizeof(state_b));
    memset(&state_c, 0, sizeof(state_c));

    controller_a.id = 1;
    controller_a.update = test_update;
    controller_a.on_signal = test_signal;
    controller_a.state = &state_a;
    controller_a.position[0] = 0.0f;
    controller_a.position[1] = 0.0f;
    controller_a.position[2] = 0.0f;

    controller_b.id = 2;
    controller_b.update = test_update;
    controller_b.on_signal = test_signal;
    controller_b.state = &state_b;
    controller_b.position[0] = 3.0f;
    controller_b.position[1] = 0.0f;
    controller_b.position[2] = 0.0f;

    controller_c.id = 3;
    controller_c.update = test_update;
    controller_c.on_signal = test_signal;
    controller_c.state = &state_c;
    controller_c.position[0] = 50.0f;
    controller_c.position[1] = 0.0f;
    controller_c.position[2] = 0.0f;

    if (!expect_int("assign team banana",
                    runtime_controller_assign_team_by_id(controllers, 3, 1, CONTROLLER_TEAM_BANANA),
                    1))
    {
        return 1;
    }

    if (!expect_int("assign team bean",
                    runtime_controller_assign_team_by_id(controllers, 3, 2, CONTROLLER_TEAM_BEAN),
                    1))
    {
        return 1;
    }

    if (!expect_int("assign team bean far",
                    runtime_controller_assign_team_by_id(controllers, 3, 3, CONTROLLER_TEAM_BEAN),
                    1))
    {
        return 1;
    }

    if (!expect_int("default signal enemies in range",
                    runtime_controller_signal_team_war(controllers, 3, 5.0f, NULL),
                    2))
    {
        return 1;
    }

    if (!expect_int("banana received one signal", state_a.signal_count, 1))
        return 1;

    if (!expect_int("bean received one signal", state_b.signal_count, 1))
        return 1;

    if (!expect_int("far bean not signaled", state_c.signal_count, 0))
        return 1;

    if (!expect_int("default signal label",
                    strcmp(state_a.last_signal, "enemy_spotted") == 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("banana target points to bean",
                    (state_a.last_target[0] == controller_b.position[0] &&
                     state_a.last_target[2] == controller_b.position[2])
                        ? 1
                        : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("custom war signal",
                    runtime_controller_signal_team_war(controllers, 3, 5.0f, "battle_engage"),
                    2))
    {
        return 1;
    }

    if (!expect_int("custom signal label",
                    strcmp(state_b.last_signal, "battle_engage") == 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("switch bean to banana",
                    runtime_controller_assign_team_by_id(controllers, 3, 2, CONTROLLER_TEAM_BANANA),
                    1))
    {
        return 1;
    }

    if (!expect_int("same-team ceasefire",
                    runtime_controller_signal_team_war(controllers, 3, 5.0f, "battle_engage"),
                    0))
    {
        return 1;
    }

    if (!expect_int("missing controller assign returns zero",
                    runtime_controller_assign_team_by_id(controllers, 3, 99, CONTROLLER_TEAM_BEAN),
                    0))
    {
        return 1;
    }

    printf("runtime_controller_team_war_test: pass\n");
    return 0;
}
