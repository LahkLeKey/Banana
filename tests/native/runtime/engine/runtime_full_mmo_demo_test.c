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
 *   6. Intelligence-driven warfront expansion scaffolding with full biome cycle.
 *   7. Multiplayer presence: mark-seen / staleness / deactivate / rejoin.
 *   8. Coherent-world transition: catalog-driven connected pair with
 *      deterministic continuity signature, plus self-loop signature.
 *   9. War intelligence telemetry surface remains queryable for HUD/runtime.
 *
 * If any stage regresses, the "coherent game world" claim regresses with it.
 */

#include "engine.h"
#include "runtime/engine/gameplay_service.h"
#include "runtime/controller/attach/controller_attach.h"
#include "runtime/engine/engine_state.h"
#include "runtime/player/player_api.h"
#include "runtime/player/player_motion_host.h"
#include "runtime/player/player_registry.h"
#include "runtime/player/player_sync_abi.h"
#include "ai/combat_controller.h"
#include "ai/wildlife_controller.h"
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

static int count_model_token(World *world, const char *token)
{
    int count = 0;

    if (!world || !token)
        return 0;

    for (int i = 0; i < world->entity_count; i++)
    {
        const Entity *entity = world->entities[i];

        if (!entity || !entity->active || entity->type != ENTITY_TYPE_NPC)
            continue;

        if (strstr(entity->gameplay_model_id, token) != NULL)
            count += 1;
    }

    return count;
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
                                  NULL,
                                  0,
                                  NULL,
                                  host_entity,
                                  &collected,
                                  6.0f,
                                  2.0f,
                                  8.0f,
                                  0,
                                  96,
                                  0,
                                  0,
                                  0);
    expect_int("pickup collected", collected, 1);
    {
        Entity *p = world_get_entity(world, pickup);
        expect_true("pickup deactivated", p && p->active == 0);
    }

    /* ---- 6. Intelligence-driven warfront expansion scaffolding ---- */
    {
        ControllerInstance *controllers[32] = {0};
        const int max_controllers = (int)(sizeof(controllers) / sizeof(controllers[0]));
        int controller_count = 0;
        int war_pickup_collected = 1;
        int base_entities = world->entity_count;
        EngineRuntimeState war_telemetry;
        EntityId banana_front_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 2.0f, 0.0f, 2.0f);
        EntityId bean_front_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 4.0f, 0.0f, 2.0f);
        Entity *banana_front = world_get_entity(world, banana_front_id);
        Entity *bean_front = world_get_entity(world, bean_front_id);

        memset(&war_telemetry, 0, sizeof(war_telemetry));

        wildlife_controller_register();
        combat_controller_register();

        if (banana_front)
        {
            strncpy(banana_front->controller_kind, "combat", sizeof(banana_front->controller_kind) - 1);
            banana_front->controller_kind[sizeof(banana_front->controller_kind) - 1] = '\0';
        }

        if (bean_front)
        {
            strncpy(bean_front->controller_kind, "wildlife", sizeof(bean_front->controller_kind) - 1);
            bean_front->controller_kind[sizeof(bean_front->controller_kind) - 1] = '\0';
        }

        expect_true("banana front entity exists", banana_front != NULL);
        expect_true("bean front entity exists", bean_front != NULL);

        if (banana_front && bean_front)
        {
            expect_true("banana front controller attached",
                        runtime_controller_attach_to_entity_with_team(world,
                                                                      controllers,
                                                                      max_controllers,
                                                                      &controller_count,
                                                                      banana_front_id,
                                                                      "combat",
                                                                      CONTROLLER_TEAM_BANANA) != 0u);
            expect_true("bean front controller attached",
                        runtime_controller_attach_to_entity_with_team(world,
                                                                      controllers,
                                                                      max_controllers,
                                                                      &controller_count,
                                                                      bean_front_id,
                                                                      "wildlife",
                                                                      CONTROLLER_TEAM_BEAN) != 0u);

            for (int biome_stage = 0; biome_stage < 4; biome_stage++)
            {
                runtime_gameplay_service_tick(world,
                                              &war_telemetry,
                                              controllers,
                                              max_controllers,
                                              &controller_count,
                                              host_entity,
                                              &war_pickup_collected,
                                              6.0f,
                                              1.55f,
                                              8.0f,
                                              2,
                                              64,
                                              1,
                                              2,
                                              biome_stage);
            }

            expect_true("warfront expanded with full-biome reinforcements", world->entity_count >= base_entities + 8);
            expect_true("controller roster expanded for full-biome cycle", controller_count >= 10);
            expect_true("banana siege-tropical model scaffolded",
                        count_model_token(world, "banana-siege-commander-tropical") > 0);
            expect_true("banana siege-glacier model scaffolded",
                        count_model_token(world, "banana-siege-commander-glacier") > 0);
            expect_true("banana siege-urban model scaffolded",
                        count_model_token(world, "banana-siege-commander-urban") > 0);
            expect_true("banana siege-volcanic model scaffolded",
                        count_model_token(world, "banana-siege-commander-volcanic") > 0);
            expect_true("bean warbrute-tropical model scaffolded",
                        count_model_token(world, "bean-warbrute-tropical") > 0);
            expect_true("bean warbrute-glacier model scaffolded",
                        count_model_token(world, "bean-warbrute-glacier") > 0);
            expect_true("bean warbrute-urban model scaffolded",
                        count_model_token(world, "bean-warbrute-urban") > 0);
            expect_true("bean warbrute-volcanic model scaffolded",
                        count_model_token(world, "bean-warbrute-volcanic") > 0);
            expect_true("war telemetry total recorded",
                        war_telemetry.war_reinforcement_spawns_total >= 8);
            expect_true("war telemetry tropical biome recorded",
                        war_telemetry.war_reinforcement_biome_hits[0] > 0);
            expect_true("war telemetry glacier biome recorded",
                        war_telemetry.war_reinforcement_biome_hits[1] > 0);
            expect_true("war telemetry urban biome recorded",
                        war_telemetry.war_reinforcement_biome_hits[2] > 0);
            expect_true("war telemetry volcanic biome recorded",
                        war_telemetry.war_reinforcement_biome_hits[3] > 0);
        }

        for (int i = 0; i < controller_count; i++)
        {
            if (controllers[i])
                controller_destroy(controllers[i]);
        }
    }

    /* ---- 7. Multiplayer presence + staleness + rejoin ------------- */
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

    /* ---- 8. Coherent-world transition ----------------------------- */
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

    expect_true("war intelligence stage getter non-negative",
                engine_get_controller_war_intelligence_stage() >= 0);
    expect_true("war biome unlock getter positive",
                engine_get_controller_war_biome_unlock_count() > 0);
    expect_true("war reinforcement total getter non-negative",
                engine_get_controller_war_reinforcement_hits_total() >= 0);
    expect_true("war reinforcement biome getter non-negative",
                engine_get_controller_war_reinforcement_hits_biome(0) >= 0);
    expect_true("war reinforcement family getter non-negative",
                engine_get_controller_war_reinforcement_hits_family_banana_siege() >= 0);
    expect_true("war reinforcement apex getter non-negative",
                engine_get_controller_war_reinforcement_hits_family_banana_apex() >= 0);
    expect_true("war reinforcement mythic getter non-negative",
                engine_get_controller_war_reinforcement_hits_family_banana_mythic() >= 0);
    expect_true("war reinforcement bean apex getter non-negative",
                engine_get_controller_war_reinforcement_hits_family_bean_apex() >= 0);
    expect_true("war reinforcement bean mythic getter non-negative",
                engine_get_controller_war_reinforcement_hits_family_bean_mythic() >= 0);
    expect_true("war reinforcement stage apex getter non-negative",
                engine_get_controller_war_reinforcement_hits_stage_banana_apex(3) >= 0);
    expect_true("war reinforcement stage mythic getter non-negative",
                engine_get_controller_war_reinforcement_hits_stage_banana_mythic(5) >= 0);
    expect_true("war reinforcement bean stage apex getter non-negative",
                engine_get_controller_war_reinforcement_hits_stage_bean_apex(3) >= 0);
    expect_true("war reinforcement bean stage mythic getter non-negative",
                engine_get_controller_war_reinforcement_hits_stage_bean_mythic(5) >= 0);
    expect_int("war reinforcement stage getter negative index clamps to zero",
               engine_get_controller_war_reinforcement_hits_stage_banana_apex(-1),
               0);
    expect_int("war reinforcement stage getter large index clamps to zero",
               engine_get_controller_war_reinforcement_hits_stage_banana_mythic(99),
               0);
    expect_true("war life generation getter non-negative",
                engine_get_controller_war_life_generation() >= 0);
    expect_true("war life alive cells getter bounded",
                engine_get_controller_war_life_alive_cells() >= 0 &&
                    engine_get_controller_war_life_alive_cells() <= 16);
    expect_true("war life frontline cells getter bounded",
                engine_get_controller_war_life_frontline_cells() >= 0 &&
                    engine_get_controller_war_life_frontline_cells() <=
                        engine_get_controller_war_life_alive_cells());
    expect_true("war procgen biome bias getter in range",
                engine_get_controller_war_procgen_biome_bias() >= 0 &&
                    engine_get_controller_war_procgen_biome_bias() < BANANA_ENGINE_TERRAIN_MAX_LAYERS);
    expect_true("war sentience humanoid index getter non-negative",
                engine_get_controller_war_sentience_humanoid_index() >= 0);
    expect_true("war sentience coordination getter non-negative",
                engine_get_controller_war_sentience_coordination_level() >= 0);
    expect_true("war sentience empathy getter non-negative",
                engine_get_controller_war_sentience_empathy_level() >= 0);
    expect_true("war sentience banana behavior getter in range",
                engine_get_controller_war_sentience_behavior_mode_banana() >= 0 &&
                    engine_get_controller_war_sentience_behavior_mode_banana() <= 3);
    expect_true("war sentience bean behavior getter in range",
                engine_get_controller_war_sentience_behavior_mode_bean() >= 0 &&
                    engine_get_controller_war_sentience_behavior_mode_bean() <= 3);
    expect_true("war sentience banana mode-channel getter non-negative",
                engine_get_controller_war_sentience_spawn_mode_hits_banana(1) >= 0);
    expect_true("war sentience bean mode-channel getter non-negative",
                engine_get_controller_war_sentience_spawn_mode_hits_bean(3) >= 0);
    expect_int("war sentience banana mode-channel getter negative clamps to zero",
               engine_get_controller_war_sentience_spawn_mode_hits_banana(-1),
               0);
    expect_int("war sentience bean mode-channel getter large index clamps to zero",
               engine_get_controller_war_sentience_spawn_mode_hits_bean(99),
               0);
    expect_true("war sentience negotiate streak getter non-negative",
                engine_get_controller_war_sentience_negotiate_streak_ticks() >= 0);
    expect_true("war sentience negotiate trim getter bounded",
                engine_get_controller_war_sentience_negotiate_deescalation_trim_last_tick() >= 0 &&
                    engine_get_controller_war_sentience_negotiate_deescalation_trim_last_tick() <= 4);
    expect_true("war sentience comeback bonus getter non-negative",
                engine_get_controller_war_sentience_comeback_bonus_last_tick() >= 0);
    expect_true("war life tick interval policy getter bounded",
                engine_get_controller_war_life_tick_interval() >= 1 &&
                    engine_get_controller_war_life_tick_interval() <= 32);
    expect_true("war life intelligence bonus max policy getter bounded",
                engine_get_controller_war_life_intelligence_bonus_max() >= 0 &&
                    engine_get_controller_war_life_intelligence_bonus_max() <= 8);
    expect_true("war procgen biome variance policy getter bounded",
                engine_get_controller_war_procgen_biome_variance() >= 0 &&
                    engine_get_controller_war_procgen_biome_variance() <= 3);
    expect_true("war sentience gain policy getter bounded",
                engine_get_controller_war_sentience_gain_per_tick() >= 0 &&
                    engine_get_controller_war_sentience_gain_per_tick() <= 8);
    expect_true("war sentience comeback policy getter bounded",
                engine_get_controller_war_sentience_comeback_bonus_per_coordination() >= 0 &&
                    engine_get_controller_war_sentience_comeback_bonus_per_coordination() <= 8);
    expect_true("war frontier getter non-negative",
                engine_get_controller_war_frontier_chunks() >= 0);
    expect_true("war biome stage getter in range",
                engine_get_controller_war_biome_stage_index() >= 0 &&
                    engine_get_controller_war_biome_stage_index() < BANANA_ENGINE_TERRAIN_MAX_LAYERS);
    expect_true("war overcrowd pct getter in range",
                engine_get_controller_war_overcrowd_pct() >= 0 &&
                    engine_get_controller_war_overcrowd_pct() <= 100);
    expect_true("war overcrowd bonus getter non-negative",
                engine_get_controller_war_overcrowd_expand_bonus_chunks() >= 0);
    expect_true("war overcrowd intelligence bonus getter in range",
                engine_get_controller_war_overcrowd_intelligence_bonus_per_stage() >= 0 &&
                    engine_get_controller_war_overcrowd_intelligence_bonus_per_stage() <= 2);
    expect_true("war apex active getter boolean",
                engine_get_controller_war_apex_feature_active() == 0 ||
                    engine_get_controller_war_apex_feature_active() == 1);
    expect_true("war mythic active getter boolean",
                engine_get_controller_war_mythic_feature_active() == 0 ||
                    engine_get_controller_war_mythic_feature_active() == 1);

    world_destroy(world);
    engine_shutdown();

    if (g_failures == 0)
    {
        printf("MMO DEMO: PASS (all 9 stages green)\n");
        return 0;
    }
    fprintf(stderr, "MMO DEMO: FAIL (%d failed assertions)\n", g_failures);
    return 1;
}
