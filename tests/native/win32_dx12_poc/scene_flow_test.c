#include "win32_dx12_poc/scene_flow.h"

#include "../runtime/support/test_support.h"

#include <string.h>

static BananaPocSceneFlowInput input(void)
{
    BananaPocSceneFlowInput in = {0};
    return in;
}

static void test_scene_flow_main_menu_paths(void **state)
{
    (void)state;
    BananaPocSceneFlowState flow = {0};
    BananaPocSceneFlowResult result = {0};
    BananaPocSceneFlowInput in = input();
    flow.scene = BANANA_POC_SCENE_MAIN_MENU;

    in.escape_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(result.quit_requested, 1,
                              "escape in main menu must request quit");

    in = input();
    in.down_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(flow.menu_index, 1,
                              "down in main menu must advance menu index");

    in = input();
    in.enter_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(flow.scene, BANANA_POC_SCENE_CHARACTER_SELECT,
                              "enter on character-select menu item must switch to character select scene");
}

static void test_scene_flow_character_select_and_options_paths(void **state)
{
    (void)state;
    BananaPocSceneFlowState flow = {0};
    BananaPocSceneFlowResult result = {0};
    BananaPocSceneFlowInput in = input();
    flow.scene = BANANA_POC_SCENE_CHARACTER_SELECT;

    in.right_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(flow.character_index, 1,
                              "right in character select must advance character index");

    in = input();
    in.down_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(flow.class_index, 1,
                              "down in character select must advance class index");

    in = input();
    in.tab_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(flow.proto_config.character_loadout_index, 1,
                              "tab in character select must cycle loadout index");

    in = input();
    in.enter_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(flow.scene, BANANA_POC_SCENE_WORLD,
                              "enter in character select must enter world");
    BANANA_TEST_ASSERT_INT_EQ(result.entered_world, 1,
                              "character select confirmation must set entered_world result");

    flow.scene = BANANA_POC_SCENE_OPTIONS;
    in = input();
    in.right_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(result.options_toggled, 1,
                              "toggle input in options must flag options_toggled");
}

static void test_scene_flow_editor_browser_and_config_paths(void **state)
{
    (void)state;
    BananaPocSceneFlowState flow = {0};
    BananaPocSceneFlowResult result = {0};
    BananaPocSceneFlowInput in = input();

    flow.scene = BANANA_POC_SCENE_LEVEL_EDITOR;
    in.raise_edit_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(flow.level_editor_height, 1,
                              "raise edit must increase level editor height");
    BANANA_TEST_ASSERT_INT_EQ(result.editor_height_changed, 1,
                              "height change must be reported in result");

    in = input();
    in.apply_edit_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(result.editor_apply_requested, 1,
                              "apply edit must set editor_apply_requested");

    flow.scene = BANANA_POC_SCENE_SCENE_BROWSER;
    flow.proto_config.scene_browser_index = 2;
    in = input();
    in.enter_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(flow.scene, BANANA_POC_SCENE_WORLD,
                              "launchable scene-browser entry must enter world");
    BANANA_TEST_ASSERT_INT_EQ(result.entered_scene_browser_scene, 1,
                              "scene-browser launch must set entered_scene_browser_scene");

    flow.scene = BANANA_POC_SCENE_CONFIG_LAB;
    in = input();
    in.right_pressed = 1;
    banana_poc_scene_flow_step(&flow, &in, &result);
    BANANA_TEST_ASSERT_INT_EQ(result.config_lab_toggled, 1,
                              "config-lab toggle must set config_lab_toggled");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_scene_flow_main_menu_paths),
    BANANA_TEST_CASE(test_scene_flow_character_select_and_options_paths),
    BANANA_TEST_CASE(test_scene_flow_editor_browser_and_config_paths)
)
