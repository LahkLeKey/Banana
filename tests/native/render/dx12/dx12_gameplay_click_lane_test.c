#include "../../../../src/native/engine/engine.h"
#include "../../../../src/native/engine/win32_dx12_poc/scene_flow.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define setenv(name, value, overwrite) _putenv_s((name), (value))
#endif

static void apply_dx12_test_launch_gate_defaults(void)
{
    const char *mode = getenv("BANANA_LAUNCH_GATE_MODE");

    setenv("BANANA_LAUNCH_GATE_MODE", mode && mode[0] != '\0' ? mode : "development", 0);

    mode = getenv("BANANA_LAUNCH_GATE_MODE");
    if (!mode || mode[0] == '\0' || strcmp(mode, "development") != 0)
        return;

    setenv("BANANA_LAUNCH_GATE_VERIFICATION_AVAILABLE", "1", 0);
    setenv("BANANA_LAUNCH_GATE_VERIFICATION_FRESH", "1", 0);
    setenv("BANANA_LAUNCH_GATE_ACCOUNT_IN_GOOD_STANDING", "1", 0);
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    BananaPocSceneFlowState flow_state;
    BananaPocSceneFlowInput flow_input;
    BananaPocSceneFlowResult flow_result;
    static const char *k_character_names[] = {
        "DX12 Pilot",
        "Vanguard One",
        "Arcanist Flux",
        "Ranger Echo",
    };
    float pickup_x = 0.0f;
    float pickup_z = 0.0f;
    float player_x = 0.0f;
    float player_z = 0.0f;
    int baseline_reached = 0;
    int reached = 0;
    int tick = 0;

    apply_dx12_test_launch_gate_defaults();

    if (!expect_int("engine init", engine_init(BANANA_POC_WIDTH, BANANA_POC_HEIGHT), 0))
        return 1;

    flow_state.scene = BANANA_POC_SCENE_MAIN_MENU;
    flow_state.menu_index = 0;
    flow_state.character_index = 0;
    flow_state.class_index = 0;
    flow_state.options_index = 0;
    flow_state.level_editor_height = 2;

    flow_input.escape_pressed = 0;
    flow_input.menu_hotkey_pressed = 0;
    flow_input.tab_pressed = 0;
    flow_input.up_pressed = 0;
    flow_input.down_pressed = 0;
    flow_input.left_pressed = 0;
    flow_input.right_pressed = 0;
    flow_input.enter_pressed = 1;
    flow_input.lower_edit_pressed = 0;
    flow_input.raise_edit_pressed = 0;
    flow_input.apply_edit_pressed = 0;

    banana_poc_scene_flow_step(&flow_state, &flow_input, &flow_result);
    if (!expect_int("entered world requested", flow_result.entered_world, 1))
        return 1;

    if (!expect_int("player upsert", engine_player_upsert("native-default-player",
                                                           k_character_names[flow_state.character_index],
                                                           "human",
                                                           1) != 0 ? 1 : 0,
                    1))
        return 1;

    (void)engine_player_build_set_class("native-default-player", flow_state.class_index);

    if (!expect_int("initial tick", engine_tick(1.0f / 60.0f), 0))
        return 1;

    if (!expect_int("pickup exists", engine_get_pbj_pickup_position(&pickup_x, &pickup_z), 1))
        return 1;
    if (!expect_int("objective starts incomplete", engine_get_pbj_pickup_collected(), 0))
        return 1;

    baseline_reached = engine_get_target_reached_count();
    if (!expect_int("center click accepted", engine_handle_right_click_normalized(0.0f, 0.0f), 1))
        return 1;
    if (!expect_int("target active", engine_get_has_move_target(), 1))
        return 1;

    for (tick = 0; tick < 900; tick++)
    {
        if (!expect_int("click lane tick", engine_tick(1.0f / 60.0f), 0))
            return 1;

        if (engine_get_pbj_pickup_collected())
            break;

        reached = engine_get_target_reached_count();
        if (reached > baseline_reached)
            break;
    }

    reached = engine_get_target_reached_count();
    if (!expect_int("target reached incremented", reached > baseline_reached ? 1 : 0, 1))
        return 1;

    /* Continue movement using public input path until the pickup is collected. */
    for (tick = 0; tick < 2400 && !engine_get_pbj_pickup_collected(); tick++)
    {
        float dx = 0.0f;
        float dz = 0.0f;
        float len = 0.0f;

        if (!engine_get_player_position(&player_x, &player_z))
            return 1;

        dx = pickup_x - player_x;
        dz = pickup_z - player_z;
        len = sqrtf((dx * dx) + (dz * dz));

        if (len > 0.001f)
        {
            engine_set_move_input(dx / len, dz / len);
        }
        else
        {
            engine_set_move_input(0.0f, 0.0f);
        }

        if (!expect_int("objective lane tick", engine_tick(1.0f / 60.0f), 0))
            return 1;
    }

    engine_set_move_input(0.0f, 0.0f);

    if (!expect_int("objective collected", engine_get_pbj_pickup_collected(), 1))
    {
        (void)engine_get_player_position(&player_x, &player_z);
        fprintf(stderr,
                "objective diagnostics: player=(%.2f,%.2f) pickup=(%.2f,%.2f)\n",
                player_x,
                player_z,
                pickup_x,
                pickup_z);
        return 1;
    }

    engine_shutdown();
    return 0;
}