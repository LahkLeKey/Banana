#include "runtime/engine/exports/engine_runtime_context.h"
#include "engine.h"

#include "runtime/support/test_support.h"

#include <string.h>

EngineRuntimeState s_engine_state = {0};
const RuntimeApplicationServicePorts *s_service_ports = (const RuntimeApplicationServicePorts *)0x1;

static int g_ui_init_calls = 0;
static int g_ui_begin_calls = 0;
static int g_ui_inventory_calls = 0;
static int g_ui_merchant_calls = 0;
static int g_ui_panel_calls = 0;
static int g_ui_text_calls = 0;
static int g_ui_end_calls = 0;
static int g_ui_shutdown_calls = 0;
static int g_player_get_calls = 0;
static int g_player_add_calls = 0;
static int g_build_class_calls = 0;
static int g_build_alloc_calls = 0;
static int g_build_equip_calls = 0;
static int g_build_stat_calls = 0;
static int g_combo_calls = 0;
static int g_price_calls = 0;
static int g_trade_buy_calls = 0;
static int g_trade_sell_calls = 0;
static unsigned char g_framebuffer[4] = {1,2,3,4};

int runtime_ui_abi_init(int width, int height) { g_ui_init_calls++; return width + height; }
void runtime_ui_abi_begin_frame(void) { g_ui_begin_calls++; }
int runtime_ui_abi_inventory_panel(float x, float y) { g_ui_inventory_calls++; return (int)(x + y); }
int runtime_ui_abi_merchant_dialog(float x, float y, int npc_id) { g_ui_merchant_calls++; return npc_id + (int)(x + y); }
void runtime_ui_abi_panel(float x, float y, float width, float height, unsigned int fill_rgba, float border_width) { (void)x;(void)y;(void)width;(void)height;(void)fill_rgba;(void)border_width; g_ui_panel_calls++; }
void runtime_ui_abi_text(float x, float y, const char *text) { (void)x;(void)y;(void)text; g_ui_text_calls++; }
void runtime_ui_abi_end_frame(void) { g_ui_end_calls++; }
const unsigned char *runtime_ui_abi_get_framebuffer(void) { return g_framebuffer; }
void runtime_ui_abi_shutdown(void) { g_ui_shutdown_calls++; }

int runtime_player_merchant_adapter_get_resource(const char *resource_type, EntityId active_player_id) { (void)resource_type; (void)active_player_id; g_player_get_calls++; return 14; }
int runtime_player_merchant_adapter_add_resource(const char *resource_type, int amount, EntityId active_player_id) { (void)resource_type; (void)active_player_id; g_player_add_calls++; return amount + 7; }
int runtime_player_build_abi_set_class(const char *player_guid, int class_type) { (void)player_guid; g_build_class_calls++; return class_type; }
int runtime_player_build_abi_set_allocations(const char *player_guid, int offense_points, int defense_points, int utility_points) { (void)player_guid; g_build_alloc_calls++; return offense_points + defense_points + utility_points; }
int runtime_player_build_abi_equip(const char *player_guid, int slot, int tier, int attack_bonus, int defense_bonus, int utility_bonus) { (void)player_guid; g_build_equip_calls++; return slot + tier + attack_bonus + defense_bonus + utility_bonus; }
int runtime_player_build_abi_get_stat(const char *player_guid, const char *stat_name) { (void)player_guid; (void)stat_name; g_build_stat_calls++; return 33; }
int runtime_player_build_abi_evaluate_combo(const char *player_guid, const char *first_skill, const char *second_skill, int elapsed_ms, int party_size, int *out_damage_bonus_pct, int *out_mitigation_bonus_pct, int *out_party_synergy_bonus_pct) { (void)player_guid; (void)first_skill; (void)second_skill; (void)elapsed_ms; (void)party_size; g_combo_calls++; if (out_damage_bonus_pct) *out_damage_bonus_pct = 4; if (out_mitigation_bonus_pct) *out_mitigation_bonus_pct = 5; if (out_party_synergy_bonus_pct) *out_party_synergy_bonus_pct = 6; return 1; }
int runtime_merchant_query_abi_get_price(const RuntimeApplicationServicePorts *ports, int npc_id, const char *item_type, int *inout_seeded) { (void)ports; (void)npc_id; (void)item_type; if (inout_seeded) *inout_seeded = 1; g_price_calls++; return 99; }
int runtime_player_merchant_adapter_trade_buy(const RuntimeApplicationServicePorts *ports, int world_ready, int npc_id, const char *item_type, int quantity, int *inout_seeded, EntityId active_player_id) { (void)ports; (void)world_ready; (void)npc_id; (void)item_type; (void)active_player_id; if (inout_seeded) *inout_seeded = 1; g_trade_buy_calls++; return quantity + 1; }
int runtime_player_merchant_adapter_trade_sell(const RuntimeApplicationServicePorts *ports, int world_ready, int npc_id, const char *item_type, int quantity, int *inout_seeded, EntityId active_player_id) { (void)ports; (void)world_ready; (void)npc_id; (void)item_type; (void)active_player_id; if (inout_seeded) *inout_seeded = 1; g_trade_sell_calls++; return quantity + 2; }

Entity *world_get_entity(World *world, EntityId id)
{
    if (!world) return NULL;
    for (int i = 0; i < world->entity_count; i++)
        if (world->entities[i] && world->entities[i]->id == id)
            return world->entities[i];
    return NULL;
}

static void reset_state(void)
{
    memset(&s_engine_state, 0, sizeof(s_engine_state));
    g_ui_init_calls = g_ui_begin_calls = g_ui_inventory_calls = g_ui_merchant_calls = 0;
    g_ui_panel_calls = g_ui_text_calls = g_ui_end_calls = g_ui_shutdown_calls = 0;
    g_player_get_calls = g_player_add_calls = g_build_class_calls = g_build_alloc_calls = 0;
    g_build_equip_calls = g_build_stat_calls = g_combo_calls = g_price_calls = 0;
    g_trade_buy_calls = g_trade_sell_calls = 0;
}

static void test_ui_wrapper_calls(void **state)
{
    (void)state;
    reset_state();
    BANANA_TEST_ASSERT_INT_EQ(engine_ui_init(10, 20), 30, "ui init must forward to abi");
    engine_ui_begin_frame();
    BANANA_TEST_ASSERT_INT_EQ(engine_ui_inventory_panel(3.f, 4.f), 7, "inventory panel must forward to abi");
    BANANA_TEST_ASSERT_INT_EQ(engine_ui_merchant_dialog(1.f, 2.f, 5), 8, "merchant dialog must forward to abi");
    engine_ui_panel(0.f, 0.f, 10.f, 10.f, 0xFFu, 1.f);
    engine_ui_text(1.f, 2.f, "hello");
    engine_ui_end_frame();
    BANANA_TEST_ASSERT_TRUE(engine_ui_get_framebuffer() == g_framebuffer, "framebuffer getter must forward abi buffer");
    engine_ui_shutdown();
    BANANA_TEST_ASSERT_INT_EQ(g_ui_init_calls, 1, "ui init must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_ui_shutdown_calls, 1, "ui shutdown must be called once");
}

static void test_player_and_merchant_exports(void **state)
{
    (void)state;
    reset_state();
    World world = {0};
    Entity player = { .id = 7, .type = ENTITY_TYPE_PLAYER, .active = 1, .position = {5.f, 0.f, 9.f} };
    Entity pickup = { .id = 8, .type = ENTITY_TYPE_STATIC, .active = 1, .position = {2.f, 0.f, 3.f} };
    strcpy(pickup.controller_kind, "pbj_pickup");
    world.entities[0] = &player;
    world.entities[1] = &pickup;
    world.entity_count = 2;
    s_engine_state.world = &world;
    s_engine_state.player_id = 7;
    s_engine_state.merchants_seeded = 0;

    BANANA_TEST_ASSERT_INT_EQ(engine_player_get_resource("wood"), 14, "resource getter must forward to adapter");
    BANANA_TEST_ASSERT_INT_EQ(engine_player_add_resource("wood", 5), 12, "resource add must forward to adapter");

    float x = 0.f, z = 0.f;
    BANANA_TEST_ASSERT_INT_EQ(engine_get_player_position(&x, &z), 1, "player position getter must succeed when active player exists");
    BANANA_TEST_ASSERT_FLOAT_EQ(x, 5.f, 0.001f, "player x must match entity position");
    BANANA_TEST_ASSERT_FLOAT_EQ(z, 9.f, 0.001f, "player z must match entity position");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_pbj_pickup_position(&x, &z), 1, "pbj pickup position getter must find pickup entity");
    BANANA_TEST_ASSERT_FLOAT_EQ(engine_get_terrain_half_span(), ((float)BANANA_ENGINE_TERRAIN_SIZE - 1.0f) * 0.5f, 0.001f, "terrain half span getter must match formula");

    BANANA_TEST_ASSERT_INT_EQ(engine_player_build_set_class("p", 2), 2, "build set class must forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(engine_player_build_set_allocations("p", 1, 2, 3), 6, "allocation setter must forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(engine_player_build_equip("p", 1, 2, 3, 4, 5), 15, "equip must forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(engine_player_build_get_stat("p", "attack"), 33, "stat getter must forward to ABI");
    int damage = 0, mitigation = 0, synergy = 0;
    BANANA_TEST_ASSERT_INT_EQ(engine_player_combo_evaluate("p", "a", "b", 10, 2, &damage, &mitigation, &synergy), 1, "combo evaluate must forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(damage, 4, "combo output damage must be filled");
    BANANA_TEST_ASSERT_INT_EQ(mitigation, 5, "combo output mitigation must be filled");
    BANANA_TEST_ASSERT_INT_EQ(synergy, 6, "combo output synergy must be filled");

    BANANA_TEST_ASSERT_INT_EQ(engine_npc_merchant_get_price(1, "ore"), 99, "merchant price getter must forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(engine_npc_merchant_trade_buy(1, "ore", 3), 4, "merchant trade buy must forward to adapter");
    BANANA_TEST_ASSERT_INT_EQ(engine_npc_merchant_trade_sell(1, "ore", 3), 5, "merchant trade sell must forward to adapter");
}

static void test_position_failures(void **state)
{
    (void)state;
    reset_state();
    float x = 0.f, z = 0.f;
    BANANA_TEST_ASSERT_INT_EQ(engine_get_player_position(&x, &z), 0, "missing world must fail player position");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_pbj_pickup_position(&x, &z), 0, "missing world must fail pickup position");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_ui_wrapper_calls),
    BANANA_TEST_CASE(test_player_and_merchant_exports),
    BANANA_TEST_CASE(test_position_failures)
)
