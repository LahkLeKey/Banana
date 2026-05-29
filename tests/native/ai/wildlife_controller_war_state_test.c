#include "ai/controller.h"
#include "ai/wildlife_controller.h"

#include <stdio.h>

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
    float scout_target[3] = {12.0f, 0.0f, 0.0f};
    float enemy_target[3] = {-12.0f, 0.0f, 0.0f};

    wildlife_controller_register();
    controller = controller_create("wildlife", 0.0f, 0.0f, 0.0f);

    if (!expect_true("controller created", controller != NULL))
        return 1;

    controller_signal(controller, "player_nearby", scout_target);
    if (!expect_true("player signal enters investigate state",
                     wildlife_controller_debug_state_name(controller) != NULL &&
                         strcmp(wildlife_controller_debug_state_name(controller), "investigate") == 0))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_update(controller, 0.5f);

    controller_signal(controller, "battle_engage", enemy_target);
    if (!expect_true("battle signal enters combat state",
                     wildlife_controller_debug_state_name(controller) != NULL &&
                         strcmp(wildlife_controller_debug_state_name(controller), "combat") == 0))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_update(controller, 0.5f);

    if (!expect_true("combat pursuit moved wildlife", controller->position[0] != 0.0f || controller->position[2] != 0.0f))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_destroy(controller);

    printf("wildlife_controller_war_state_test: pass\n");
    return 0;
}
