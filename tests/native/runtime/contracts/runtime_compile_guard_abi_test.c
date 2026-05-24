#include "runtime/controller/controller_kind_domain.h"
#include "runtime/input/input_abi.h"
#include "runtime/input/move_target_service.h"
#include "runtime/player/player_gateway_abi.h"
#include "runtime/player/player_build_abi.h"
#include "runtime/player/player_builds.h"
#include "runtime/merchant/merchant_query_abi.h"
#include "runtime/merchant/merchant_trade_abi.h"
#include "runtime/player/player_sync_abi.h"
#include "runtime/resource/resource_domain.h"
#include "runtime/terrain/terrain_abi.h"

#include <stddef.h>
#include <stdio.h>

#define ABI_COMPILE_ASSERT(name, expr) typedef char abi_compile_assert_##name[(expr) ? 1 : -1]

typedef float (*RuntimeTerrainPortSampleHeightFn)(const EngineRuntimeState *state,
                                                  float x,
                                                  float z);
typedef int (*RuntimeTerrainPortSetHeightFn)(EngineRuntimeState *state,
                                             int x,
                                             int z,
                                             int elevation);
typedef void (*RuntimeTerrainPortMarkRegionDirtyFn)(EngineRuntimeState *state,
                                                    int min_x,
                                                    int min_z,
                                                    int max_x,
                                                    int max_z);

typedef float (*RuntimeTerrainAbiSampleFn)(const RuntimeApplicationServicePorts *ports,
                                           const EngineRuntimeState *state,
                                           float x,
                                           float z);
typedef int (*RuntimeTerrainAbiSetHeightFn)(const RuntimeApplicationServicePorts *ports,
                                            EngineRuntimeState *state,
                                            int x,
                                            int z,
                                            int elevation);
typedef void (*RuntimeTerrainAbiMarkRegionDirtyFn)(const RuntimeApplicationServicePorts *ports,
                                                   EngineRuntimeState *state,
                                                   int min_x,
                                                   int min_z,
                                                   int max_x,
                                                   int max_z);

typedef const char *(*RuntimePlayerGatewayActiveGuidFn)(EntityId active_player_id);
typedef RuntimeMerchantResourceGateway (*RuntimePlayerGatewayBuilderFn)(
    RuntimeMerchantGetResourceByKeyFn get,
    RuntimeMerchantSetResourceTotalByKeyFn set_total,
    RuntimeMerchantAddResourceByKeyFn add,
    void *context);
typedef int (*RuntimeMerchantQueryAbiGetPriceFn)(const RuntimeApplicationServicePorts *ports,
                                                 int npc_id,
                                                 const char *item_type,
                                                 int *inout_seeded);
typedef int (*RuntimeMerchantTradeAbiBuyFn)(const RuntimeApplicationServicePorts *ports,
                                            int world_ready,
                                            int npc_id,
                                            const char *item_type,
                                            int quantity,
                                            int *inout_seeded,
                                            RuntimeMerchantResourceGateway resource_gateway);
typedef int (*RuntimeMerchantTradeAbiSellFn)(const RuntimeApplicationServicePorts *ports,
                                             int world_ready,
                                             int npc_id,
                                             const char *item_type,
                                             int quantity,
                                             int *inout_seeded,
                                             RuntimeMerchantResourceGateway resource_gateway);

ABI_COMPILE_ASSERT(controller_kind_unknown_index,
                   RUNTIME_CONTROLLER_KIND_UNKNOWN == 0);
ABI_COMPILE_ASSERT(controller_kind_human_index,
                   RUNTIME_CONTROLLER_KIND_HUMAN == 1);
ABI_COMPILE_ASSERT(controller_kind_ai_index,
                   RUNTIME_CONTROLLER_KIND_AI == 2);
ABI_COMPILE_ASSERT(controller_kind_count_floor,
                   RUNTIME_CONTROLLER_KIND_COUNT >= 10);

ABI_COMPILE_ASSERT(build_class_vanguard_index,
                   BUILD_CLASS_VANGUARD == 0);
ABI_COMPILE_ASSERT(build_class_arcanist_index,
                   BUILD_CLASS_ARCANIST == 1);
ABI_COMPILE_ASSERT(build_class_ranger_index,
                   BUILD_CLASS_RANGER == 2);
ABI_COMPILE_ASSERT(build_class_count,
                   BUILD_CLASS_COUNT == 3);

ABI_COMPILE_ASSERT(gear_slot_weapon_index,
                   GEAR_SLOT_WEAPON == 0);
ABI_COMPILE_ASSERT(gear_slot_armor_index,
                   GEAR_SLOT_ARMOR == 1);
ABI_COMPILE_ASSERT(gear_slot_trinket_index,
                   GEAR_SLOT_TRINKET == 2);
ABI_COMPILE_ASSERT(gear_slot_count,
                   GEAR_SLOT_COUNT == 3);

ABI_COMPILE_ASSERT(build_stat_health_index,
                   BUILD_STAT_HEALTH == 0);
ABI_COMPILE_ASSERT(build_stat_attack_index,
                   BUILD_STAT_ATTACK == 1);
ABI_COMPILE_ASSERT(build_stat_defense_index,
                   BUILD_STAT_DEFENSE == 2);
ABI_COMPILE_ASSERT(build_stat_utility_index,
                   BUILD_STAT_UTILITY == 3);
ABI_COMPILE_ASSERT(build_stat_count,
                   BUILD_STAT_COUNT == 4);

ABI_COMPILE_ASSERT(resource_key_wood_index,
                   RUNTIME_RESOURCE_WOOD == 0);
ABI_COMPILE_ASSERT(resource_key_ore_index,
                   RUNTIME_RESOURCE_ORE == 1);
ABI_COMPILE_ASSERT(resource_key_gold_index,
                   RUNTIME_RESOURCE_GOLD == 2);
ABI_COMPILE_ASSERT(resource_key_count,
                   RUNTIME_RESOURCE_COUNT == 3);

ABI_COMPILE_ASSERT(move_target_apply_invalid_input_index,
                   RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_INPUT == 0);
ABI_COMPILE_ASSERT(move_target_apply_accepted_index,
                   RUNTIME_MOVE_TARGET_APPLY_ACCEPTED == 1);
ABI_COMPILE_ASSERT(move_target_apply_raycast_failed_index,
                   RUNTIME_MOVE_TARGET_APPLY_RAYCAST_FAILED == 2);
ABI_COMPILE_ASSERT(move_target_apply_invalid_target_index,
                   RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_TARGET == 3);
ABI_COMPILE_ASSERT(move_target_apply_result_count,
                   RUNTIME_MOVE_TARGET_APPLY_RESULT_COUNT == 4);

ABI_COMPILE_ASSERT(input_canvas_point_layout_xy,
                   sizeof(RuntimeInputCanvasPoint) == sizeof(float) * 2);
ABI_COMPILE_ASSERT(screen_normalized_point_layout_xy,
                   sizeof(RuntimeScreenNormalizedPoint) == sizeof(float) * 2);
ABI_COMPILE_ASSERT(viewport_size_layout_wh,
                   sizeof(RuntimeViewportSize) == sizeof(int) * 2);
ABI_COMPILE_ASSERT(player_sync_window_layout,
                   sizeof(RuntimePlayerSyncWindow) == sizeof(int64_t) * 2);

ABI_COMPILE_ASSERT(terrain_port_sample_signature,
                   sizeof(((RuntimeTerrainServicePort *)0)->sample_height) == sizeof(RuntimeTerrainPortSampleHeightFn));
ABI_COMPILE_ASSERT(terrain_port_set_height_signature,
                   sizeof(((RuntimeTerrainServicePort *)0)->set_height) == sizeof(RuntimeTerrainPortSetHeightFn));
ABI_COMPILE_ASSERT(terrain_port_mark_region_dirty_signature,
                   sizeof(((RuntimeTerrainServicePort *)0)->mark_region_dirty) == sizeof(RuntimeTerrainPortMarkRegionDirtyFn));

ABI_COMPILE_ASSERT(terrain_abi_sample_signature,
                   sizeof(&runtime_terrain_abi_sample_height) == sizeof(RuntimeTerrainAbiSampleFn));
ABI_COMPILE_ASSERT(terrain_abi_set_height_signature,
                   sizeof(&runtime_terrain_abi_set_height) == sizeof(RuntimeTerrainAbiSetHeightFn));
ABI_COMPILE_ASSERT(terrain_abi_mark_region_dirty_signature,
                   sizeof(&runtime_terrain_abi_mark_region_dirty) == sizeof(RuntimeTerrainAbiMarkRegionDirtyFn));

ABI_COMPILE_ASSERT(player_gateway_resource_get_signature,
                   sizeof(((RuntimeMerchantResourceGateway *)0)->get) == sizeof(RuntimeMerchantGetResourceByKeyFn));
ABI_COMPILE_ASSERT(player_gateway_resource_set_signature,
                   sizeof(((RuntimeMerchantResourceGateway *)0)->set_total) == sizeof(RuntimeMerchantSetResourceTotalByKeyFn));
ABI_COMPILE_ASSERT(player_gateway_resource_add_signature,
                   sizeof(((RuntimeMerchantResourceGateway *)0)->add) == sizeof(RuntimeMerchantAddResourceByKeyFn));
ABI_COMPILE_ASSERT(player_gateway_layout_context_first,
                   offsetof(RuntimeMerchantResourceGateway, context) == 0);
ABI_COMPILE_ASSERT(player_gateway_layout_get_after_context,
                   offsetof(RuntimeMerchantResourceGateway, get) > offsetof(RuntimeMerchantResourceGateway, context));
ABI_COMPILE_ASSERT(player_gateway_layout_set_after_get,
                   offsetof(RuntimeMerchantResourceGateway, set_total) > offsetof(RuntimeMerchantResourceGateway, get));
ABI_COMPILE_ASSERT(player_gateway_layout_add_after_set,
                   offsetof(RuntimeMerchantResourceGateway, add) > offsetof(RuntimeMerchantResourceGateway, set_total));
ABI_COMPILE_ASSERT(player_gateway_layout_min_size,
                   sizeof(RuntimeMerchantResourceGateway) >= sizeof(void *) +
                                                            sizeof(RuntimeMerchantGetResourceByKeyFn) +
                                                            sizeof(RuntimeMerchantSetResourceTotalByKeyFn) +
                                                            sizeof(RuntimeMerchantAddResourceByKeyFn));

ABI_COMPILE_ASSERT(player_gateway_active_guid_signature,
                   sizeof(&runtime_player_gateway_active_guid) == sizeof(RuntimePlayerGatewayActiveGuidFn));
ABI_COMPILE_ASSERT(player_gateway_builder_signature,
                   sizeof(&runtime_player_gateway_resource_gateway) == sizeof(RuntimePlayerGatewayBuilderFn));
ABI_COMPILE_ASSERT(merchant_query_abi_get_price_signature,
                   sizeof(&runtime_merchant_query_abi_get_price) == sizeof(RuntimeMerchantQueryAbiGetPriceFn));
ABI_COMPILE_ASSERT(merchant_trade_abi_buy_signature,
                   sizeof(&runtime_merchant_trade_abi_buy) == sizeof(RuntimeMerchantTradeAbiBuyFn));
ABI_COMPILE_ASSERT(merchant_trade_abi_sell_signature,
                   sizeof(&runtime_merchant_trade_abi_sell) == sizeof(RuntimeMerchantTradeAbiSellFn));

int main(void)
{
    printf("runtime compile-time ABI guards validated\n");
    return 0;
}
