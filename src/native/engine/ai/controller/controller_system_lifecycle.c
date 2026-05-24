#include "controller_system.h"

#include <stdio.h>
#include <stdlib.h>

ControllerSystem *controller_system_create(void)
{
    ControllerSystem *s = (ControllerSystem *)calloc(1, sizeof(ControllerSystem));
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
