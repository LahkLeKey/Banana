#include "../../../../src/native/engine/win32_dx12_poc/scene_flow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void fail_test(const char *label, const char *message)
{
    fprintf(stderr, "FAIL %s: %s\n", label, message);
    exit(1);
}

static void expect_int(const char *label, int actual, int expected)
{
    if (actual != expected)
        fail_test(label, "unexpected value");
}

static BananaPocSceneFlowInput input_zero(void)
{
    BananaPocSceneFlowInput input;

    input.escape_pressed = 0;
    input.menu_hotkey_pressed = 0;
    input.tab_pressed = 0;
    input.up_pressed = 0;
    input.down_pressed = 0;
    input.left_pressed = 0;
    input.right_pressed = 0;
    input.enter_pressed = 0;
    input.lower_edit_pressed = 0;
    input.raise_edit_pressed = 0;
    input.apply_edit_pressed = 0;

    return input;
}

int main(void)
{
    BananaPocSceneFlowState state;
    BananaPocSceneFlowResult result;
    BananaPocSceneFlowInput input;

    state.scene = BANANA_POC_SCENE_MAIN_MENU;
    state.menu_index = 0;
    state.character_index = 1;
    state.class_index = 2;
    state.options_index = 0;
    state.level_editor_height = 2;
    state.proto_config.scene_browser_index = 0;
    state.proto_config.config_lab_index = 0;
    state.proto_config.character_loadout_index = 0;
    state.proto_config.level_editor_brush_radius = 1;
    state.proto_config.level_editor_paint_mode = 0;
    state.proto_config.option_input_assist = 0;
    state.proto_config.option_compact_hud = 0;
    state.proto_config.active_world_variant = 0;
    state.proto_config.render_debug_overlay = 0;
    state.proto_config.physics_wireframe = 0;
    state.proto_config.world_seed_index = 0;
    state.proto_config.frame_cap_index = 0;

    input = input_zero();
    input.down_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("main menu down", state.menu_index, 1);
    expect_int("main menu profile dirty", result.profile_dirty, 1);

    input = input_zero();
    input.enter_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("main menu to character select", state.scene, BANANA_POC_SCENE_CHARACTER_SELECT);
    expect_int("main menu enter world", result.entered_world, 0);

    input = input_zero();
    input.right_pressed = 1;
    input.down_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("character select character", state.character_index, 2);
    expect_int("character select class", state.class_index, 2);
    expect_int("character select dirty", result.profile_dirty, 1);

    input = input_zero();
    input.tab_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("character select loadout", state.proto_config.character_loadout_index, 1);

    input = input_zero();
    input.enter_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("character select to world", state.scene, BANANA_POC_SCENE_WORLD);
    expect_int("character select confirmed", result.character_selection_confirmed, 1);
    expect_int("character select entered world", result.entered_world, 1);

    input = input_zero();
    input.menu_hotkey_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("world menu hotkey", state.scene, BANANA_POC_SCENE_MAIN_MENU);

    state.scene = BANANA_POC_SCENE_OPTIONS;
    state.options_index = 1;
    input = input_zero();
    input.left_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("options toggle flag", result.options_toggled, 1);
    expect_int("options dirty", result.profile_dirty, 1);

    state.options_index = 4;
    input = input_zero();
    input.up_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("options navigation expanded", state.options_index, 3);

    state.scene = BANANA_POC_SCENE_LEVEL_EDITOR;
    state.level_editor_height = 2;
    input = input_zero();
    input.up_pressed = 1;
    input.right_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("editor brush radius", state.proto_config.level_editor_brush_radius, 2);
    expect_int("editor paint mode", state.proto_config.level_editor_paint_mode, 1);

    input = input_zero();
    input.raise_edit_pressed = 1;
    input.apply_edit_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("editor height", state.level_editor_height, 3);
    expect_int("editor height dirty", result.editor_height_changed, 1);
    expect_int("editor apply", result.editor_apply_requested, 1);

    state.scene = BANANA_POC_SCENE_SCENE_BROWSER;
    state.proto_config.scene_browser_index = 2;
    input = input_zero();
    input.enter_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("scene browser to world", state.scene, BANANA_POC_SCENE_WORLD);
    expect_int("scene browser result flag", result.entered_scene_browser_scene, 1);
    expect_int("scene browser variant", result.scene_browser_variant, 2);

    input = input_zero();
    input.escape_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, &result);
    expect_int("escape back to menu", state.scene, BANANA_POC_SCENE_MAIN_MENU);

    /* Diagnostics boundary: NULL state must early-return without crashing. */
    banana_poc_scene_flow_step(NULL, &input, &result);

    /* Diagnostics boundary: NULL input must early-return without crashing. */
    banana_poc_scene_flow_step(&state, NULL, &result);

    /* Diagnostics boundary: NULL result must early-return without touching state. */
    state.scene = BANANA_POC_SCENE_MAIN_MENU;
    input = input_zero();
    input.down_pressed = 1;
    banana_poc_scene_flow_step(&state, &input, NULL);
    expect_int("null result preserves scene", state.scene, BANANA_POC_SCENE_MAIN_MENU);

    puts("scene flow ok");
    return 0;
}