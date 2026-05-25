#include "../../../../src/native/engine/engine.h"
#include "../../../../src/native/engine/win32_dx12_poc/scene_flow.h"

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
    int collected = 0;
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
    if (!expect_int("scene switched to world", flow_state.scene, BANANA_POC_SCENE_WORLD))
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

    engine_player_set_transform("native-default-player", pickup_x, 1.55f, pickup_z, 1);

    for (tick = 0; tick < 120; tick++)
    {
        if (!expect_int("tick after teleport", engine_tick(1.0f / 60.0f), 0))
            return 1;

        collected = engine_get_pbj_pickup_collected();
        if (collected)
            break;
    }

    if (!expect_int("objective collected", collected, 1))
        return 1;

    engine_shutdown();
    return 0;
}