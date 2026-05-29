#include "ai/combat_controller.h"
#include "ai/controller.h"

#include <stdio.h>
#include <string.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    ControllerInstance *controller = NULL;
    float hostile_target[3] = {8.0f, 0.0f, 0.0f};

    combat_controller_register();
    controller = controller_create("combat", 0.0f, 0.0f, 0.0f);

    if (!expect_true("combat controller created", controller != NULL))
        return 1;

    if (!expect_true("combat starts idle",
                     combat_controller_debug_mode(controller) != NULL &&
                         strcmp(combat_controller_debug_mode(controller), "idle") == 0))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_signal(controller, "battle_engage", hostile_target);

    if (!expect_true("battle engage enters combat mode",
                     combat_controller_debug_mode(controller) != NULL &&
                         strcmp(combat_controller_debug_mode(controller), "combat") == 0))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_update(controller, 0.5f);
    if (!expect_true("combat update advances toward target", controller->position[0] > 0.0f))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_destroy(controller);
    printf("combat_controller_signal_test: pass\n");
    return 0;
}