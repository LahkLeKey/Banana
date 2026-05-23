#include "runtime/controller_kind_domain.h"
#include "runtime/input_abi.h"
#include "runtime/move_target_service.h"
#include "runtime/player_build_abi.h"
#include "runtime/player_builds.h"
#include "runtime/player_sync_abi.h"
#include "runtime/resource_domain.h"

#include <stdio.h>

#define ABI_COMPILE_ASSERT(name, expr) typedef char abi_compile_assert_##name[(expr) ? 1 : -1]

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

int main(void)
{
    printf("runtime compile-time ABI guards validated\n");
    return 0;
}
