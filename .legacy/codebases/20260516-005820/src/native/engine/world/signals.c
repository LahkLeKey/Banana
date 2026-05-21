#include "signals.h"
#include "../ai/controller.h"
#include "world.h"
#include <stdio.h>
#include <string.h>

static SignalQueue s_queue;
static int s_initialized = 0;

SignalQueue *signals_get_queue(void)
{
    if (!s_initialized)
    {
        memset(&s_queue, 0, sizeof(s_queue));
        s_initialized = 1;
    }
    return &s_queue;
}

void signals_send(uint32_t target_id, const char *name, const char *data)
{
    SignalQueue *q = signals_get_queue();
    if (q->count >= BANANA_MAX_SIGNALS)
    {
        fprintf(stderr, "[engine/signals] queue full — signal dropped: %s\n", name);
        return;
    }
    Signal *s = &q->items[(q->head + q->count) % BANANA_MAX_SIGNALS];
    s->target_id = target_id;
    strncpy(s->name, name, BANANA_SIGNAL_NAME_LEN - 1);
    if (data)
        strncpy(s->data, data, BANANA_SIGNAL_DATA_LEN - 1);
    else
        s->data[0] = '\0';
    q->count++;
}

void signals_flush(struct World *world)
{
    SignalQueue *q = signals_get_queue();
    while (q->count > 0)
    {
        Signal *sig = &q->items[q->head];
        Entity *e = world_get_entity(world, sig->target_id);
        if (e && e->controller_id)
        {
            ControllerInstance *controller = engine_controller_find(e->controller_id);
            if (controller)
                controller_signal(controller, sig->name, sig->data[0] ? sig->data : NULL);
        }
        q->head = (q->head + 1) % BANANA_MAX_SIGNALS;
        q->count--;
    }
}
