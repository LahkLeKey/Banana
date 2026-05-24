#ifndef BANANA_ENGINE_RUNTIME_MOVE_TARGET_DOMAIN_H
#define BANANA_ENGINE_RUNTIME_MOVE_TARGET_DOMAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeMoveTargetPoint
    {
        float x;
        float z;
    } RuntimeMoveTargetPoint;

    typedef struct RuntimeMoveTargetState
    {
        int has_target;
        RuntimeMoveTargetPoint target;
    } RuntimeMoveTargetState;

    typedef struct RuntimeMoveTargetSteering
    {
        int has_target;
        int reached_target;
        float move_x;
        float move_z;
    } RuntimeMoveTargetSteering;

    void runtime_move_target_reset(RuntimeMoveTargetState *state);

    int runtime_move_target_set(RuntimeMoveTargetState *state,
                                RuntimeMoveTargetPoint target);

    void runtime_move_target_clear(RuntimeMoveTargetState *state);

    int runtime_move_target_compute_steering(const RuntimeMoveTargetState *state,
                                             const float *player_position,
                                             const float *camera_eye,
                                             const float *camera_target,
                                             int camera_valid,
                                             float arrival_threshold,
                                             RuntimeMoveTargetSteering *out_steering);

#ifdef __cplusplus
}
#endif

#endif
