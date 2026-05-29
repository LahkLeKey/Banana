#include "../scene_flow.h"
#include "demo_scene_catalog.h"

static void clear_result(BananaPocSceneFlowResult *result)
{
    if (!result)
        return;

    result->quit_requested = 0;
    result->profile_dirty = 0;
    result->entered_world = 0;
    result->character_selection_confirmed = 0;
    result->options_toggled = 0;
    result->editor_height_changed = 0;
    result->editor_apply_requested = 0;
    result->entered_scene_browser_scene = 0;
    result->scene_browser_variant = 0;
    result->scene_browser_launch_blocked = 0;
    result->scene_browser_launch_diagnostics = (int)BANANA_POC_DEMO_SCENE_VALIDATION_OK;
    result->config_lab_toggled = 0;
}

void banana_poc_scene_flow_step(BananaPocSceneFlowState *state,
                                const BananaPocSceneFlowInput *input,
                                BananaPocSceneFlowResult *result)
{
    if (!state || !input)
        return;

    clear_result(result);

    if (input->escape_pressed)
    {
        if (state->scene == BANANA_POC_SCENE_MAIN_MENU)
        {
            if (result)
                result->quit_requested = 1;
            return;
        }

        state->scene = BANANA_POC_SCENE_MAIN_MENU;
        return;
    }

    if (input->menu_hotkey_pressed &&
        (state->scene == BANANA_POC_SCENE_WORLD || state->scene == BANANA_POC_SCENE_LEVEL_EDITOR))
    {
        state->scene = BANANA_POC_SCENE_MAIN_MENU;
        return;
    }

    if (state->scene == BANANA_POC_SCENE_MAIN_MENU)
    {
        if (input->up_pressed && state->menu_index > 0)
        {
            state->menu_index--;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->down_pressed && state->menu_index < 6)
        {
            state->menu_index++;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->enter_pressed)
        {
            if (state->menu_index == 0)
            {
                state->scene = BANANA_POC_SCENE_WORLD;
                if (result)
                {
                    result->entered_world = 1;
                    result->profile_dirty = 1;
                }
            }
            else if (state->menu_index == 1)
            {
                state->scene = BANANA_POC_SCENE_CHARACTER_SELECT;
            }
            else if (state->menu_index == 2)
            {
                state->scene = BANANA_POC_SCENE_SCENE_BROWSER;
            }
            else if (state->menu_index == 3)
            {
                state->scene = BANANA_POC_SCENE_LEVEL_EDITOR;
            }
            else if (state->menu_index == 4)
            {
                state->scene = BANANA_POC_SCENE_CONFIG_LAB;
            }
            else if (state->menu_index == 5)
            {
                state->scene = BANANA_POC_SCENE_OPTIONS;
            }
            else if (result)
            {
                result->quit_requested = 1;
                result->profile_dirty = 1;
            }
        }

        return;
    }

    if (state->scene == BANANA_POC_SCENE_CHARACTER_SELECT)
    {
        if (input->tab_pressed)
        {
            state->proto_config.character_loadout_index =
                (state->proto_config.character_loadout_index + 1) % 4;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->left_pressed && state->character_index > 0)
        {
            state->character_index--;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->right_pressed && state->character_index < 3)
        {
            state->character_index++;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->up_pressed && state->class_index > 0)
        {
            state->class_index--;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->down_pressed && state->class_index < 2)
        {
            state->class_index++;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->enter_pressed)
        {
            state->scene = BANANA_POC_SCENE_WORLD;
            if (result)
            {
                result->entered_world = 1;
                result->character_selection_confirmed = 1;
                result->profile_dirty = 1;
            }
        }

        return;
    }

    if (state->scene == BANANA_POC_SCENE_OPTIONS)
    {
        if (input->up_pressed && state->options_index > 0)
        {
            state->options_index--;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->down_pressed && state->options_index < 4)
        {
            state->options_index++;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->left_pressed || input->right_pressed || input->enter_pressed)
        {
            if (result)
            {
                result->options_toggled = 1;
                result->profile_dirty = 1;
            }
        }

        return;
    }

    if (state->scene == BANANA_POC_SCENE_LEVEL_EDITOR)
    {
        if (input->up_pressed && state->proto_config.level_editor_brush_radius < 4)
        {
            state->proto_config.level_editor_brush_radius++;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->down_pressed && state->proto_config.level_editor_brush_radius > 1)
        {
            state->proto_config.level_editor_brush_radius--;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->left_pressed)
        {
            state->proto_config.level_editor_paint_mode--;
            if (state->proto_config.level_editor_paint_mode < 0)
                state->proto_config.level_editor_paint_mode = 2;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->right_pressed)
        {
            state->proto_config.level_editor_paint_mode =
                (state->proto_config.level_editor_paint_mode + 1) % 3;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->lower_edit_pressed && state->level_editor_height > 0)
        {
            state->level_editor_height--;
            if (result)
            {
                result->editor_height_changed = 1;
                result->profile_dirty = 1;
            }
        }

        if (input->raise_edit_pressed && state->level_editor_height < 3)
        {
            state->level_editor_height++;
            if (result)
            {
                result->editor_height_changed = 1;
                result->profile_dirty = 1;
            }
        }

        if (input->apply_edit_pressed && result)
            result->editor_apply_requested = 1;
        return;
    }

    if (state->scene == BANANA_POC_SCENE_SCENE_BROWSER)
    {
        int max_scene_index = banana_poc_demo_scene_catalog_count() - 1;

        if (max_scene_index < 0)
            return;

        state->proto_config.scene_browser_index =
            banana_poc_demo_scene_catalog_clamp_index(state->proto_config.scene_browser_index);

        /* Navigate catalog-defined prototype scenes. */
        if (input->up_pressed && state->proto_config.scene_browser_index > 0)
        {
            state->proto_config.scene_browser_index--;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->down_pressed && state->proto_config.scene_browser_index < max_scene_index)
        {
            state->proto_config.scene_browser_index++;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->enter_pressed)
        {
            const BananaPocDemoSceneCatalogEntry *entry =
                banana_poc_demo_scene_catalog_at_index(state->proto_config.scene_browser_index);
            BananaPocDemoSceneValidationStatus status =
                banana_poc_demo_scene_catalog_validate_index(state->proto_config.scene_browser_index);

            if (entry && status == BANANA_POC_DEMO_SCENE_VALIDATION_OK)
            {
                /* All prototype scenes reuse the WORLD runtime for now.
                   scene_browser_variant tells main.c which flavour to prep. */
                state->scene = BANANA_POC_SCENE_WORLD;
                state->proto_config.active_world_variant = entry->browser_variant;
                if (result)
                {
                    result->entered_world = 1;
                    result->entered_scene_browser_scene = 1;
                    result->scene_browser_variant = entry->browser_variant;
                    result->profile_dirty = 1;
                }
            }
            else if (result)
            {
                result->scene_browser_launch_blocked = 1;
                result->scene_browser_launch_diagnostics = (int)status;
            }
        }

        return;
    }

    if (state->scene == BANANA_POC_SCENE_CONFIG_LAB)
    {
        /* Navigate the four config rows. */
        if (input->up_pressed && state->proto_config.config_lab_index > 0)
        {
            state->proto_config.config_lab_index--;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->down_pressed && state->proto_config.config_lab_index < 3)
        {
            state->proto_config.config_lab_index++;
            if (result)
                result->profile_dirty = 1;
        }

        if (input->left_pressed || input->right_pressed || input->enter_pressed)
        {
            int idx = state->proto_config.config_lab_index;
            if (idx == 0)
                state->proto_config.render_debug_overlay ^= 1;
            else if (idx == 1)
                state->proto_config.physics_wireframe ^= 1;
            else if (idx == 2)
                state->proto_config.world_seed_index =
                    (state->proto_config.world_seed_index + 1) % 4;
            else
                state->proto_config.frame_cap_index =
                    (state->proto_config.frame_cap_index + 1) % 4;

            if (result)
            {
                result->config_lab_toggled = 1;
                result->profile_dirty = 1;
            }
        }
    }
}