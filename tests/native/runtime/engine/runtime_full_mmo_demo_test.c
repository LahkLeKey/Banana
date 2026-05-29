/*
 * Full-MMO Demo Integration Test (spec 031)
 *
 * Owns the end-to-end "is this a real MMO?" scenario in one process,
 * exercising the actual ABIs (not registry math), via the launch-gate
 * env contract supplied by CMake set_tests_properties:
 *
 *   1. Engine boot through real launch gate + world create.
 *   2. Multi-player upsert (host + remote) via runtime_player_api.
 *   3. Click-to-move: apply_input → runtime_player_motion_tick → entity
 *      position advances on the real motion path.
 *   4. Economy: real merchant ABI (engine_player_upsert / add_resource /
 *      npc_merchant_get_price / trade_buy / trade_sell) with verified
 *      gold + wood ledger.
 *   5. Pickup pipeline via runtime_gameplay_service_tick.
 *   6. Multiplayer presence: mark-seen / staleness / deactivate / rejoin.
 *   7. Coherent-world transition: catalog-driven connected pair with
 *      deterministic continuity signature, plus self-loop signature.
 *
 * If any stage regresses, the "coherent game world" claim regresses with it.
 */

#include "engine.h"
#include "runtime/engine/gameplay_service.h"
#include "runtime/player/player_api.h"
#include "runtime/player/player_motion_host.h"
#include "runtime/player/player_registry.h"
#include "runtime/player/player_sync_abi.h"
#include "win32_dx12_poc/scene/demo_scene_catalog.h"
#include "world/world.h"

#include <stdio.h>
#include <string.h>

static int g_failures = 0;

static void expect_true(const char *label, int actual)
{
    if (!actual)
    {
        fprintf(stderr, "FAIL: %s\n", label);
        ++g_failures;
    }
}

static void expect_int(const char *label, int actual, int expected)
{
    if (actual != expected)
    {
        fprintf(stderr, "FAIL: %s expected=%d actual=%d\n", label, expected, actual);
        ++g_failures;
    }
}

static float flat_height(float x, float z)
{
    (void)x;
    (void)z;
    return 0.0f;
}

static uint32_t attach_controller_stub(uint32_t entity_id, const char *type)
{
    (void)type;
    return entity_id + 5000u;
}

int main(void)
{
    /* ---- 1. Engine boot through real launch gate ------------------ */
    expect_int("engine_init", engine_init(640, 360), 0);

    World *world = world_create();
    expect_true("world created", world != NULL);
    if (!world)
    {
        engine_shutdown();
        return 1;
    }

    runtime_player_registry_reset();

    /* ---- 2. Multi-player upsert ----------------------------------- */
    EntityId host_entity = 0;
    uint32_t host_id = runtime_player_api_upsert(world, "host-player", "Host", "human", 1,
                                                 flat_height, attach_controller_stub, &host_entity);
    expect_true("host upsert", host_id != 0);

    EntityId remote_entity = host_entity;
    uint32_t remote_id = runtime_player_api_upsert(world, "remote-player", "Remote", "human", 1,
                                                   flat_height, attach_controller_stub, &remote_entity);
    expect_true("remote upsert", remote_id != 0);
    expect_int("two players in registry", runtime_player_registry_count(), 2);

    /* Seed host at a known position so motion delta is observable. */
    runtime_player_api_set_transform(world, "host-player", 0.0f, 0.0f, 0.0f, 1, 16.0f, flat_height);

    /* ---- 3. Click-to-move via real motion tick -------------------- */
    runtime_player_api_apply_input("host-player", 1.0f, 0.0f);
    {
        NativePlayerBinding *b = runtime_player_registry_find("host-player");
        expect_true("host binding", b != NULL);
        expect_true("host move x stored", b && b->pending_move_x > 0.0f);
    }

    float camera_eye[3] = {0.f, 5.f, 5.f};
    float camera_target[3] = {0.f, 0.f, 0.f};
    runtime_player_motion_tick(world,
                               host_entity,
                               0.0f, 0.0f, /* per-tick analog input; pending_move is consumed */
                               camera_eye, camera_target, 1,
                               4.0f /* speed */,
                               0.25f /* dt */,
                               16.0f /* island_span */,
                               flat_height);
    {
        Entity *e = world_get_entity(world, host_entity);
        expect_true("host entity present", e != NULL);
        expect_true("host moved on x via motion tick", e && (e->position[0] != 0.0f || e->position[2] != 0.0f));
    }

    /* ---- 4. Real merchant ABI economy ----------------------------- */
    expect_true("engine player upsert seed",
                engine_player_upsert("native-default-player", "native-default-player", "human", 1) != 0);
    expect_int("seed gold", engine_player_add_resource("gold", 500), 500);
    expect_int("wood price", engine_npc_merchant_get_price(0, "wood"), 5);
    expect_int("buy 3 wood ok", engine_npc_merchant_trade_buy(0, "wood", 3), 0);
    expect_int("gold after buy", engine_player_get_resource("gold"), 485);
    expect_int("wood after buy", engine_player_get_resource("wood"), 3);
    expect_int("sell 1 wood ok", engine_npc_merchant_trade_sell(0, "wood", 1), 0);
    expect_int("wood after sell", engine_player_get_resource("wood"), 2);

    /* ---- 5. Pickup pipeline --------------------------------------- */
    /* Move host adjacent to pickup spawn for guaranteed collection. */
    runtime_player_api_set_transform(world, "host-player", 4.0f, 0.0f, -3.0f, 1, 16.0f, flat_height);
    EntityId pickup = world_spawn_entity(world, ENTITY_TYPE_STATIC, 4.5f, 0.0f, -3.5f);
    {
        Entity *p = world_get_entity(world, pickup);
        expect_true("pickup spawn", p != NULL);
        if (p)
        {
            strncpy(p->controller_kind, "pbj_pickup", sizeof(p->controller_kind) - 1);
            p->controller_kind[sizeof(p->controller_kind) - 1] = '\0';
        }
    }
    int collected = 0;
    runtime_gameplay_service_tick(world,
                                  NULL,
                                  0,
                                  NULL,
                                  host_entity,
                                  &collected,
                                  6.0f,
                                  2.0f,
                                  8.0f,
                                  0,
                                  96);
    expect_int("pickup collected", collected, 1);
    {
        Entity *p = world_get_entity(world, pickup);
        expect_true("pickup deactivated", p && p->active == 0);
    }

    /* ---- 6. Multiplayer presence + staleness + rejoin ------------- */
    /* current=12000, threshold=1500 → host last_seen=11500 fresh; remote=1000 stale. */
    runtime_player_sync_abi_mark_seen("host-player", 11500);
    runtime_player_sync_abi_mark_seen("remote-player", 1000);
    RuntimePlayerSyncWindow window = {.current_time_ms = 12000, .stale_threshold_ms = 1500};
    runtime_player_sync_abi_update_staleness(window);
    {
        NativePlayerBinding *h = runtime_player_registry_find("host-player");
        NativePlayerBinding *r = runtime_player_registry_find("remote-player");
        expect_true("host fresh", h && h->is_stale == 0);
        expect_true("remote stale", r && r->is_stale == 1);
    }
    runtime_player_sync_abi_deactivate_stale();
    {
        NativePlayerBinding *r = runtime_player_registry_find("remote-player");
        NativePlayerBinding *h = runtime_player_registry_find("host-player");
        expect_true("remote deactivated", r && r->active == 0);
        expect_true("host still active", h && h->active == 1);
    }

    /* Rejoin */
    runtime_player_sync_abi_mark_seen("remote-player", 12500);
    window.current_time_ms = 13000;
    runtime_player_sync_abi_update_staleness(window);
    {
        NativePlayerBinding *r = runtime_player_registry_find("remote-player");
        expect_true("remote re-fresh after rejoin", r && r->is_stale == 0);
    }

    /* ---- 7. Coherent-world transition ----------------------------- */
    int catalog_count = banana_poc_demo_scene_catalog_count();
    expect_true("catalog has entries", catalog_count > 0);

    int from_v = -1;
    int to_v = -1;
    for (int i = 0; i < catalog_count && from_v < 0; ++i)
    {
        if (!banana_poc_demo_scene_catalog_is_launchable_index(i))
            continue;
        const BananaPocDemoSceneCatalogEntry *ei = banana_poc_demo_scene_catalog_at_index(i);
        if (!ei)
            continue;
        for (int j = 0; j < catalog_count; ++j)
        {
            if (j == i || !banana_poc_demo_scene_catalog_is_launchable_index(j))
                continue;
            const BananaPocDemoSceneCatalogEntry *ej = banana_poc_demo_scene_catalog_at_index(j);
            if (!ej)
                continue;
            if (banana_poc_demo_scene_catalog_coherent_transition_connected(ei->browser_variant,
                                                                            ej->browser_variant))
            {
                from_v = ei->browser_variant;
                to_v = ej->browser_variant;
                break;
            }
        }
    }
    expect_true("found connected variant pair", from_v >= 0 && to_v >= 0);

    if (from_v >= 0 && to_v >= 0)
    {
        unsigned int sig_a = banana_poc_demo_scene_catalog_coherent_transition_signature(from_v, to_v);
        unsigned int sig_b = banana_poc_demo_scene_catalog_coherent_transition_signature(from_v, to_v);
        unsigned int sig_back = banana_poc_demo_scene_catalog_coherent_transition_signature(to_v, from_v);

        expect_true("forward signature non-zero", sig_a != 0u);
        expect_true("forward signature deterministic", sig_a == sig_b);
        expect_true("round-trip back signature non-zero", sig_back != 0u);
    }

    int self_v = -1;
    for (int i = 0; i < catalog_count; ++i)
    {
        if (!banana_poc_demo_scene_catalog_is_launchable_index(i))
            continue;
        const BananaPocDemoSceneCatalogEntry *ei = banana_poc_demo_scene_catalog_at_index(i);
        if (ei)
        {
            self_v = ei->browser_variant;
            break;
        }
    }
    expect_true("self-loop variant available", self_v >= 0);
    if (self_v >= 0)
    {
        expect_int("self-loop connected",
                   banana_poc_demo_scene_catalog_coherent_transition_connected(self_v, self_v), 1);
        unsigned int self_a = banana_poc_demo_scene_catalog_coherent_transition_signature(self_v, self_v);
        unsigned int self_b = banana_poc_demo_scene_catalog_coherent_transition_signature(self_v, self_v);
        expect_true("self-loop signature deterministic", self_a == self_b);
        expect_true("self-loop signature non-zero", self_a != 0u);
    }

    world_destroy(world);
    engine_shutdown();

    if (g_failures == 0)
    {
        printf("MMO DEMO: PASS (all 7 stages green)\n");
        return 0;
    }
    fprintf(stderr, "MMO DEMO: FAIL (%d failed assertions)\n", g_failures);
    return 1;
}
