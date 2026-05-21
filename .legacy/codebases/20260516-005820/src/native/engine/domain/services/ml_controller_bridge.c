#include "ml_controller_bridge.h"

#include "../../../ml/domain/histogram/histogram_matrix.h"
#include "../../../ml/domain/services/ml_controller_service.h"

#include <math.h>
#include <stdio.h>

int engine_ml_compute_move(uint32_t controller_id, float x, float y, float z, float dt,
                           float *out_move_x, float *out_move_z)
{
    char state_json[256];
    char action_json[256];
    float phase = 0.0f;
    float len = 0.0f;

    (void)y;

    if (!out_move_x || !out_move_z)
        return -1;

    snprintf(state_json, sizeof(state_json),
             "{\"entity_id\":%u,\"x\":%.3f,\"y\":%.3f,\"z\":%.3f,\"dt\":%.5f}",
             controller_id, x, y, z, dt);
    ml_controller_infer_action(state_json, action_json, sizeof(action_json));

    phase = (float)controller_id * 0.73f + x * 0.17f - z * 0.13f + dt * 4.0f;
    *out_move_x = sinf(phase);
    *out_move_z = cosf(phase * 1.17f);

    len = sqrtf((*out_move_x * *out_move_x) + (*out_move_z * *out_move_z));
    if (len > 1e-5f)
    {
        *out_move_x /= len;
        *out_move_z /= len;
    }

    ml_histogram_update("ai_action", (double)len);
    return 0;
}

void engine_ml_record_player_input(float input_x, float input_z)
{
    float magnitude = sqrtf((input_x * input_x) + (input_z * input_z));
    ml_histogram_update("player_action", (double)magnitude);
}
