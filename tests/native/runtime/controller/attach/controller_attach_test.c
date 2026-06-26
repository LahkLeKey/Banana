#include "runtime/controller/attach/controller_attach.h"

#include "runtime/support/test_support.h"

#include <string.h>

static ControllerInstance g_controllers_storage[8];
static int g_controller_create_calls = 0;

ControllerInstance *controller_create(const char *type, float x, float y, float z)
{
    ControllerInstance *instance = NULL;

    (void)x;
    (void)y;
    (void)z;

    g_controller_create_calls += 1;
    if (!type || strcmp(type, "missing") == 0)
        return NULL;

    instance = &g_controllers_storage[(g_controller_create_calls - 1) % 8];
    memset(instance, 0, sizeof(*instance));
    strncpy(instance->type_name, type, sizeof(instance->type_name) - 1);
    return instance;
}

Entity *world_get_entity(World *world, EntityId id)
{
    int i = 0;

    if (!world || id == 0)
        return NULL;

    for (i = 0; i < world->entity_count; i++)
    {
        Entity *entity = world->entities[i];
        if (entity && entity->id == id)
            return entity;
    }

    return NULL;
}

static void test_create_and_register_with_team_validates_inputs(void **state)
{
    (void)state;
    ControllerInstance *controllers[2] = {0};
    int count = 0;

    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_create_and_register_with_team(NULL,
                                                                               2,
                                                                               &count,
                                                                               "combat",
                                                                               0.0f,
                                                                               0.0f,
                                                                               0.0f,
                                                                               CONTROLLER_TEAM_BANANA),
                              0,
                              "null controller arrays must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_create_and_register_with_team(controllers,
                                                                               2,
                                                                               NULL,
                                                                               "combat",
                                                                               0.0f,
                                                                               0.0f,
                                                                               0.0f,
                                                                               CONTROLLER_TEAM_BANANA),
                              0,
                              "null controller count pointers must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_create_and_register_with_team(controllers,
                                                                               2,
                                                                               &count,
                                                                               NULL,
                                                                               0.0f,
                                                                               0.0f,
                                                                               0.0f,
                                                                               CONTROLLER_TEAM_BANANA),
                              0,
                              "null controller type must be rejected");

    count = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_create_and_register_with_team(controllers,
                                                                               2,
                                                                               &count,
                                                                               "combat",
                                                                               0.0f,
                                                                               0.0f,
                                                                               0.0f,
                                                                               CONTROLLER_TEAM_BANANA),
                              0,
                              "negative controller count must be rejected");

    count = 2;
    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_create_and_register_with_team(controllers,
                                                                               2,
                                                                               &count,
                                                                               "combat",
                                                                               0.0f,
                                                                               0.0f,
                                                                               0.0f,
                                                                               CONTROLLER_TEAM_BANANA),
                              0,
                              "full controller arrays must be rejected");

    count = 0;
    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_create_and_register_with_team(controllers,
                                                                               2,
                                                                               &count,
                                                                               "missing",
                                                                               0.0f,
                                                                               0.0f,
                                                                               0.0f,
                                                                               CONTROLLER_TEAM_BANANA),
                              0,
                              "controller factory failures must propagate as zero ids");
}

static void test_create_and_register_default_team_mapping(void **state)
{
    (void)state;
    ControllerInstance *controllers[4] = {0};
    int count = 0;
    uint32_t id = 0;

    g_controller_create_calls = 0;

    id = runtime_controller_create_and_register(controllers,
                                                4,
                                                &count,
                                                "combat",
                                                1.0f,
                                                2.0f,
                                                3.0f);
    BANANA_TEST_ASSERT_INT_EQ((int)id, 1,
                              "first created controller id must be one-based");
    BANANA_TEST_ASSERT_INT_EQ((int)controllers[0]->team,
                              (int)CONTROLLER_TEAM_BANANA,
                              "combat controllers should default to banana team");

    id = runtime_controller_create_and_register(controllers,
                                                4,
                                                &count,
                                                "wildlife",
                                                4.0f,
                                                5.0f,
                                                6.0f);
    BANANA_TEST_ASSERT_INT_EQ((int)id, 2,
                              "second controller id should increment sequentially");
    BANANA_TEST_ASSERT_INT_EQ((int)controllers[1]->team,
                              (int)CONTROLLER_TEAM_BEAN,
                              "wildlife controllers should default to bean team");

    id = runtime_controller_create_and_register(controllers,
                                                4,
                                                &count,
                                                "utility",
                                                7.0f,
                                                8.0f,
                                                9.0f);
    BANANA_TEST_ASSERT_INT_EQ((int)id, 3,
                              "third controller id should increment sequentially");
    BANANA_TEST_ASSERT_INT_EQ((int)controllers[2]->team,
                              (int)CONTROLLER_TEAM_NEUTRAL,
                              "unrecognized controller types should default to neutral team");
}

static void test_attach_to_entity_paths(void **state)
{
    (void)state;
    ControllerInstance *controllers[4] = {0};
    World world = {0};
    Entity entity = { .id = 77, .active = 1 };
    int count = 0;
    uint32_t id = 0;

    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_attach_to_entity_with_team(NULL,
                                                                            controllers,
                                                                            4,
                                                                            &count,
                                                                            77,
                                                                            "combat",
                                                                            CONTROLLER_TEAM_BANANA),
                              0,
                              "null worlds must be rejected");

    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_attach_to_entity_with_team(&world,
                                                                            controllers,
                                                                            4,
                                                                            &count,
                                                                            77,
                                                                            "combat",
                                                                            CONTROLLER_TEAM_BANANA),
                              0,
                              "missing entities must be rejected");

    world.entities[0] = &entity;
    world.entity_count = 1;

    id = runtime_controller_attach_to_entity_with_team(&world,
                                                       controllers,
                                                       4,
                                                       &count,
                                                       77,
                                                       "combat",
                                                       CONTROLLER_TEAM_BANANA);

    BANANA_TEST_ASSERT_INT_EQ((int)id, 1,
                              "attach helper must create and return a controller id");
    BANANA_TEST_ASSERT_INT_EQ((int)entity.controller_id,
                              (int)id,
                              "attach helper must persist controller id on entity");
    BANANA_TEST_ASSERT_INT_EQ((int)controllers[0]->team,
                              (int)CONTROLLER_TEAM_BANANA,
                              "attach helper should preserve explicit team values");

    id = runtime_controller_attach_to_entity(&world,
                                             controllers,
                                             4,
                                             &count,
                                             77,
                                             "wildlife");
    BANANA_TEST_ASSERT_INT_EQ((int)id, 2,
                              "default-team attach should continue id sequencing");
    BANANA_TEST_ASSERT_INT_EQ((int)controllers[1]->team,
                              (int)CONTROLLER_TEAM_BEAN,
                              "default-team attach must map wildlife to bean team");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_create_and_register_with_team_validates_inputs),
    BANANA_TEST_CASE(test_create_and_register_default_team_mapping),
    BANANA_TEST_CASE(test_attach_to_entity_paths)
)
