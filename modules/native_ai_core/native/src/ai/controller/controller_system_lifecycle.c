#include "controller_system.h"

#include <stdio.h>
#include <stdlib.h>

static void *controller_system_default_calloc(size_t count, size_t size)
{
    return calloc(count, size);
}

static ControllerSystemCallocFn s_controller_system_calloc_hook = controller_system_default_calloc;

void controller_system_set_calloc_hook(ControllerSystemCallocFn hook)
{
    s_controller_system_calloc_hook = hook ? hook : controller_system_default_calloc;
}

void controller_system_reset_calloc_hook(void)
{
    s_controller_system_calloc_hook = controller_system_default_calloc;
}

ControllerSystem *controller_system_create(void)
{
    ControllerSystem *s = (ControllerSystem *)s_controller_system_calloc_hook(1, sizeof(ControllerSystem));
    if (!s)
        return NULL;

    s->next_id = 1;
    return s;
}

uint32_t controller_system_spawn(ControllerSystem *sys, const char *type, float x, float y, float z)
{
    if (!sys || sys->count >= BANANA_CTRL_SYS_CAPACITY)
    {
        fprintf(stderr, "[ctrl_sys] capacity exceeded\n");
        return 0;
    }

    ControllerInstance *c = controller_create(type, x, y, z);
    if (!c)
        return 0;

    c->id = sys->next_id++;
    sys->instances[sys->count++] = c;
    return c->id;
}

void controller_system_destroy(ControllerSystem *sys)
{
    if (!sys)
        return;

    for (int i = 0; i < sys->count; i++)
    {
        controller_destroy(sys->instances[i]);
        sys->instances[i] = NULL;
    }

    free(sys);
}
