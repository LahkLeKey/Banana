#ifndef BANANA_ENGINE_RUNTIME_MOVE_TARGET_SERVICE_H
#define BANANA_ENGINE_RUNTIME_MOVE_TARGET_SERVICE_H

#include "move_target_domain.h"
#include "player/player_motion.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BANANA_COMPILE_ASSERT(name, expr) typedef char banana_compile_assert_##name[(expr) ? 1 : -1]

    typedef struct RuntimeScreenNormalizedPoint
    {
        float x;
        float y;
    } RuntimeScreenNormalizedPoint;

    typedef struct RuntimeViewportSize
    {
        int width;
        int height;
    } RuntimeViewportSize;

    typedef enum RuntimeMoveTargetApplyResult
    {
        RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_INPUT = 0,
        RUNTIME_MOVE_TARGET_APPLY_ACCEPTED = 1,
        RUNTIME_MOVE_TARGET_APPLY_RAYCAST_FAILED = 2,
        RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_TARGET = 3,
        RUNTIME_MOVE_TARGET_APPLY_RESULT_COUNT = 4
    } RuntimeMoveTargetApplyResult;

    BANANA_COMPILE_ASSERT(move_target_apply_invalid_input_index,
                          RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_INPUT == 0);
    BANANA_COMPILE_ASSERT(move_target_apply_accepted_index,
                          RUNTIME_MOVE_TARGET_APPLY_ACCEPTED == 1);
    BANANA_COMPILE_ASSERT(move_target_apply_raycast_failed_index,
                          RUNTIME_MOVE_TARGET_APPLY_RAYCAST_FAILED == 2);
    BANANA_COMPILE_ASSERT(move_target_apply_invalid_target_index,
                          RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_TARGET == 3);
    BANANA_COMPILE_ASSERT(move_target_apply_result_count,
                          RUNTIME_MOVE_TARGET_APPLY_RESULT_COUNT == 4);

    RuntimeMoveTargetApplyResult runtime_move_target_apply_click(RuntimeMoveTargetState *state,
                                                                 RuntimeScreenNormalizedPoint normalized,
                                                                 const float *camera_eye,
                                                                 const float *camera_target,
                                                                 int camera_valid,
                                                                 RuntimeViewportSize viewport,
                                                                 float island_span,
                                                                 RuntimeTerrainSampleHeightFn sample_height);

    int runtime_move_target_apply_result_to_abi_status(RuntimeMoveTargetApplyResult result);

#ifdef __cplusplus
}
#endif

#endif
