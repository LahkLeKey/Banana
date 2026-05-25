#ifndef BANANA_ENGINE_WIN32_DX12_POC_SCENE_FLOW_H
#define BANANA_ENGINE_WIN32_DX12_POC_SCENE_FLOW_H

#include "scene_overlay.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct BananaPocSceneFlowInput
{
    int escape_pressed;
    int menu_hotkey_pressed;
    int tab_pressed;
    int up_pressed;
    int down_pressed;
    int left_pressed;
    int right_pressed;
    int enter_pressed;
    int lower_edit_pressed;
    int raise_edit_pressed;
    int apply_edit_pressed;
} BananaPocSceneFlowInput;

typedef struct BananaPocSceneFlowState
{
    BananaPocScene scene;
    int menu_index;
    int character_index;
    int class_index;
    int options_index;
    int level_editor_height;
    BananaPocProtoConfig proto_config; /* scene browser + config lab state */
} BananaPocSceneFlowState;

typedef struct BananaPocSceneFlowResult
{
    int quit_requested;
    int profile_dirty;
    int entered_world;
    int character_selection_confirmed;
    int options_toggled;
    int editor_height_changed;
    int editor_apply_requested;
    int entered_scene_browser_scene; /* fired when user picks a proto scene     */
    int scene_browser_variant;       /* which proto scene was chosen (0-3)      */
    int scene_browser_launch_blocked;
    int scene_browser_launch_diagnostics;
    int config_lab_toggled;          /* fired when a config-lab value is changed */
} BananaPocSceneFlowResult;

void banana_poc_scene_flow_step(BananaPocSceneFlowState *state,
                                const BananaPocSceneFlowInput *input,
                                BananaPocSceneFlowResult *result);

#ifdef __cplusplus
}
#endif

#endif