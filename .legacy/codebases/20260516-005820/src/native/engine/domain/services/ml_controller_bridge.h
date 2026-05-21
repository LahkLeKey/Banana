#pragma once

#include <stdint.h>

int engine_ml_compute_move(uint32_t controller_id, float x, float y, float z, float dt,
						   float *out_move_x, float *out_move_z);
void engine_ml_record_player_input(float input_x, float input_z);
