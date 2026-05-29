#include "runtime/tick/tick_post_phase.h"

#include <stdio.h>
#include <string.h>

static int s_follow_calls = 0;
static int s_render_calls = 0;
static int s_sequence_next = 0;
static int s_follow_sequence = -1;
static int s_render_sequence = -1;
static int s_controller_signal_count = 0;

typedef struct TickControllerSignalState
{
    int signal_count;
    char last_signal[32];
} TickControllerSignalState;

static void follow_cb(void *context)
{
    (void)context;
    s_follow_calls += 1;
    s_follow_sequence = s_sequence_next++;
}

static void render_cb(void *context)
{
    (void)context;
    s_render_calls += 1;
    s_render_sequence = s_sequence_next++;
}

static void test_controller_signal(ControllerInstance *self, const char *signal, const void *data)
{
    TickControllerSignalState *state = NULL;
    (void)data;

    s_controller_signal_count += 1;

    if (!self || !self->state)
        return;

    state = (TickControllerSignalState *)self->state;
    state->signal_count += 1;
    if (signal)
    {
        strncpy(state->last_signal, signal, sizeof(state->last_signal) - 1);
        state->last_signal[sizeof(state->last_signal) - 1] = '\0';
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
    runtime_tick_post_phase_execute(NULL,
                                    NULL,
                                    0,
                                    1.0f / 60.0f,
                                    NULL,
                                    follow_cb,
                                    render_cb);

    if (!expect_int("follow called", s_follow_calls, 1))
        return 1;
    if (!expect_int("render called", s_render_calls, 1))
        return 1;

    s_follow_calls = 0;
    s_render_calls = 0;

    runtime_tick_post_phase_execute(NULL,
                                    NULL,
                                    0,
                                    1.0f / 60.0f,
                                    NULL,
                                    NULL,
                                    NULL);

    if (!expect_int("follow skipped when null", s_follow_calls, 0))
        return 1;
    if (!expect_int("render skipped when null", s_render_calls, 0))
        return 1;

    /* Seam: controller sync must be invoked before follow/render even with zero controllers. */
    s_follow_calls = 0;
    s_render_calls = 0;
    s_sequence_next = 0;
    s_follow_sequence = -1;
    s_render_sequence = -1;

    runtime_tick_post_phase_execute(NULL,
                                    NULL,
                                    0,
                                    1.0f / 60.0f,
                                    NULL,
                                    follow_cb,
                                    render_cb);

    if (!expect_int("follow ordering precedes render", s_follow_sequence, 0))
        return 1;
    if (!expect_int("render ordering follows follow", s_render_sequence, 1))
        return 1;

    /* Boundary: dt == 0 still dispatches camera follow + render scene. */
    s_follow_calls = 0;
    s_render_calls = 0;

    runtime_tick_post_phase_execute(NULL,
                                    NULL,
                                    0,
                                    0.0f,
                                    NULL,
                                    follow_cb,
                                    render_cb);

    if (!expect_int("follow runs on dt=0", s_follow_calls, 1))
        return 1;
    if (!expect_int("render runs on dt=0", s_render_calls, 1))
        return 1;

    /* Null-guard: only follow provided still produces a single follow call without crashing. */
    s_follow_calls = 0;
    s_render_calls = 0;

    runtime_tick_post_phase_execute(NULL,
                                    NULL,
                                    0,
                                    1.0f / 60.0f,
                                    NULL,
                                    follow_cb,
                                    NULL);

    if (!expect_int("follow runs when render is null", s_follow_calls, 1))
        return 1;
    if (!expect_int("render stays zero when render is null", s_render_calls, 0))
        return 1;

    /* Nearby hostile teams auto-engage during post phase. */
    {
        ControllerInstance banana;
        ControllerInstance bean;
        ControllerInstance neutral;
        ControllerInstance *controllers[3] = {&banana, &bean, &neutral};
        TickControllerSignalState banana_state;
        TickControllerSignalState bean_state;
        TickControllerSignalState neutral_state;

        memset(&banana, 0, sizeof(banana));
        memset(&bean, 0, sizeof(bean));
        memset(&neutral, 0, sizeof(neutral));
        memset(&banana_state, 0, sizeof(banana_state));
        memset(&bean_state, 0, sizeof(bean_state));
        memset(&neutral_state, 0, sizeof(neutral_state));

        banana.id = 1;
        banana.team = CONTROLLER_TEAM_BANANA;
        banana.position[0] = 0.0f;
        banana.position[2] = 0.0f;
        banana.on_signal = test_controller_signal;
        banana.state = &banana_state;

        bean.id = 2;
        bean.team = CONTROLLER_TEAM_BEAN;
        bean.position[0] = 4.0f;
        bean.position[2] = 0.0f;
        bean.on_signal = test_controller_signal;
        bean.state = &bean_state;

        neutral.id = 3;
        neutral.team = CONTROLLER_TEAM_NEUTRAL;
        neutral.position[0] = 2.0f;
        neutral.position[2] = 0.0f;
        neutral.on_signal = test_controller_signal;
        neutral.state = &neutral_state;

        s_controller_signal_count = 0;
        runtime_tick_post_phase_execute(NULL,
                                        controllers,
                                        3,
                                        1.0f / 60.0f,
                                        NULL,
                                        NULL,
                                        NULL);

        if (!expect_int("battle signal count", s_controller_signal_count, 2))
            return 1;
        if (!expect_int("banana battle signal", banana_state.signal_count, 1))
            return 1;
        if (!expect_int("bean battle signal", bean_state.signal_count, 1))
            return 1;
        if (!expect_int("neutral no battle signal", neutral_state.signal_count, 0))
            return 1;
        if (!expect_int("battle signal name", strcmp(banana_state.last_signal, "battle_engage") == 0 ? 1 : 0, 1))
            return 1;
    }

    return 0;
}
