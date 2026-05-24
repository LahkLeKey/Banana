#include "../scene_overlay.h"

#include "../../engine.h"

#include <stdio.h>
#include <string.h>

void banana_poc_render_scene_overlay(BananaPocScene scene,
                                     int main_menu_index,
                                     int character_index,
                                     int class_index,
                                     int option_index,
                                     int auto_target_enabled,
                                     int target_hotkey_enabled,
                                     int telemetry_enabled,
                                     int editor_height,
                                     int elapsed_seconds,
                                     int objective_timeout_seconds,
                                     int objective_collected,
                                     int startup_smoke_mode,
                                     const BananaPocProtoConfig *proto)
{
    char line_one[160];
    char line_two[160];
    char line_three[160];
    char line_four[160];
    char line_five[160];
    int remaining_seconds = objective_timeout_seconds - elapsed_seconds;
    float player_x = 0.0f;
    float player_z = 0.0f;
    float target_x = 0.0f;
    float target_z = 0.0f;
    float world_half_span = engine_get_terrain_half_span();
    float minimap_x = 988.0f;
    float minimap_y = 520.0f;
    float minimap_w = 280.0f;
    float minimap_h = 184.0f;
    float marker_size = 5.0f;
    int has_player = 0;
    int has_target = 0;
    float nav_dx = 0.0f;
    float nav_dz = 0.0f;
    const char *hint_x = "HOLD";
    const char *hint_z = "HOLD";
    static const char *k_main_menu_items[] = {
        "Enter World",
        "Character Select",
        "Scene Browser",
        "Level Editor",
        "Config Lab",
        "Options",
        "Quit",
    };
    static const char *k_character_names[] = {
        "DX12 Pilot",
        "Vanguard One",
        "Arcanist Flux",
        "Ranger Echo",
    };
    static const char *k_class_names[] = {
        "Vanguard",
        "Arcanist",
        "Ranger",
    };
    static const char *k_loadout_names[] = {
        "Balanced",
        "Tank",
        "Burst",
        "Support",
    };
    static const char *k_variant_names[] = {
        "Open World",
        "Physics Sandbox",
        "AI Encounter",
        "Render Benchmark",
    };

    if (remaining_seconds < 0)
    {
        remaining_seconds = 0;
    }

    if (world_half_span < 1.0f)
    {
        world_half_span = 1.0f;
    }

    engine_ui_begin_frame();

    if (scene == BANANA_POC_SCENE_MAIN_MENU ||
        scene == BANANA_POC_SCENE_CHARACTER_SELECT ||
        scene == BANANA_POC_SCENE_OPTIONS ||
        scene == BANANA_POC_SCENE_SCENE_BROWSER ||
        scene == BANANA_POC_SCENE_CONFIG_LAB)
    {
        engine_ui_panel(0.0f,
                        0.0f,
                        (float)BANANA_POC_WIDTH,
                        (float)BANANA_POC_HEIGHT,
                        0x081423FFu,
                        0.0f);
        engine_ui_panel(170.0f, 90.0f, 940.0f, 530.0f, 0x101822E8u, 2.0f);

        if (scene == BANANA_POC_SCENE_MAIN_MENU)
        {
            int i = 0;
            engine_ui_text(220.0f, 140.0f, "BANANA DX12 POC");
            engine_ui_text(220.0f, 172.0f, "Main Menu - Use Up/Down, Enter Select");
            for (i = 0; i < (int)(sizeof(k_main_menu_items) / sizeof(k_main_menu_items[0])); i++)
            {
                char item_line[128];
                snprintf(item_line,
                         sizeof(item_line),
                         "%s %s",
                         (i == main_menu_index) ? ">" : " ",
                         k_main_menu_items[i]);
                engine_ui_text(240.0f, 230.0f + ((float)i * 36.0f), item_line);
            }
            engine_ui_text(240.0f, 502.0f, "Scaffolded menus: Character, Scene Browser, Editor, Config Lab, Options");
        }
        else if (scene == BANANA_POC_SCENE_CHARACTER_SELECT)
        {
            char character_line[160];
            char class_line[160];
            char loadout_line[160];
            int loadout_index = proto ? proto->character_loadout_index : 0;
            engine_ui_text(220.0f, 140.0f, "Character Select");
            engine_ui_text(220.0f, 172.0f, "Left/Right character, Up/Down class, TAB loadout, Enter start");

            snprintf(character_line,
                     sizeof(character_line),
                     "Character: %s",
                     k_character_names[character_index]);
            snprintf(class_line,
                     sizeof(class_line),
                     "Class: %s",
                     k_class_names[class_index]);
            snprintf(loadout_line,
                     sizeof(loadout_line),
                     "Loadout Preset (TAB): %s",
                     k_loadout_names[loadout_index]);

            engine_ui_text(240.0f, 246.0f, character_line);
            engine_ui_text(240.0f, 286.0f, class_line);
            engine_ui_text(240.0f, 326.0f, loadout_line);
            engine_ui_text(240.0f, 374.0f, "Press Enter to Enter World");
        }
        else if (scene == BANANA_POC_SCENE_OPTIONS)
        {
            char option_line_a[160];
            char option_line_b[160];
            char option_line_c[160];
            char option_line_d[160];
            char option_line_e[160];
            engine_ui_text(220.0f, 140.0f, "Options");
            engine_ui_text(220.0f, 172.0f, "Up/Down select option, Left/Right toggle, Esc to menu");

            snprintf(option_line_a,
                     sizeof(option_line_a),
                     "%s Auto Startup Target: %s",
                     option_index == 0 ? ">" : " ",
                     auto_target_enabled ? "On" : "Off");
            snprintf(option_line_b,
                     sizeof(option_line_b),
                     "%s Target Hotkey (T): %s",
                     option_index == 1 ? ">" : " ",
                     target_hotkey_enabled ? "On" : "Off");
            snprintf(option_line_c,
                     sizeof(option_line_c),
                     "%s Telemetry Prints: %s",
                     option_index == 2 ? ">" : " ",
                     telemetry_enabled ? "On" : "Off");
            snprintf(option_line_d,
                     sizeof(option_line_d),
                     "%s Input Assist Hints: %s",
                     option_index == 3 ? ">" : " ",
                     (proto && proto->option_input_assist) ? "On" : "Off");
            snprintf(option_line_e,
                     sizeof(option_line_e),
                     "%s Compact HUD: %s",
                     option_index == 4 ? ">" : " ",
                     (proto && proto->option_compact_hud) ? "On" : "Off");

            engine_ui_text(240.0f, 246.0f, option_line_a);
            engine_ui_text(240.0f, 286.0f, option_line_b);
            engine_ui_text(240.0f, 326.0f, option_line_c);
            engine_ui_text(240.0f, 366.0f, option_line_d);
            engine_ui_text(240.0f, 406.0f, option_line_e);
        }
        else if (scene == BANANA_POC_SCENE_SCENE_BROWSER)
        {
            static const char *k_proto_scenes[] = {
                "Open World         (terrain + physics + player)",
                "Physics Sandbox    (flat arena, spawn test)",
                "AI Encounter       (pre-spawned AI actors)",
                "Render Benchmark   (geometry stress test)",
            };
            int i = 0;
            int browser_index = proto ? proto->scene_browser_index : 0;
            engine_ui_text(220.0f, 140.0f, "Scene Browser");
            engine_ui_text(220.0f, 172.0f, "Up/Down select, Enter launch, Esc back");
            for (i = 0; i < 4; i++)
            {
                char row[160];
                snprintf(row, sizeof(row), "%s %s",
                         (i == browser_index) ? ">" : " ",
                         k_proto_scenes[i]);
                engine_ui_text(240.0f, 230.0f + ((float)i * 40.0f), row);
            }
        }
        else if (scene == BANANA_POC_SCENE_CONFIG_LAB)
        {
            static const char *k_seed_labels[]  = { "Default", "42", "1337", "99" };
            static const char *k_fcap_labels[]  = { "30 FPS", "60 FPS", "120 FPS", "Uncapped" };
            char lab_line_a[160];
            char lab_line_b[160];
            char lab_line_c[160];
            char lab_line_d[160];
            int lab_index    = proto ? proto->config_lab_index     : 0;
            int dbg_overlay  = proto ? proto->render_debug_overlay : 0;
            int phys_wire    = proto ? proto->physics_wireframe    : 0;
            int seed_idx     = proto ? proto->world_seed_index     : 0;
            int fcap_idx     = proto ? proto->frame_cap_index      : 0;
            engine_ui_text(220.0f, 140.0f, "Config Lab");
            engine_ui_text(220.0f, 172.0f, "Up/Down select, Left/Right/Enter toggle, Esc back");
            snprintf(lab_line_a, sizeof(lab_line_a),
                     "%s Render Debug Overlay: %s",
                     lab_index == 0 ? ">" : " ", dbg_overlay ? "On" : "Off");
            snprintf(lab_line_b, sizeof(lab_line_b),
                     "%s Physics Wireframe:    %s",
                     lab_index == 1 ? ">" : " ", phys_wire ? "On" : "Off");
            snprintf(lab_line_c, sizeof(lab_line_c),
                     "%s World Seed:           %s",
                     lab_index == 2 ? ">" : " ", k_seed_labels[seed_idx]);
            snprintf(lab_line_d, sizeof(lab_line_d),
                     "%s Frame Cap:            %s",
                     lab_index == 3 ? ">" : " ", k_fcap_labels[fcap_idx]);
            engine_ui_text(240.0f, 230.0f, lab_line_a);
            engine_ui_text(240.0f, 270.0f, lab_line_b);
            engine_ui_text(240.0f, 310.0f, lab_line_c);
            engine_ui_text(240.0f, 350.0f, lab_line_d);
        }

        engine_ui_end_frame();
        return;
    }

    has_player = engine_get_player_position(&player_x, &player_z);
    has_target = engine_get_pbj_pickup_position(&target_x, &target_z);
    if (has_player && has_target)
    {
        nav_dx = target_x - player_x;
        nav_dz = target_z - player_z;
        if (nav_dx > 0.20f)
            hint_x = "D";
        else if (nav_dx < -0.20f)
            hint_x = "A";

        if (nav_dz > 0.20f)
            hint_z = "S";
        else if (nav_dz < -0.20f)
            hint_z = "W";
    }

    engine_ui_panel(12.0f, 12.0f, 760.0f, 94.0f, 0x0A1118C0u, 1.0f);
    engine_ui_panel(12.0f, 112.0f, 420.0f, 92.0f, 0x121E28C0u, 1.0f);
    engine_ui_panel(minimap_x, minimap_y, minimap_w, minimap_h, 0x1A2A22D0u, 1.0f);

    snprintf(line_one,
             sizeof(line_one),
             scene == BANANA_POC_SCENE_LEVEL_EDITOR
                 ? "BANANA DX12 POC | LEVEL EDITOR | WASD MOVE | E APPLY HEIGHT"
                 : "BANANA DX12 POC | WASD MOVE | RIGHT-CLICK MOVE TARGET | ESC QUIT");

    if (proto)
    {
        size_t used = strlen(line_one);
        if (used < sizeof(line_one) - 1)
        {
            snprintf(line_one + used,
                     sizeof(line_one) - used,
                     " | VARIANT: %s",
                     k_variant_names[proto->active_world_variant]);
        }
    }

    if (startup_smoke_mode)
    {
        snprintf(line_two,
                 sizeof(line_two),
                 "MODE: STARTUP SMOKE (CI)  |  ELAPSED: %ds",
                 elapsed_seconds);
    }
    else if (objective_collected)
    {
        snprintf(line_two,
                 sizeof(line_two),
                 "OBJECTIVE COMPLETE: PBJ PICKUP COLLECTED");
    }
    else if (objective_timeout_seconds <= 0)
    {
        snprintf(line_two,
                 sizeof(line_two),
                 "OBJECTIVE: COLLECT PBJ PICKUP  |  TIME LEFT: UNLIMITED");
    }
    else
    {
        snprintf(line_two,
                 sizeof(line_two),
                 "OBJECTIVE: COLLECT PBJ PICKUP  |  TIME LEFT: %ds",
                 remaining_seconds);
    }

    snprintf(line_three,
             sizeof(line_three),
             "ENTITIES: %d  |  CLICKS: %d  |  TARGETS REACHED: %d",
             engine_get_entity_count(),
             engine_get_click_count(),
             engine_get_target_reached_count());

    if (has_player && has_target)
    {
        snprintf(line_four,
                 sizeof(line_four),
                 "PLAYER XZ: (%.2f, %.2f)  TARGET XZ: (%.2f, %.2f)",
                 player_x,
                 player_z,
                 target_x,
                 target_z);

        if (proto && !proto->option_input_assist)
        {
            snprintf(line_five,
                     sizeof(line_five),
                     "INPUT ASSIST HINTS OFF  |  DELTA=(%.2f, %.2f)",
                     nav_dx,
                     nav_dz);
        }
        else
        {
            snprintf(line_five,
                     sizeof(line_five),
                     "NAV HINT: X -> %s  Z -> %s  DELTA=(%.2f, %.2f)",
                     hint_x,
                     hint_z,
                     nav_dx,
                     nav_dz);
        }
    }
    else
    {
        snprintf(line_four,
                 sizeof(line_four),
                 "PLAYER/TARGET POSITION UNAVAILABLE");
        snprintf(line_five,
                 sizeof(line_five),
                 "OBJECTIVE MAY ALREADY BE COLLECTED");
    }

    engine_ui_text(22.0f, 24.0f, line_one);
    engine_ui_text(22.0f, 48.0f, line_two);
    engine_ui_text(22.0f, 72.0f, line_three);
    engine_ui_text(22.0f, 124.0f, line_four);
    if (scene == BANANA_POC_SCENE_LEVEL_EDITOR)
    {
        char editor_line[160];
        static const char *k_paint_mode_names[] = { "Sculpt", "Flatten", "Smooth" };
        int brush = proto ? proto->level_editor_brush_radius : 1;
        int mode = proto ? proto->level_editor_paint_mode : 0;
        snprintf(editor_line,
                 sizeof(editor_line),
                 "EDITOR HEIGHT=%d | BRUSH=%d | MODE=%s | [ ] height | arrows brush/mode | E apply",
                 editor_height,
                 brush,
                 k_paint_mode_names[mode]);
        engine_ui_text(22.0f, 148.0f, editor_line);
    }
    else if (!(proto && proto->option_compact_hud))
    {
        engine_ui_text(22.0f, 148.0f, line_five);
    }

    if (proto && proto->render_debug_overlay)
    {
        char debug_line[160];
        snprintf(debug_line,
                 sizeof(debug_line),
                 "DEBUG OVERLAY ON | seed=%d frame-cap=%d wire=%d input-assist=%d",
                 proto->world_seed_index,
                 proto->frame_cap_index,
                 proto->physics_wireframe,
                 proto->option_input_assist);
        engine_ui_text(22.0f, 176.0f, debug_line);
    }
    engine_ui_text(minimap_x + 10.0f, minimap_y + 10.0f, "MINIMAP (TERRAIN + ACTORS)");

    engine_ui_panel(minimap_x + 10.0f,
                    minimap_y + 34.0f,
                    minimap_w - 20.0f,
                    minimap_h - 44.0f,
                    0x1E5A2FD8u,
                    1.0f);

    if (has_player)
    {
        float px = (player_x / world_half_span);
        float pz = (player_z / world_half_span);
        float panel_w = minimap_w - 20.0f;
        float panel_h = minimap_h - 44.0f;
        float screen_x = (minimap_x + 10.0f) + ((px + 1.0f) * 0.5f) * panel_w;
        float screen_y = (minimap_y + 34.0f) + ((pz + 1.0f) * 0.5f) * panel_h;

        if (screen_x < (minimap_x + 10.0f))
            screen_x = minimap_x + 10.0f;
        if (screen_x > (minimap_x + minimap_w - 10.0f - marker_size * 2.0f))
            screen_x = minimap_x + minimap_w - 10.0f - marker_size * 2.0f;
        if (screen_y < (minimap_y + 34.0f))
            screen_y = minimap_y + 34.0f;
        if (screen_y > (minimap_y + minimap_h - 10.0f - marker_size * 2.0f))
            screen_y = minimap_y + minimap_h - 10.0f - marker_size * 2.0f;

        engine_ui_panel(screen_x,
                        screen_y,
                        marker_size * 2.0f,
                        marker_size * 2.0f,
                        0x44D46CFFu,
                        1.0f);
    }

    if (has_target)
    {
        float tx = (target_x / world_half_span);
        float tz = (target_z / world_half_span);
        float panel_w = minimap_w - 20.0f;
        float panel_h = minimap_h - 44.0f;
        float screen_x = (minimap_x + 10.0f) + ((tx + 1.0f) * 0.5f) * panel_w;
        float screen_y = (minimap_y + 34.0f) + ((tz + 1.0f) * 0.5f) * panel_h;

        if (screen_x < (minimap_x + 10.0f))
            screen_x = minimap_x + 10.0f;
        if (screen_x > (minimap_x + minimap_w - 10.0f - marker_size * 2.0f))
            screen_x = minimap_x + minimap_w - 10.0f - marker_size * 2.0f;
        if (screen_y < (minimap_y + 34.0f))
            screen_y = minimap_y + 34.0f;
        if (screen_y > (minimap_y + minimap_h - 10.0f - marker_size * 2.0f))
            screen_y = minimap_y + minimap_h - 10.0f - marker_size * 2.0f;

        engine_ui_panel(screen_x,
                        screen_y,
                        marker_size * 2.0f,
                        marker_size * 2.0f,
                        0x4A8CFAFFu,
                        1.0f);
    }

    engine_ui_inventory_panel((float)BANANA_POC_WIDTH - 16.0f, 12.0f);

    engine_ui_end_frame();
}