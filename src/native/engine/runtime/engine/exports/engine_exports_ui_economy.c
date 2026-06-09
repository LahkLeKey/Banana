#include "../../../engine.h"

#include "engine_runtime_context.h"

#include "../../abi/ui/ui_abi.h"
#include "../../merchant/query/merchant_query_abi.h"
#include "../../player/build/abi/player_build_abi.h"
#include "../../player/merchant/player_merchant_adapter.h"

#include <string.h>

int engine_ui_init(int width, int height)
{
    return runtime_ui_abi_init(width, height);
}

void engine_ui_begin_frame(void)
{
    runtime_ui_abi_begin_frame();
}

int engine_ui_inventory_panel(float x, float y)
{
    return runtime_ui_abi_inventory_panel(x, y);
}

int engine_ui_merchant_dialog(float x, float y, int npc_id)
{
    return runtime_ui_abi_merchant_dialog(x, y, npc_id);
}

void engine_ui_panel(float x,
                     float y,
                     float width,
                     float height,
                     unsigned int fill_rgba,
                     float border_width)
{
    runtime_ui_abi_panel(x, y, width, height, fill_rgba, border_width);
}

void engine_ui_text(float x, float y, const char *text)
{
    runtime_ui_abi_text(x, y, text);
}

void engine_ui_end_frame(void)
{
    runtime_ui_abi_end_frame();
}

const unsigned char *engine_ui_get_framebuffer(void)
{
    return runtime_ui_abi_get_framebuffer();
}

void engine_ui_shutdown(void)
{
    runtime_ui_abi_shutdown();
}

int engine_player_get_resource(const char *resource_type)
{
    return runtime_player_merchant_adapter_get_resource(resource_type,
                                                        s_engine_state.player_id);
}

int engine_player_add_resource(const char *resource_type, int amount)
{
    return runtime_player_merchant_adapter_add_resource(resource_type,
                                                        amount,
                                                        s_engine_state.player_id);
}

int engine_get_player_position(float *out_x, float *out_z)
{
    Entity *player = NULL;

    if (!s_engine_state.world || !s_engine_state.player_id || !out_x || !out_z)
        return 0;

    player = world_get_entity(s_engine_state.world, s_engine_state.player_id);
    if (!player || !player->active)
        return 0;

    *out_x = player->position[0];
    *out_z = player->position[2];
    return 1;
}

int engine_get_pbj_pickup_position(float *out_x, float *out_z)
{
    int i = 0;

    if (!s_engine_state.world || !out_x || !out_z)
        return 0;

    for (i = 0; i < s_engine_state.world->entity_count; i++)
    {
        Entity *entity = s_engine_state.world->entities[i];
        if (!entity || !entity->active)
            continue;
        if (entity->type != ENTITY_TYPE_STATIC)
            continue;
        if (strcmp(entity->controller_kind, "pbj_pickup") != 0)
            continue;

        *out_x = entity->position[0];
        *out_z = entity->position[2];
        return 1;
    }

    return 0;
}

float engine_get_terrain_half_span(void)
{
    return ((float)BANANA_ENGINE_TERRAIN_SIZE - 1.0f) * 0.5f;
}

int engine_player_build_set_class(const char *player_guid, int class_type)
{
    return runtime_player_build_abi_set_class(player_guid, class_type);
}

int engine_player_build_set_allocations(const char *player_guid,
                                        int offense_points,
                                        int defense_points,
                                        int utility_points)
{
    return runtime_player_build_abi_set_allocations(player_guid,
                                                    offense_points,
                                                    defense_points,
                                                    utility_points);
}

int engine_player_build_equip(const char *player_guid,
                              int slot,
                              int tier,
                              int attack_bonus,
                              int defense_bonus,
                              int utility_bonus)
{
    return runtime_player_build_abi_equip(player_guid,
                                          slot,
                                          tier,
                                          attack_bonus,
                                          defense_bonus,
                                          utility_bonus);
}

int engine_player_build_get_stat(const char *player_guid, const char *stat_name)
{
    return runtime_player_build_abi_get_stat(player_guid, stat_name);
}

int engine_player_combo_evaluate(const char *player_guid,
                                 const char *first_skill,
                                 const char *second_skill,
                                 int elapsed_ms,
                                 int party_size,
                                 int *out_damage_bonus_pct,
                                 int *out_mitigation_bonus_pct,
                                 int *out_party_synergy_bonus_pct)
{
    return runtime_player_build_abi_evaluate_combo(player_guid,
                                                   first_skill,
                                                   second_skill,
                                                   elapsed_ms,
                                                   party_size,
                                                   out_damage_bonus_pct,
                                                   out_mitigation_bonus_pct,
                                                   out_party_synergy_bonus_pct);
}

int engine_npc_merchant_get_price(int npc_id, const char *item_type)
{
    return runtime_merchant_query_abi_get_price(s_service_ports,
                                                npc_id,
                                                item_type,
                                                &s_engine_state.merchants_seeded);
}

int engine_npc_merchant_trade_buy(int npc_id, const char *item_type, int quantity)
{
    return runtime_player_merchant_adapter_trade_buy(s_service_ports,
                                                     s_engine_state.world != NULL,
                                                     npc_id,
                                                     item_type,
                                                     quantity,
                                                     &s_engine_state.merchants_seeded,
                                                     s_engine_state.player_id);
}

int engine_npc_merchant_trade_sell(int npc_id, const char *item_type, int quantity)
{
    return runtime_player_merchant_adapter_trade_sell(s_service_ports,
                                                      s_engine_state.world != NULL,
                                                      npc_id,
                                                      item_type,
                                                      quantity,
                                                      &s_engine_state.merchants_seeded,
                                                      s_engine_state.player_id);
}
