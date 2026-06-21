#include "runtime/render/material/render_material.h"

#include "../../support/test_support.h"

#include <string.h>

static void test_render_material_for_entity_all_types(void **state)
{
    (void)state;
    Material m;

    m = runtime_render_material_for_entity(ENTITY_TYPE_NPC);
    BANANA_TEST_ASSERT_TRUE(m.r > 0.0f || m.g > 0.0f || m.b > 0.0f,
                            "NPC material must have non-zero colour components");

    m = runtime_render_material_for_entity(ENTITY_TYPE_DYNAMIC);
    BANANA_TEST_ASSERT_TRUE(m.b > 0.5f,
                            "dynamic entity material must lean toward blue");

    m = runtime_render_material_for_entity(ENTITY_TYPE_PLAYER);
    BANANA_TEST_ASSERT_TRUE(m.r > 0.8f,
                            "player material must have high red component");

    m = runtime_render_material_for_entity(ENTITY_TYPE_STATIC);
    BANANA_TEST_ASSERT_FLOAT_EQ(m.a, 1.0f, 0.001f,
                                "static entity fallback material must be fully opaque");
}

static void test_render_material_for_actor_paths(void **state)
{
    (void)state;
    Material m;

    /* null entity falls back to entity-type lookup */
    m = runtime_render_material_for_actor(NULL);
    BANANA_TEST_ASSERT_FLOAT_EQ(m.a, 1.0f, 0.001f, "null actor must still produce valid material");

    Entity npc_merchant = { .id = 1, .type = ENTITY_TYPE_NPC };
    strcpy(npc_merchant.controller_kind, "merchant");
    m = runtime_render_material_for_actor(&npc_merchant);
    BANANA_TEST_ASSERT_TRUE(m.r > 0.8f,
                            "merchant NPC material must have high red component");

    Entity npc_combat = { .id = 2, .type = ENTITY_TYPE_NPC };
    strcpy(npc_combat.controller_kind, "combat");
    m = runtime_render_material_for_actor(&npc_combat);
    BANANA_TEST_ASSERT_TRUE(m.r > 0.7f && m.g < 0.4f,
                            "combat NPC material must be predominantly red");

    Entity npc_wildlife = { .id = 3, .type = ENTITY_TYPE_NPC };
    strcpy(npc_wildlife.controller_kind, "wildlife");
    m = runtime_render_material_for_actor(&npc_wildlife);
    BANANA_TEST_ASSERT_TRUE(m.g > 0.6f,
                            "wildlife NPC material must lean toward green");

    Entity dyn_resource = { .id = 4, .type = ENTITY_TYPE_DYNAMIC };
    strcpy(dyn_resource.controller_kind, "resource");
    m = runtime_render_material_for_actor(&dyn_resource);
    BANANA_TEST_ASSERT_TRUE(m.b > 0.7f,
                            "dynamic resource material must lean toward blue");

    Entity trig_quest = { .id = 5, .type = ENTITY_TYPE_TRIGGER };
    strcpy(trig_quest.controller_kind, "quest");
    m = runtime_render_material_for_actor(&trig_quest);
    BANANA_TEST_ASSERT_TRUE(m.r > 0.6f && m.b > 0.7f,
                            "quest trigger material must be predominantly purple");

    Entity static_camp = { .id = 6, .type = ENTITY_TYPE_STATIC };
    strcpy(static_camp.controller_kind, "camp");
    m = runtime_render_material_for_actor(&static_camp);
    BANANA_TEST_ASSERT_TRUE(m.r > 0.4f && m.g > 0.3f,
                            "camp static material must be brownish");

    Entity static_pbj = { .id = 7, .type = ENTITY_TYPE_STATIC };
    strcpy(static_pbj.controller_kind, "pbj_pickup");
    m = runtime_render_material_for_actor(&static_pbj);
    BANANA_TEST_ASSERT_TRUE(m.r > 0.7f,
                            "pbj pickup material must have high red component");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_render_material_for_entity_all_types),
    BANANA_TEST_CASE(test_render_material_for_actor_paths)
)
