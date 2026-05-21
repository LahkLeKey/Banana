#ifndef BANANA_ENGINE_RUNTIME_INPUT_CONTRACT_H
#define BANANA_ENGINE_RUNTIME_INPUT_CONTRACT_H

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_input_contract_get_click_count(void);
    int runtime_input_contract_get_target_reached_count(void);
    int runtime_input_contract_get_has_move_target(void);

    int runtime_input_contract_handle_right_click(float canvas_x, float canvas_y);
    int runtime_input_contract_handle_right_click_normalized(float screen_x, float screen_y);

    void runtime_input_contract_sanitize_move_input(float input_x,
                                                    float input_z,
                                                    float *out_move_x,
                                                    float *out_move_z);

#ifdef __cplusplus
}
#endif

#endif
