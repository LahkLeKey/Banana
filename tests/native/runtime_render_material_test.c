#include "runtime/render_material.h"

#include <math.h>
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
    Material npc = runtime_render_material_for_entity(ENTITY_TYPE_NPC);
    Material player = runtime_render_material_for_entity(ENTITY_TYPE_PLAYER);
    Material fallback = runtime_render_material_for_entity((EntityType)99);
    Entity merchant = {0};
    Entity resource = {0};
    Entity quest = {0};
    Material merchant_mat;
    Material resource_mat;
    Material quest_mat;

    merchant.type = ENTITY_TYPE_NPC;
    strncpy(merchant.controller_kind, "merchant", sizeof(merchant.controller_kind) - 1);
    resource.type = ENTITY_TYPE_DYNAMIC;
    strncpy(resource.controller_kind, "resource", sizeof(resource.controller_kind) - 1);
    quest.type = ENTITY_TYPE_TRIGGER;
    strncpy(quest.controller_kind, "quest", sizeof(quest.controller_kind) - 1);

    merchant_mat = runtime_render_material_for_actor(&merchant);
    resource_mat = runtime_render_material_for_actor(&resource);
    quest_mat = runtime_render_material_for_actor(&quest);

    if (!expect_true("npc green", fabsf(npc.color[1] - 0.80f) < 0.0001f))
        return 1;
    if (!expect_true("player red", fabsf(player.color[0] - 0.95f) < 0.0001f))
        return 1;
    if (!expect_true("fallback gray", fabsf(fallback.color[0] - 0.55f) < 0.0001f))
        return 1;
    if (!expect_true("merchant warm", fabsf(merchant_mat.color[0] - 0.95f) < 0.0001f))
        return 1;
    if (!expect_true("resource cyan", fabsf(resource_mat.color[1] - 0.65f) < 0.0001f))
        return 1;
    if (!expect_true("quest violet", fabsf(quest_mat.color[2] - 0.90f) < 0.0001f))
        return 1;

    return 0;
}
