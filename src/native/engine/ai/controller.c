#include "controller.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Simple registry: maps type_name → factory function */
#define BANANA_MAX_CONTROLLER_TYPES 32

static struct {
    char             type_name[64];
    ControllerFactory factory;
} s_registry[BANANA_MAX_CONTROLLER_TYPES];

static int s_registry_count = 0;

/* Called by controller implementations to register themselves */
void controller_register(const char *type_name, ControllerFactory factory) {
    if (s_registry_count >= BANANA_MAX_CONTROLLER_TYPES) {
        fprintf(stderr, "[engine/ai] controller registry full\n");
        return;
    }
    strncpy(s_registry[s_registry_count].type_name, type_name, 63);
    s_registry[s_registry_count].factory = factory;
    s_registry_count++;
}

ControllerInstance *controller_create(const char *type_name,
                                       float x, float y, float z) {
    for (int i = 0; i < s_registry_count; i++) {
        if (strncmp(s_registry[i].type_name, type_name, 63) == 0)
            return s_registry[i].factory(x, y, z);
    }
    fprintf(stderr, "[engine/ai] unknown controller type: %s\n", type_name);
    return NULL;
}

void controller_update(ControllerInstance *c, float dt) {
    if (c && c->update) c->update(c, dt);
}

void controller_signal(ControllerInstance *c, const char *signal, const void *data) {
    if (c && c->on_signal) c->on_signal(c, signal, data);
}

void controller_destroy(ControllerInstance *c) {
    if (!c) return;
    if (c->destroy) c->destroy(c);
    free(c);
}

/* engine_controller_create / engine_controller_update / engine_controller_signal
 * are implemented in engine.c (singleton-backed).
 * They are NOT defined here to avoid duplicate-symbol errors. */
