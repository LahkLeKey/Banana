#include "runtime/render_material.h"

#include <math.h>
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
    Material npc = runtime_render_material_for_entity(ENTITY_TYPE_NPC);
    Material player = runtime_render_material_for_entity(ENTITY_TYPE_PLAYER);
    Material fallback = runtime_render_material_for_entity((EntityType)99);

    if (!expect_true("npc green", fabsf(npc.color[1] - 0.80f) < 0.0001f))
        return 1;
    if (!expect_true("player red", fabsf(player.color[0] - 0.95f) < 0.0001f))
        return 1;
    if (!expect_true("fallback gray", fabsf(fallback.color[0] - 0.55f) < 0.0001f))
        return 1;

    return 0;
}
