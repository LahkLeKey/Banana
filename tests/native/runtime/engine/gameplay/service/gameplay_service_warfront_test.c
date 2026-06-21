#include "render/backend.h"
#include "ai/controller.h"
#include "world/entity.h"
#include "world/world.h"
#include "runtime/engine/gameplay/service/gameplay_service_warfront.h"
#include "../../../support/test_support.h"

#include <string.h>

static int g_hostile_override = 1;

int controller_teams_are_hostile(ControllerTeam first, ControllerTeam second)
{
    if (!g_hostile_override)
        return 0;

    return first != CONTROLLER_TEAM_NEUTRAL &&
           second != CONTROLLER_TEAM_NEUTRAL &&
           first != second;
}

static void set_entity(Entity *entity,
                       EntityId id,
                       EntityType type,
                       int active,
                       const char *controller_kind)
{
    memset(entity, 0, sizeof(*entity));
    entity->id = id;
    entity->type = type;
    entity->active = active;
    if (controller_kind)
        strncpy(entity->controller_kind, controller_kind, sizeof(entity->controller_kind) - 1);
}

static void set_controller(ControllerInstance *controller,
                           ControllerInstanceId id,
                           ControllerTeam team,
                           float x,
                           float z)
{
    memset(controller, 0, sizeof(*controller));
    controller->id = id;
    controller->team = team;
    controller->position[0] = x;
    controller->position[2] = z;
}

static void test_warfront_counts_active_war_npcs(void **state)
{
    (void)state;
    World world;
    Entity combat;
    Entity wildlife;
    Entity inactive_combat;
    Entity player;
    Entity misc;

    memset(&world, 0, sizeof(world));
    set_entity(&combat, 1u, ENTITY_TYPE_NPC, 1, "combat");
    set_entity(&wildlife, 2u, ENTITY_TYPE_NPC, 1, "wildlife");
    set_entity(&inactive_combat, 3u, ENTITY_TYPE_NPC, 0, "combat");
    set_entity(&player, 4u, ENTITY_TYPE_PLAYER, 1, "combat");
    set_entity(&misc, 5u, ENTITY_TYPE_NPC, 1, "merchant");

    world.entities[0] = &combat;
    world.entities[1] = &wildlife;
    world.entities[2] = &inactive_combat;
    world.entities[3] = &player;
    world.entities[4] = &misc;
    world.entity_count = 5;

    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_count_active_war_npcs(NULL), 0,
                              "null world must count zero war NPCs");
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_count_active_war_npcs(&world), 2,
                              "active combat and wildlife NPCs must be counted");
}

static void test_warfront_counts_team_controllers(void **state)
{
    (void)state;
    ControllerInstance banana_a;
    ControllerInstance banana_b;
    ControllerInstance bean_a;
    ControllerInstance *controllers[4];

    set_controller(&banana_a, 1u, CONTROLLER_TEAM_BANANA, 0.0f, 0.0f);
    set_controller(&banana_b, 2u, CONTROLLER_TEAM_BANANA, 1.0f, 1.0f);
    set_controller(&bean_a, 3u, CONTROLLER_TEAM_BEAN, 2.0f, 2.0f);

    controllers[0] = &banana_a;
    controllers[1] = NULL;
    controllers[2] = &banana_b;
    controllers[3] = &bean_a;

    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_count_team_controllers(NULL, 3, CONTROLLER_TEAM_BANANA), 0,
                              "null controller arrays must count zero controllers");
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_count_team_controllers(controllers, 0, CONTROLLER_TEAM_BANANA), 0,
                              "non-positive controller counts must return zero");
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_count_team_controllers(controllers, 4, CONTROLLER_TEAM_BANANA), 2,
                              "team counting must include matching controllers only");
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_count_team_controllers(controllers, 4, CONTROLLER_TEAM_BEAN), 1,
                              "team counting must support the bean side");
}

static void test_warfront_finds_best_active_warfront(void **state)
{
    (void)state;
    ControllerInstance bean_sparse;
    ControllerInstance banana_sparse;
    ControllerInstance bean_far;
    ControllerInstance banana_far;
    ControllerInstance banana_mid;
    ControllerInstance bean_mid;
    ControllerInstance bean_same_team;
    ControllerInstance banana_same_team;
    ControllerInstance *sparse_controllers[4];
    ControllerInstance *controllers[6];
    ControllerInstance *out_banana = (ControllerInstance *)0x1;
    ControllerInstance *out_bean = (ControllerInstance *)0x2;

    set_controller(&bean_sparse, 7u, CONTROLLER_TEAM_BEAN, 0.0f, 0.0f);
    set_controller(&banana_sparse, 8u, CONTROLLER_TEAM_BANANA, 1.0f, 0.0f);

    sparse_controllers[0] = NULL;
    sparse_controllers[1] = &banana_sparse;
    sparse_controllers[2] = NULL;
    sparse_controllers[3] = &bean_sparse;

    set_controller(&bean_far, 1u, CONTROLLER_TEAM_BEAN, 0.0f, 0.0f);
    set_controller(&banana_far, 2u, CONTROLLER_TEAM_BANANA, 5.0f, 0.0f);
    set_controller(&banana_mid, 3u, CONTROLLER_TEAM_BANANA, 10.0f, 0.0f);
    set_controller(&bean_mid, 4u, CONTROLLER_TEAM_BEAN, 11.0f, 0.0f);
    set_controller(&bean_same_team, 5u, CONTROLLER_TEAM_BEAN, 20.0f, 0.0f);
    set_controller(&banana_same_team, 6u, CONTROLLER_TEAM_BANANA, 30.0f, 0.0f);

    controllers[0] = &bean_far;
    controllers[1] = &banana_far;
    controllers[2] = &banana_mid;
    controllers[3] = &bean_mid;
    controllers[4] = &bean_same_team;
    controllers[5] = &banana_same_team;

    g_hostile_override = 1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_find_active_warfront(NULL, 6, 10.0f, &out_banana, &out_bean), 0,
                              "null controller arrays must not find a warfront");
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_find_active_warfront(controllers, 1, 10.0f, &out_banana, &out_bean), 0,
                              "single-controller arrays must not find a warfront");
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_find_active_warfront(controllers, 6, 0.0f, &out_banana, &out_bean), 0,
                              "non-positive radii must not find a warfront");
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_find_active_warfront(controllers, 6, 10.0f, NULL, &out_bean), 0,
                              "null banana output must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_find_active_warfront(controllers, 6, 10.0f, &out_banana, NULL), 0,
                              "null bean output must be rejected");

    out_banana = NULL;
    out_bean = NULL;
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_find_active_warfront(sparse_controllers, 4, 10.0f, &out_banana, &out_bean), 1,
                              "sparse controller arrays must still find a hostile pair");
    BANANA_TEST_ASSERT_TRUE(out_banana == &banana_sparse,
                            "sparse controller arrays must preserve banana selection");
    BANANA_TEST_ASSERT_TRUE(out_bean == &bean_sparse,
                            "sparse controller arrays must preserve bean selection");

    out_banana = NULL;
    out_bean = NULL;
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_find_active_warfront(controllers, 6, 10.0f, &out_banana, &out_bean), 1,
                              "a valid hostile pair within range must be found");
    BANANA_TEST_ASSERT_TRUE(out_banana == &banana_mid,
                            "the closest banana controller must be selected");
    BANANA_TEST_ASSERT_TRUE(out_bean == &bean_mid,
                            "the closest bean controller must be selected");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_warfront_counts_active_war_npcs),
    BANANA_TEST_CASE(test_warfront_counts_team_controllers),
    BANANA_TEST_CASE(test_warfront_finds_best_active_warfront)
)