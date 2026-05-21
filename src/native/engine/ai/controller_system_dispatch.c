#include "controller_system.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP) && !defined(__INTELLISENSE__)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

#include <string.h>

void controller_system_update(ControllerSystem *sys, float dt)
{
    int i = 0;

    if (!sys)
        return;

#pragma omp parallel for schedule(dynamic, 4)
    for (i = 0; i < sys->count; i++)
        controller_update(sys->instances[i], dt);
}

void controller_system_signal_all(ControllerSystem *sys, const char *signal, const void *data)
{
    int i = 0;

    if (!sys)
        return;

#pragma omp parallel for schedule(guided, 8)
    for (i = 0; i < sys->count; i++)
        controller_signal(sys->instances[i], signal, data);
}

void controller_system_signal_type(ControllerSystem *sys, const char *type, const char *signal,
                                   const void *data)
{
    int i = 0;

    if (!sys)
        return;

#pragma omp parallel for schedule(guided, 8)
    for (i = 0; i < sys->count; i++)
    {
        ControllerInstance *c = sys->instances[i];
        if (!type || strncmp(c->type_name, type, 63) == 0)
            controller_signal(c, signal, data);
    }
}
