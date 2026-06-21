#include "runtime/render/material/render_material.h"

#include "../../support/test_support.h"

#include <string.h>

static void test_entity_material_variants(void **state)
{
    (void)state;
    Material npc = runtime_render_material_for_entity(ENTITY_TYPE_NPC);
    Material dyn = runtime_render_material_for_entity(ENTITY_TYPE_DYNAMIC);
    Material player = runtime_render_material_for_entity(ENTITY_TYPE_PLAYER);
    Material other = runtime_render_material_for_entity(ENTITY_TYPE_STATIC);

    BANANA_TEST_ASSERT_TRUE(npc.color[1] > npc.color[0], "npc entity material must lean green");
    BANANA_TEST_ASSERT_TRUE(dyn.color[2] > dyn.color[0], "dynamic entity material must lean blue");
    BANANA_TEST_ASSERT_TRUE(player.color[0] > 0.9f, "player material must have strong red/yellow component");
    BANANA_TEST_ASSERT_FLOAT_EQ(other.color[3], 1.0f, 0.001f, "fallback material must be opaque");
}

static void test_actor_material_variants(void **state)
{
    (void)state;
    Entity entity = {0};
    entity.type = ENTITY_TYPE_NPC;
    strcpy(entity.controller_kind, "merchant");
    Material merchant = runtime_render_material_for_actor(&entity);
    strcpy(entity.controller_kind, "combat");
    Material combat = runtime_render_material_for_actor(&entity);
    strcpy(entity.controller_kind, "wildlife");
    Material wildlife = runtime_render_material_for_actor(&entity);

    entity.type = ENTITY_TYPE_DYNAMIC;
    strcpy(entity.controller_kind, "resource");
    Material resource = runtime_render_material_for_actor(&entity);
    entity.type = ENTITY_TYPE_TRIGGER;
    strcpy(entity.controller_kind, "quest");
    Material quest = runtime_render_material_for_actor(&entity);
    entity.type = ENTITY_TYPE_STATIC;
    strcpy(entity.controller_kind, "camp");
    Material camp = runtime_render_material_for_actor(&entity);
    strcpy(entity.controller_kind, "pbj_pickup");
    Material pbj = runtime_render_material_for_actor(&entity);
    Material fallback = runtime_render_material_for_actor(NULL);

    BANANA_TEST_ASSERT_TRUE(merchant.color[0] > 0.9f, "merchant actor material must be orange-ish");
    BANANA_TEST_ASSERT_TRUE(combat.color[0] > 0.8f && combat.color[1] < 0.3f, "combat actor material must be red-ish");
    BANANA_TEST_ASSERT_TRUE(wildlife.color[1] > 0.7f, "wildlife actor material must be green-ish");
    BANANA_TEST_ASSERT_TRUE(resource.color[2] > 0.8f, "resource actor material must be blue-ish");
    BANANA_TEST_ASSERT_TRUE(quest.color[0] > 0.7f && quest.color[2] > 0.8f, "quest actor material must be purple-ish");
    BANANA_TEST_ASSERT_TRUE(camp.color[0] > 0.5f, "camp actor material must be brown-ish");
    BANANA_TEST_ASSERT_TRUE(pbj.color[0] > 0.8f, "pbj actor material must be orange-ish");
    BANANA_TEST_ASSERT_FLOAT_EQ(fallback.color[3], 1.0f, 0.001f, "null actor fallback material must be opaque");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_entity_material_variants),
    BANANA_TEST_CASE(test_actor_material_variants)
)
