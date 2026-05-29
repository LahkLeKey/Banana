#if defined(_WIN32)

#include "../../engine.h"
#include "../scene_overlay.h"
#include "../scene/demo_scene_catalog.h"
#include "../scene_flow.h"
#include "../objective_policy.h"
#include "../../render/backend.h"
#include "../../render/backend_dx12.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

static const char *BANANA_DX12_WINDOW_CLASS = "BananaDx12RuntimeWindow";

static unsigned char *s_ui_bgra_buffer = NULL;
static size_t s_ui_bgra_capacity = 0;
static char s_profile_path[MAX_PATH] = {0};

static int consume_key_press(int vk, int *last_down)
{
    int down = (GetAsyncKeyState(vk) & 0x8000) ? 1 : 0;
    int pressed = down && (!(*last_down));
    *last_down = down;
    return pressed;
}

static float axis_value(int positive_key, int negative_key)
{
    float axis = 0.0f;
    if (GetAsyncKeyState(positive_key) & 0x8000)
    {
        axis += 1.0f;
    }
    if (GetAsyncKeyState(negative_key) & 0x8000)
    {
        axis -= 1.0f;
    }
    return axis;
}

static void update_keyboard_move_input(void)
{
    float move_x = axis_value('D', 'A');
    float move_z = axis_value('S', 'W');
    float length = sqrtf((move_x * move_x) + (move_z * move_z));

    if (length > 1.0f)
    {
        move_x /= length;
        move_z /= length;
    }

    engine_set_move_input(move_x, move_z);
}

static int point_in_rect(int x, int y, int left, int top, int right, int bottom)
{
    return (x >= left && x <= right && y >= top && y <= bottom) ? 1 : 0;
}

static void apply_menu_mouse_input(BananaPocScene scene,
                                   int mouse_x,
                                   int mouse_y,
                                   int click_edge,
                                   BananaPocSceneFlowState *state,
                                   BananaPocSceneFlowInput *input)
{
    int i = 0;

    if (!state || !input)
        return;

    if (scene == BANANA_POC_SCENE_MAIN_MENU)
    {
        for (i = 0; i <= 6; i++)
        {
            int item_y = 230 + (i * 36);
            if (point_in_rect(mouse_x, mouse_y, 220, item_y - 8, 1040, item_y + 22))
            {
                state->menu_index = i;
                if (click_edge)
                    input->enter_pressed = 1;
                break;
            }
        }
        return;
    }

    if (scene == BANANA_POC_SCENE_CHARACTER_SELECT)
    {
        if (!click_edge)
            return;

        if (point_in_rect(mouse_x, mouse_y, 230, 238, 1040, 268))
        {
            if (mouse_x < 640)
                input->left_pressed = 1;
            else
                input->right_pressed = 1;
            return;
        }

        if (point_in_rect(mouse_x, mouse_y, 230, 278, 1040, 308))
        {
            if (mouse_x < 640)
                input->up_pressed = 1;
            else
                input->down_pressed = 1;
            return;
        }

        if (point_in_rect(mouse_x, mouse_y, 230, 318, 1040, 348))
        {
            input->tab_pressed = 1;
            return;
        }

        if (point_in_rect(mouse_x, mouse_y, 230, 362, 1040, 392))
        {
            input->enter_pressed = 1;
            return;
        }
        return;
    }

    if (scene == BANANA_POC_SCENE_OPTIONS)
    {
        for (i = 0; i <= 4; i++)
        {
            int item_y = 246 + (i * 40);
            if (point_in_rect(mouse_x, mouse_y, 220, item_y - 10, 1060, item_y + 20))
            {
                state->options_index = i;
                if (click_edge)
                    input->enter_pressed = 1;
                break;
            }
        }
        return;
    }

    if (scene == BANANA_POC_SCENE_SCENE_BROWSER)
    {
        int scene_count = banana_poc_demo_scene_catalog_count();

        if (scene_count <= 0)
            return;

        for (i = 0; i < scene_count; i++)
        {
            int item_y = 230 + (i * 34);
            if (point_in_rect(mouse_x, mouse_y, 220, item_y - 10, 1060, item_y + 20))
            {
                state->proto_config.scene_browser_index = i;
                if (click_edge)
                    input->enter_pressed = 1;
                break;
            }
        }
        return;
    }

    if (scene == BANANA_POC_SCENE_CONFIG_LAB)
    {
        for (i = 0; i < 4; i++)
        {
            int item_y = 230 + (i * 40);
            if (point_in_rect(mouse_x, mouse_y, 220, item_y - 10, 1060, item_y + 20))
            {
                state->proto_config.config_lab_index = i;
                if (click_edge)
                    input->enter_pressed = 1;
                break;
            }
        }
    }
}

static int parse_env_int(const char *name, int fallback, int min_value, int max_value)
{
    const char *raw = getenv(name);
    long parsed = 0;

    if (!raw || raw[0] == '\0')
    {
        return fallback;
    }

    parsed = strtol(raw, NULL, 10);
    if (parsed < (long)min_value)
    {
        return min_value;
    }
    if (parsed > (long)max_value)
    {
        return max_value;
    }

    return (int)parsed;
}

static int parse_env_flag(const char *name)
{
    const char *raw = getenv(name);

    if (!raw || raw[0] == '\0')
    {
        return 0;
    }

    if (strcmp(raw, "0") == 0 ||
        _stricmp(raw, "false") == 0 ||
        _stricmp(raw, "off") == 0 ||
        _stricmp(raw, "no") == 0)
    {
        return 0;
    }

    return 1;
}

static const char *gameplay_role_label(BananaPocGameplayPlacementRole role)
{
    switch (role)
    {
    case BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_REFERENCE:
        return "reference";
    case BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK:
        return "landmark";
    case BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_TRAVERSAL:
        return "traversal";
    default:
        return "unknown";
    }
}

static void log_scene_gameplay_placements(int browser_variant)
{
    int placement_count = banana_poc_demo_scene_gameplay_placement_count_for_variant(browser_variant);
    int placement_index = 0;

    for (placement_index = 0; placement_index < placement_count; placement_index++)
    {
        const BananaPocDemoSceneGameplayPlacement *placement =
            banana_poc_demo_scene_gameplay_placement_at(browser_variant, placement_index);

        if (!placement)
            continue;

        printf("[dx12-poc] gameplay-placement variant=%d index=%d role=%s model=%s fallback=%s tag=%s pos=(%.1f,%.1f,%.1f)\n",
               browser_variant,
               placement_index,
               gameplay_role_label(placement->role),
               (placement->model_id && placement->model_id[0] != '\0') ? placement->model_id : "none",
               (placement->fallback_model_id && placement->fallback_model_id[0] != '\0') ? placement->fallback_model_id : "none",
               (placement->diagnostics_tag && placement->diagnostics_tag[0] != '\0') ? placement->diagnostics_tag : "none",
               placement->x,
               placement->y,
               placement->z);
    }
}

static void log_coherent_world_profile(int browser_variant)
{
    BananaPocCoherentWorldProfile profile;
    int consistent = 0;

    if (!banana_poc_demo_scene_catalog_build_coherent_profile(browser_variant, &profile))
    {
        printf("[dx12-poc] coherent-world variant=%d status=missing-profile\n", browser_variant);
        return;
    }

    consistent = banana_poc_demo_scene_catalog_coherent_profile_consistent(&profile);

    printf("[dx12-poc] coherent-world variant=%d scene=%s kind=%d primary=%d secondary=%d route=%d anchor=%s theme=%s placements=%d signature=%u status=%s consistent=%d\n",
           profile.browser_variant,
           profile.scene_key ? profile.scene_key : "unknown-scene",
           (int)profile.kind,
           (int)profile.primary_region_id,
           (int)profile.secondary_region_id,
           (int)profile.route_id,
           profile.anchor_county_id ? profile.anchor_county_id : "unknown-anchor",
           profile.gameplay_theme_id ? profile.gameplay_theme_id : "unknown-theme",
           profile.gameplay_placement_count,
           profile.bootstrap_signature,
           banana_poc_demo_scene_catalog_validation_message(profile.validation_status),
           consistent);
}

static void set_env_default(const char *name, const char *value)
{
    const char *raw = getenv(name);

    if (!name || !value)
    {
        return;
    }

    if (raw && raw[0] != '\0')
    {
        return;
    }

    _putenv_s(name, value);
}

static void apply_dx12_poc_launch_gate_defaults(void)
{
    const char *mode = getenv("BANANA_LAUNCH_GATE_MODE");

    set_env_default("BANANA_LAUNCH_GATE_MODE", "development");

    if (mode && mode[0] != '\0' && _stricmp(mode, "development") != 0)
    {
        return;
    }

    /* Keep DX12 POC runnable in local development when launch gate verification services are absent. */
    set_env_default("BANANA_LAUNCH_GATE_VERIFICATION_AVAILABLE", "1");
    set_env_default("BANANA_LAUNCH_GATE_VERIFICATION_FRESH", "1");
    set_env_default("BANANA_LAUNCH_GATE_ACCOUNT_IN_GOOD_STANDING", "1");
}

static void apply_scene_variant_war_policy(int browser_variant)
{
    float radius = 0.0f;
    int reinforcements_per_tick = 0;
    int population_cap = 0;
    char radius_buffer[32];
    char reinforcement_buffer[32];
    char cap_buffer[32];

    if (!banana_poc_demo_scene_catalog_war_policy_for_variant(browser_variant,
                                                              &radius,
                                                              &reinforcements_per_tick,
                                                              &population_cap))
    {
        return;
    }

    snprintf(radius_buffer, sizeof(radius_buffer), "%.2f", radius);
    snprintf(reinforcement_buffer, sizeof(reinforcement_buffer), "%d", reinforcements_per_tick);
    snprintf(cap_buffer, sizeof(cap_buffer), "%d", population_cap);

    _putenv_s("BANANA_CONTROLLER_WAR_RADIUS", radius_buffer);
    _putenv_s("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", reinforcement_buffer);
    _putenv_s("BANANA_CONTROLLER_WAR_POPULATION_CAP", cap_buffer);

    printf("[dx12-poc] variant-war-policy variant=%d radius=%.2f reinforcements=%d cap=%d\n",
           browser_variant,
           radius,
           reinforcements_per_tick,
           population_cap);
}

static void init_profile_path(void)
{
    DWORD length = GetModuleFileNameA(NULL, s_profile_path, MAX_PATH);
    char *last_slash = NULL;

    if (length == 0 || length >= MAX_PATH)
    {
        strcpy(s_profile_path, "banana_dx12_poc_profile.cfg");
        return;
    }

    last_slash = strrchr(s_profile_path, '\\');
    if (last_slash)
    {
        last_slash[1] = '\0';
        strncat(s_profile_path,
                "banana_dx12_poc_profile.cfg",
                MAX_PATH - strlen(s_profile_path) - 1);
    }
    else
    {
        strcpy(s_profile_path, "banana_dx12_poc_profile.cfg");
    }
}

static int load_int_value(const char *line, const char *key, int fallback)
{
    size_t key_len = strlen(key);

    if (strncmp(line, key, key_len) != 0 || line[key_len] != '=')
        return fallback;

    return atoi(line + key_len + 1);
}

static void load_profile(int *menu_index,
                         int *character_index,
                         int *class_index,
                         int *options_index,
                         int *auto_target_enabled,
                         int *target_hotkey_enabled,
                         int *telemetry_enabled,
                         int *level_editor_height,
                         BananaPocProtoConfig *proto)
{
    FILE *file = NULL;
    char line[128];

    if (!menu_index || !character_index || !class_index || !options_index ||
        !auto_target_enabled || !target_hotkey_enabled || !telemetry_enabled ||
        !level_editor_height || !proto || s_profile_path[0] == '\0')
        return;

    file = fopen(s_profile_path, "r");
    if (!file)
        return;

    while (fgets(line, sizeof(line), file))
    {
        char *newline = strpbrk(line, "\r\n");
        if (newline)
            *newline = '\0';

        *menu_index = load_int_value(line, "menu_index", *menu_index);
        *character_index = load_int_value(line, "character_index", *character_index);
        *class_index = load_int_value(line, "class_index", *class_index);
        *options_index = load_int_value(line, "options_index", *options_index);
        *auto_target_enabled = load_int_value(line, "auto_target_enabled", *auto_target_enabled);
        *target_hotkey_enabled = load_int_value(line, "target_hotkey_enabled", *target_hotkey_enabled);
        *telemetry_enabled = load_int_value(line, "telemetry_enabled", *telemetry_enabled);
        *level_editor_height = load_int_value(line, "level_editor_height", *level_editor_height);
        proto->scene_browser_index = load_int_value(line, "scene_browser_index", proto->scene_browser_index);
        proto->config_lab_index = load_int_value(line, "config_lab_index", proto->config_lab_index);
        proto->character_loadout_index = load_int_value(line, "character_loadout_index", proto->character_loadout_index);
        proto->level_editor_brush_radius = load_int_value(line, "level_editor_brush_radius", proto->level_editor_brush_radius);
        proto->level_editor_paint_mode = load_int_value(line, "level_editor_paint_mode", proto->level_editor_paint_mode);
        proto->option_input_assist = load_int_value(line, "option_input_assist", proto->option_input_assist);
        proto->option_compact_hud = load_int_value(line, "option_compact_hud", proto->option_compact_hud);
        proto->active_world_variant = load_int_value(line, "active_world_variant", proto->active_world_variant);
        proto->render_debug_overlay = load_int_value(line, "render_debug_overlay", proto->render_debug_overlay);
        proto->physics_wireframe = load_int_value(line, "physics_wireframe", proto->physics_wireframe);
        proto->world_seed_index = load_int_value(line, "world_seed_index", proto->world_seed_index);
        proto->frame_cap_index = load_int_value(line, "frame_cap_index", proto->frame_cap_index);
    }

    fclose(file);
}

static void save_profile(int menu_index,
                         int character_index,
                         int class_index,
                         int options_index,
                         int auto_target_enabled,
                         int target_hotkey_enabled,
                         int telemetry_enabled,
                         int level_editor_height,
                         const BananaPocProtoConfig *proto)
{
    FILE *file = NULL;

    if (s_profile_path[0] == '\0')
        return;

    file = fopen(s_profile_path, "w");
    if (!file)
        return;

    fprintf(file, "menu_index=%d\n", menu_index);
    fprintf(file, "character_index=%d\n", character_index);
    fprintf(file, "class_index=%d\n", class_index);
    fprintf(file, "options_index=%d\n", options_index);
    fprintf(file, "auto_target_enabled=%d\n", auto_target_enabled);
    fprintf(file, "target_hotkey_enabled=%d\n", target_hotkey_enabled);
    fprintf(file, "telemetry_enabled=%d\n", telemetry_enabled);
    fprintf(file, "level_editor_height=%d\n", level_editor_height);
    fprintf(file, "scene_browser_index=%d\n", proto ? proto->scene_browser_index : 0);
    fprintf(file, "config_lab_index=%d\n", proto ? proto->config_lab_index : 0);
    fprintf(file, "character_loadout_index=%d\n", proto ? proto->character_loadout_index : 0);
    fprintf(file, "level_editor_brush_radius=%d\n", proto ? proto->level_editor_brush_radius : 1);
    fprintf(file, "level_editor_paint_mode=%d\n", proto ? proto->level_editor_paint_mode : 0);
    fprintf(file, "option_input_assist=%d\n", proto ? proto->option_input_assist : 0);
    fprintf(file, "option_compact_hud=%d\n", proto ? proto->option_compact_hud : 0);
    fprintf(file, "active_world_variant=%d\n", proto ? proto->active_world_variant : 0);
    fprintf(file, "render_debug_overlay=%d\n", proto ? proto->render_debug_overlay : 0);
    fprintf(file, "physics_wireframe=%d\n", proto ? proto->physics_wireframe : 0);
    fprintf(file, "world_seed_index=%d\n", proto ? proto->world_seed_index : 0);
    fprintf(file, "frame_cap_index=%d\n", proto ? proto->frame_cap_index : 0);
    fclose(file);
}

static HWND find_game_window(void)
{
    return FindWindowA(BANANA_DX12_WINDOW_CLASS, NULL);
}

static void update_game_hud_title(HWND game_window,
                                  BananaPocScene scene,
                                  int objective_collected,
                                  int elapsed_seconds,
                                  int objective_timeout_seconds)
{
    char title[256];
    int remaining_seconds = objective_timeout_seconds - elapsed_seconds;
    if (remaining_seconds < 0)
    {
        remaining_seconds = 0;
    }

    if (scene == BANANA_POC_SCENE_MAIN_MENU)
    {
        snprintf(title,
                 sizeof(title),
                 "Banana DX12 POC | Main Menu | Enter Select");
    }
    else if (scene == BANANA_POC_SCENE_CHARACTER_SELECT)
    {
        snprintf(title,
                 sizeof(title),
                 "Banana DX12 POC | Character Select | Enter Confirm");
    }
    else if (scene == BANANA_POC_SCENE_OPTIONS)
    {
        snprintf(title,
                 sizeof(title),
                 "Banana DX12 POC | Options | Left/Right Toggle");
    }
    else if (scene == BANANA_POC_SCENE_LEVEL_EDITOR)
    {
        snprintf(title,
                 sizeof(title),
                 "Banana DX12 POC | Level Editor | E Apply Height");
    }
    else if (scene == BANANA_POC_SCENE_SCENE_BROWSER)
    {
        snprintf(title,
                 sizeof(title),
                 "Banana DX12 POC | Scene Browser | Up/Down select, Enter launch");
    }
    else if (scene == BANANA_POC_SCENE_CONFIG_LAB)
    {
        snprintf(title,
                 sizeof(title),
                 "Banana DX12 POC | Config Lab | Up/Down select, Left/Right toggle");
    }
    else if (objective_collected)
    {
        snprintf(title,
                 sizeof(title),
                 "Banana DX12 POC | Objective Complete | ESC to Exit");
    }
    else
    {
        snprintf(title,
                 sizeof(title),
                 "Banana DX12 POC | Objective: Collect PBJ Pickup | Time Left: %ds | WASD + Right-Click",
                 remaining_seconds);
    }

    SetWindowTextA(game_window, title);
}

static int ensure_ui_bgra_capacity(int width, int height)
{
    size_t required = (size_t)width * (size_t)height * 4u;
    unsigned char *resized = NULL;

    if (required == 0)
    {
        return 0;
    }

    if (s_ui_bgra_capacity >= required && s_ui_bgra_buffer)
    {
        return 1;
    }

    resized = (unsigned char *)realloc(s_ui_bgra_buffer, required);
    if (!resized)
    {
        return 0;
    }

    s_ui_bgra_buffer = resized;
    s_ui_bgra_capacity = required;
    return 1;
}

static void copy_rgba_to_premultiplied_bgra(const unsigned char *rgba,
                                             unsigned char *bgra,
                                             int width,
                                             int height)
{
    int pixel_count = width * height;
    int i = 0;
    for (i = 0; i < pixel_count; i++)
    {
        const unsigned char r = rgba[i * 4 + 0];
        const unsigned char g = rgba[i * 4 + 1];
        const unsigned char b = rgba[i * 4 + 2];
        const unsigned char a = rgba[i * 4 + 3];

        bgra[i * 4 + 0] = (unsigned char)((b * a) / 255u);
        bgra[i * 4 + 1] = (unsigned char)((g * a) / 255u);
        bgra[i * 4 + 2] = (unsigned char)((r * a) / 255u);
        bgra[i * 4 + 3] = a;
    }
}

static void blit_ui_overlay(HWND game_window,
                            const unsigned char *ui_rgba,
                            int width,
                            int height)
{
    HDC window_dc = NULL;
    HDC memory_dc = NULL;
    HBITMAP dib = NULL;
    HGDIOBJ previous_bitmap = NULL;
    BITMAPINFO bmi;
    void *dib_bits = NULL;
    BLENDFUNCTION blend;

    if (!game_window || !ui_rgba || width <= 0 || height <= 0)
    {
        return;
    }

    if (!ensure_ui_bgra_capacity(width, height))
    {
        return;
    }

    copy_rgba_to_premultiplied_bgra(ui_rgba, s_ui_bgra_buffer, width, height);

    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    window_dc = GetDC(game_window);
    if (!window_dc)
    {
        return;
    }

    memory_dc = CreateCompatibleDC(window_dc);
    if (!memory_dc)
    {
        ReleaseDC(game_window, window_dc);
        return;
    }

    dib = CreateDIBSection(memory_dc, &bmi, DIB_RGB_COLORS, &dib_bits, NULL, 0);
    if (!dib || !dib_bits)
    {
        DeleteDC(memory_dc);
        ReleaseDC(game_window, window_dc);
        return;
    }

    memcpy(dib_bits, s_ui_bgra_buffer, (size_t)width * (size_t)height * 4u);

    previous_bitmap = SelectObject(memory_dc, dib);

    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    AlphaBlend(window_dc,
               0,
               0,
               width,
               height,
               memory_dc,
               0,
               0,
               width,
               height,
               blend);

    if (previous_bitmap)
    {
        SelectObject(memory_dc, previous_bitmap);
    }

    DeleteObject(dib);
    DeleteDC(memory_dc);
    ReleaseDC(game_window, window_dc);
}

static void render_ui_overlay(HWND game_window,
                              BananaPocScene scene,
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
    int using_dx12_runtime = (banana_render_backend_active() == BANANA_RENDER_BACKEND_DX12) ? 1 : 0;

    if (!game_window && !using_dx12_runtime)
    {
        return;
    }

    banana_poc_render_scene_overlay(scene,
                                    main_menu_index,
                                    character_index,
                                    class_index,
                                    option_index,
                                    auto_target_enabled,
                                    target_hotkey_enabled,
                                    telemetry_enabled,
                                    editor_height,
                                    elapsed_seconds,
                                    objective_timeout_seconds,
                                    objective_collected,
                                    startup_smoke_mode,
                                    proto);

    if (using_dx12_runtime)
    {
        banana_dx12_runtime_set_ui_overlay(engine_ui_get_framebuffer(),
                                           BANANA_POC_WIDTH,
                                           BANANA_POC_HEIGHT);
        return;
    }

    blit_ui_overlay(game_window, engine_ui_get_framebuffer(), BANANA_POC_WIDTH, BANANA_POC_HEIGHT);
}

int main(void)
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER previous_counter;
    ULONGLONG telemetry_started_ms = 0;
    ULONGLONG hud_updated_ms = 0;
    ULONGLONG session_started_ms = 0;
    HWND game_window = NULL;
    BananaPocObjectivePolicy objective_policy;
    int startup_smoke_mode = 0;
    int startup_smoke_seconds = 0;
    int telemetry_interval_ms = 1000;
    int auto_target_enabled = 1;
    int target_hotkey_enabled = 0;
    int telemetry_enabled = 0;
    BananaPocScene scene = BANANA_POC_SCENE_MAIN_MENU;
    int menu_index = 0;
    int character_index = 0;
    int class_index = 0;
    int options_index = 0;
    int level_editor_height = 2;
    BananaPocProtoConfig proto_config;
    int world_variant = 0;
    int startup_scene_variant = -1;
    int exit_code = 0;
    int last_rbutton_down = 0;
    int last_lbutton_down = 0;
    int last_target_hotkey_down = 0;
    int last_up_down = 0;
    int last_down_down = 0;
    int last_left_down = 0;
    int last_right_down = 0;
    int last_enter_down = 0;
    int last_menu_hotkey_down = 0;
    int last_menu_lbutton_down = 0;
    int last_tab_down = 0;
    int last_apply_edit_down = 0;
    int last_raise_edit_down = 0;
    int last_lower_edit_down = 0;
    int last_escape_down = 0;
    int auto_target_injected = 0;
    ULONGLONG frame_throttle_prev_ms = 0;

    if (!QueryPerformanceFrequency(&frequency) || !QueryPerformanceCounter(&previous_counter))
    {
        fprintf(stderr, "[dx12-poc] high resolution timer unavailable\n");
        return 1;
    }

    printf("[dx12-poc] launching Banana playable prototype\n");
    printf("[dx12-poc] controls: arrows navigate menus, Enter select, WASD in world, ESC back/quit\n");
    memset(&proto_config, 0, sizeof(proto_config));
    proto_config.level_editor_brush_radius = 1;

    apply_dx12_poc_launch_gate_defaults();

    printf("[dx12-poc] backend requested=%s active=%s status=%s\n",
           banana_render_backend_name(banana_render_backend_requested()),
           banana_render_backend_name(banana_render_backend_active()),
           banana_render_backend_status());

    if (engine_init(BANANA_POC_WIDTH, BANANA_POC_HEIGHT) != 0)
    {
        fprintf(stderr, "[dx12-poc] engine_init failed\n");
        return 1;
    }

    if (engine_ui_init(BANANA_POC_WIDTH, BANANA_POC_HEIGHT) != 0)
    {
        fprintf(stderr, "[dx12-poc] engine_ui_init failed\n");
        engine_shutdown();
        return 1;
    }

    if (engine_player_upsert("native-default-player", "DX12 Pilot", "human", 1) == 0)
    {
        fprintf(stderr, "[dx12-poc] player bootstrap failed\n");
        engine_shutdown();
        return 1;
    }

    startup_smoke_mode = parse_env_flag("BANANA_DX12_POC_AUTOTEST");
    startup_smoke_seconds = parse_env_int("BANANA_DX12_POC_AUTOTEST_SECONDS", 2, 1, 30);
    startup_scene_variant = parse_env_int("BANANA_DX12_POC_SCENE_VARIANT", -1, -1, 127);
    banana_poc_objective_policy_init(&objective_policy,
                                     parse_env_int("BANANA_DX12_POC_OBJECTIVE_TIMEOUT_SECONDS", 0, 0, 3600));
    telemetry_interval_ms = parse_env_int("BANANA_DX12_POC_TELEMETRY_INTERVAL_MS", 5000, 250, 60000);
    auto_target_enabled = parse_env_int("BANANA_DX12_POC_AUTO_TARGET", 0, 0, 1);
    target_hotkey_enabled = parse_env_int("BANANA_DX12_POC_ENABLE_TARGET_HOTKEY", 0, 0, 1);
    telemetry_enabled = parse_env_int("BANANA_DX12_POC_TELEMETRY", 0, 0, 1);
    init_profile_path();
    load_profile(&menu_index,
                 &character_index,
                 &class_index,
                 &options_index,
                 &auto_target_enabled,
                 &target_hotkey_enabled,
                 &telemetry_enabled,
                 &level_editor_height,
                 &proto_config);
    if (menu_index < 0 || menu_index > 6)
        menu_index = 0;
    if (character_index < 0)
        character_index = 0;
    if (character_index > 3)
        character_index = 3;
    if (class_index < 0)
        class_index = 0;
    if (class_index > 2)
        class_index = 2;
    if (options_index < 0)
        options_index = 0;
    if (options_index > 4)
        options_index = 4;
    if (level_editor_height < 0)
        level_editor_height = 0;
    if (level_editor_height > 3)
        level_editor_height = 3;
    proto_config.scene_browser_index =
        banana_poc_demo_scene_catalog_clamp_index(proto_config.scene_browser_index);
    if (proto_config.config_lab_index < 0)
        proto_config.config_lab_index = 0;
    if (proto_config.config_lab_index > 3)
        proto_config.config_lab_index = 3;
    if (proto_config.character_loadout_index < 0)
        proto_config.character_loadout_index = 0;
    if (proto_config.character_loadout_index > 3)
        proto_config.character_loadout_index = 3;
    if (proto_config.level_editor_brush_radius < 1)
        proto_config.level_editor_brush_radius = 1;
    if (proto_config.level_editor_brush_radius > 4)
        proto_config.level_editor_brush_radius = 4;
    if (proto_config.level_editor_paint_mode < 0)
        proto_config.level_editor_paint_mode = 0;
    if (proto_config.level_editor_paint_mode > 2)
        proto_config.level_editor_paint_mode = 2;
    if (proto_config.world_seed_index < 0)
        proto_config.world_seed_index = 0;
    if (proto_config.world_seed_index > 3)
        proto_config.world_seed_index = 3;
    if (proto_config.frame_cap_index < 0)
        proto_config.frame_cap_index = 0;
    if (proto_config.frame_cap_index > 3)
        proto_config.frame_cap_index = 3;
    if (!banana_poc_demo_scene_catalog_for_browser_variant(proto_config.active_world_variant))
    {
        const BananaPocDemoSceneCatalogEntry *default_scene =
            banana_poc_demo_scene_catalog_at_index(banana_poc_demo_scene_catalog_first_enabled_index());
        proto_config.active_world_variant = default_scene ? default_scene->browser_variant : 0;
    }

    apply_scene_variant_war_policy(proto_config.active_world_variant);

    if (startup_scene_variant >= 0)
    {
        int startup_scene_index =
            banana_poc_demo_scene_catalog_index_for_browser_variant(startup_scene_variant);
        BananaPocDemoSceneValidationStatus startup_scene_status =
            banana_poc_demo_scene_catalog_validate_index(startup_scene_index);
        const BananaPocDemoSceneCatalogEntry *startup_scene_entry =
            banana_poc_demo_scene_catalog_for_browser_variant(startup_scene_variant);

        if (startup_scene_entry && startup_scene_status == BANANA_POC_DEMO_SCENE_VALIDATION_OK)
        {
            const BananaPocContinentAssetPack *startup_pack =
                banana_poc_continent_asset_pack_for_region(startup_scene_entry->primary_region_id);
            int gameplay_placement_count =
                banana_poc_demo_scene_gameplay_placement_count_for_variant(startup_scene_variant);
            scene = BANANA_POC_SCENE_WORLD;
            world_variant = startup_scene_variant;
            proto_config.active_world_variant = startup_scene_variant;
            proto_config.scene_browser_index = banana_poc_demo_scene_catalog_clamp_index(startup_scene_index);
            banana_poc_objective_policy_apply_world_variant(&objective_policy, world_variant);
                 apply_scene_variant_war_policy(world_variant);
            printf("[dx12-poc] startup scene override variant=%d key=%s kind=%d asset-pack=%s gameplay-theme=%s placements=%d\n",
                   world_variant,
                   startup_scene_entry->scene_key,
                   (int)startup_scene_entry->kind,
                   startup_scene_entry->asset_pack_id,
                   (startup_pack && startup_pack->gameplay_theme_id) ? startup_pack->gameplay_theme_id : "unknown-theme",
                   gameplay_placement_count);
                        log_coherent_world_profile(startup_scene_variant);
                        log_scene_gameplay_placements(startup_scene_variant);
        }
        else
        {
            fprintf(stderr,
                    "[dx12-poc] startup scene override rejected variant=%d reason=%s\n",
                    startup_scene_variant,
                    banana_poc_demo_scene_catalog_validation_message(startup_scene_status));
        }
    }

    if (IsDebuggerPresent() && !parse_env_flag("BANANA_DX12_POC_ALLOW_DEBUG_TELEMETRY"))
        telemetry_enabled = 0;

    telemetry_started_ms = GetTickCount64();
    hud_updated_ms = telemetry_started_ms;
    session_started_ms = telemetry_started_ms;
    for (;;)
    {
        LARGE_INTEGER now_counter;
        float dt = 0.0f;
        ULONGLONG now_ms = GetTickCount64();
        int frame_budget_ms = 0;
        if (proto_config.frame_cap_index == 0)
            frame_budget_ms = 33;
        else if (proto_config.frame_cap_index == 1)
            frame_budget_ms = 16;
        else if (proto_config.frame_cap_index == 2)
            frame_budget_ms = 8;
        if (frame_budget_ms > 0 && frame_throttle_prev_ms != 0)
        {
            ULONGLONG frame_delta = now_ms - frame_throttle_prev_ms;
            if (frame_delta < (ULONGLONG)frame_budget_ms)
            {
                Sleep((DWORD)((ULONGLONG)frame_budget_ms - frame_delta));
                now_ms = GetTickCount64();
            }
        }
        frame_throttle_prev_ms = now_ms;
        int elapsed_seconds = (int)((now_ms - session_started_ms) / 1000ULL);
        int objective_collected = 0;
        int tick_result = 0;
        BananaPocScene previous_scene = scene;
        BananaPocSceneFlowState flow_state;
        BananaPocSceneFlowInput flow_input;
        BananaPocSceneFlowResult flow_result;
        static const char *k_character_names[] = {
            "DX12 Pilot",
            "Vanguard One",
            "Arcanist Flux",
            "Ranger Echo",
        };

        flow_state.scene = scene;
        flow_state.menu_index = menu_index;
        flow_state.character_index = character_index;
        flow_state.class_index = class_index;
        flow_state.options_index = options_index;
        flow_state.level_editor_height = level_editor_height;
        flow_state.proto_config = proto_config;

        flow_input.escape_pressed = consume_key_press(VK_ESCAPE, &last_escape_down);
        flow_input.menu_hotkey_pressed = consume_key_press('M', &last_menu_hotkey_down);
        flow_input.tab_pressed = consume_key_press(VK_TAB, &last_tab_down);
        flow_input.up_pressed = consume_key_press(VK_UP, &last_up_down);
        flow_input.down_pressed = consume_key_press(VK_DOWN, &last_down_down);
        flow_input.left_pressed = consume_key_press(VK_LEFT, &last_left_down);
        flow_input.right_pressed = consume_key_press(VK_RIGHT, &last_right_down);
        flow_input.enter_pressed = consume_key_press(VK_RETURN, &last_enter_down);
        flow_input.lower_edit_pressed = consume_key_press(VK_OEM_4, &last_lower_edit_down);
        flow_input.raise_edit_pressed = consume_key_press(VK_OEM_6, &last_raise_edit_down);
        flow_input.apply_edit_pressed = consume_key_press('E', &last_apply_edit_down);

        if (game_window)
        {
            HCURSOR arrow = LoadCursor(NULL, IDC_ARROW);
            if (arrow)
                SetCursor(arrow);
        }

        if (game_window &&
            (scene == BANANA_POC_SCENE_MAIN_MENU ||
             scene == BANANA_POC_SCENE_CHARACTER_SELECT ||
             scene == BANANA_POC_SCENE_OPTIONS ||
             scene == BANANA_POC_SCENE_SCENE_BROWSER ||
             scene == BANANA_POC_SCENE_CONFIG_LAB))
        {
            int menu_lbutton_down = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
            int menu_click_edge = (menu_lbutton_down && !last_menu_lbutton_down) ? 1 : 0;
            POINT cursor_screen;
            POINT cursor_client;
            if (GetCursorPos(&cursor_screen))
            {
                cursor_client = cursor_screen;
                if (ScreenToClient(game_window, &cursor_client))
                {
                    apply_menu_mouse_input(scene,
                                           cursor_client.x,
                                           cursor_client.y,
                                           menu_click_edge,
                                           &flow_state,
                                           &flow_input);
                }
            }
            last_menu_lbutton_down = menu_lbutton_down;
        }
        else
        {
            last_menu_lbutton_down = 0;
        }

        banana_poc_scene_flow_step(&flow_state, &flow_input, &flow_result);

        scene = flow_state.scene;
        menu_index = flow_state.menu_index;
        character_index = flow_state.character_index;
        class_index = flow_state.class_index;
        options_index = flow_state.options_index;
        level_editor_height = flow_state.level_editor_height;
        proto_config = flow_state.proto_config;

        if (flow_result.entered_world)
        {
            banana_poc_objective_policy_on_entered_world(&objective_policy,
                                                         flow_result.entered_scene_browser_scene);
            if (!flow_result.entered_scene_browser_scene)
            {
                const BananaPocDemoSceneCatalogEntry *default_scene =
                    banana_poc_demo_scene_catalog_at_index(banana_poc_demo_scene_catalog_first_enabled_index());
                world_variant = default_scene ? default_scene->browser_variant : 0;
                proto_config.active_world_variant = world_variant;
                apply_scene_variant_war_policy(world_variant);
            }

            if (previous_scene == BANANA_POC_SCENE_CHARACTER_SELECT)
            {
                if (engine_player_upsert("native-default-player",
                                         k_character_names[character_index],
                                         "human",
                                         1) == 0)
                {
                    fprintf(stderr, "[dx12-poc] character apply failed\n");
                    scene = BANANA_POC_SCENE_CHARACTER_SELECT;
                }
                else
                {
                    (void)engine_player_build_set_class("native-default-player", class_index);
                }
            }
            else
            {
                if (engine_player_upsert("native-default-player",
                                         k_character_names[character_index],
                                         "human",
                                         1) != 0)
                {
                    (void)engine_player_build_set_class("native-default-player", class_index);
                }
            }
        }

        if (flow_result.options_toggled)
        {
            if (options_index == 0)
                auto_target_enabled = auto_target_enabled ? 0 : 1;
            else if (options_index == 1)
                target_hotkey_enabled = target_hotkey_enabled ? 0 : 1;
            else
                telemetry_enabled = telemetry_enabled ? 0 : 1;
            if (options_index == 3)
                proto_config.option_input_assist ^= 1;
            else if (options_index == 4)
                proto_config.option_compact_hud ^= 1;
        }

        if (flow_result.entered_scene_browser_scene)
        {
            int previous_world_variant = world_variant;
            const BananaPocDemoSceneCatalogEntry *entry =
                banana_poc_demo_scene_catalog_for_browser_variant(flow_result.scene_browser_variant);
            const BananaPocContinentAssetPack *pack =
                entry ? banana_poc_continent_asset_pack_for_region(entry->primary_region_id) : NULL;
            int gameplay_placement_count =
                banana_poc_demo_scene_gameplay_placement_count_for_variant(flow_result.scene_browser_variant);
            world_variant = flow_result.scene_browser_variant;
            proto_config.active_world_variant = world_variant;
            auto_target_injected = 0;
            banana_poc_objective_policy_apply_world_variant(&objective_policy, world_variant);
            apply_scene_variant_war_policy(world_variant);
            if (world_variant == 2 || world_variant == 3)
                telemetry_enabled = 1;
            printf("[dx12-poc] launching scene variant=%d key=%s kind=%d asset-pack=%s gameplay-theme=%s placements=%d\n",
                   world_variant,
                   (entry && entry->scene_key) ? entry->scene_key : "unknown-scene",
                   entry ? (int)entry->kind : -1,
                   (entry && entry->asset_pack_id) ? entry->asset_pack_id : "unknown-pack",
                   (pack && pack->gameplay_theme_id) ? pack->gameplay_theme_id : "unknown-theme",
                   gameplay_placement_count);
            if (banana_poc_demo_scene_catalog_coherent_transition_connected(previous_world_variant, world_variant))
            {
                unsigned int transition_signature =
                    banana_poc_demo_scene_catalog_coherent_transition_signature(previous_world_variant, world_variant);
                printf("[dx12-poc] coherent-transition from=%d to=%d connected=1 signature=%u\n",
                       previous_world_variant,
                       world_variant,
                       transition_signature);
            }
            else
            {
                printf("[dx12-poc] coherent-transition from=%d to=%d connected=0 signature=0\n",
                       previous_world_variant,
                       world_variant);
            }
                        log_coherent_world_profile(flow_result.scene_browser_variant);
                        log_scene_gameplay_placements(flow_result.scene_browser_variant);
        }

        if (flow_result.scene_browser_launch_blocked)
        {
            int browser_index = banana_poc_demo_scene_catalog_clamp_index(proto_config.scene_browser_index);
            const BananaPocDemoSceneCatalogEntry *entry =
                banana_poc_demo_scene_catalog_at_index(browser_index);
            BananaPocDemoSceneValidationStatus status =
                (BananaPocDemoSceneValidationStatus)flow_result.scene_browser_launch_diagnostics;
            fprintf(stderr,
                    "[dx12-poc] scene launch blocked index=%d key=%s reason=%s\n",
                    browser_index,
                    (entry && entry->scene_key) ? entry->scene_key : "unknown-scene",
                    banana_poc_demo_scene_catalog_validation_message(status));
        }

        if (flow_result.config_lab_toggled)
        {
            printf("[dx12-poc] config-lab: dbg=%d phys-wire=%d seed=%d fcap=%d\n",
                   proto_config.render_debug_overlay,
                   proto_config.physics_wireframe,
                   proto_config.world_seed_index,
                   proto_config.frame_cap_index);
        }

        if (flow_result.editor_apply_requested)
        {
            float px = 0.0f;
            float pz = 0.0f;
            static const char *k_paint_mode_labels[] = { "Sculpt", "Flatten", "Smooth" };
            if (engine_get_player_position(&px, &pz))
            {
                float half_span = engine_get_terrain_half_span();
                int terrain_size = (int)(half_span * 2.0f + 1.0f);
                int ix = (int)roundf(px + half_span);
                int iz = (int)roundf(pz + half_span);

                if (ix < 0)
                    ix = 0;
                if (iz < 0)
                    iz = 0;
                if (ix >= terrain_size)
                    ix = terrain_size - 1;
                if (iz >= terrain_size)
                    iz = terrain_size - 1;

                if (engine_terrain_set_height(ix, iz, level_editor_height) != 0)
                {
                    fprintf(stderr,
                            "[dx12-poc] level-editor apply failed grid=(%d,%d) height=%d\n",
                            ix,
                            iz,
                            level_editor_height);
                }
                else
                {
                    printf("[dx12-poc] level-editor set grid=(%d,%d) height=%d brush=%d mode=%s\n",
                           ix,
                           iz,
                           level_editor_height,
                           proto_config.level_editor_brush_radius,
                           k_paint_mode_labels[proto_config.level_editor_paint_mode]);
                    flow_result.profile_dirty = 1;
                }
            }
        }

        if (flow_result.profile_dirty)
        {
            save_profile(menu_index,
                         character_index,
                         class_index,
                         options_index,
                         auto_target_enabled,
                         target_hotkey_enabled,
                         telemetry_enabled,
                         level_editor_height,
                         &proto_config);
        }

        if (flow_result.quit_requested)
            break;

        if (scene == BANANA_POC_SCENE_MAIN_MENU && previous_scene != BANANA_POC_SCENE_MAIN_MENU)
            engine_set_move_input(0.0f, 0.0f);

        if (!game_window)
        {
            game_window = find_game_window();
            if (game_window)
            {
                SetForegroundWindow(game_window);
                SetFocus(game_window);
            }
        }

        if (game_window && (now_ms - hud_updated_ms) >= 200ULL)
        {
            objective_collected = engine_get_pbj_pickup_collected();
            update_game_hud_title(game_window,
                                  scene,
                                  objective_collected,
                                  elapsed_seconds,
                                  banana_poc_objective_policy_timeout_seconds(&objective_policy));
            hud_updated_ms = now_ms;
        }

        if (game_window && (scene == BANANA_POC_SCENE_WORLD || scene == BANANA_POC_SCENE_LEVEL_EDITOR))
        {
            int rbutton_down = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ? 1 : 0;
            int lbutton_down = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
            int click_edge = (rbutton_down && !last_rbutton_down) || (lbutton_down && !last_lbutton_down);
            if (click_edge)
            {
                POINT cursor_screen;
                POINT cursor_client;
                if (GetCursorPos(&cursor_screen))
                {
                    cursor_client = cursor_screen;
                    if (ScreenToClient(game_window, &cursor_client))
                    {
                        engine_handle_right_click((float)cursor_client.x,
                                                  (float)cursor_client.y);
                    }
                }
            }
            last_rbutton_down = rbutton_down;
            last_lbutton_down = lbutton_down;

            if (target_hotkey_enabled)
            {
                int hotkey_down = (GetAsyncKeyState('T') & 0x8000) ? 1 : 0;
                if (hotkey_down && !last_target_hotkey_down)
                {
                    engine_handle_right_click((float)(BANANA_POC_WIDTH / 2),
                                              (float)(BANANA_POC_HEIGHT / 2));
                    printf("[dx12-poc] injected target click via T hotkey\n");
                }
                last_target_hotkey_down = hotkey_down;
            }

            if (!startup_smoke_mode && auto_target_enabled && !auto_target_injected && elapsed_seconds >= 2)
            {
                engine_handle_right_click((float)(BANANA_POC_WIDTH / 2),
                                          (float)(BANANA_POC_HEIGHT / 2));
                auto_target_injected = 1;
                printf("[dx12-poc] injected startup move target\n");
            }
        }
        else
        {
            last_rbutton_down = 0;
            last_lbutton_down = 0;
            last_target_hotkey_down = 0;
        }

        if (startup_smoke_mode && elapsed_seconds >= startup_smoke_seconds)
        {
            printf("[dx12-poc] startup smoke passed after %d second(s)\n", elapsed_seconds);
            break;
        }

        if (banana_poc_objective_policy_should_fail_timeout(&objective_policy,
                                                            startup_smoke_mode,
                                                            scene,
                                                            objective_collected,
                                                            elapsed_seconds))
        {
            fprintf(stderr, "[dx12-poc] objective failed: PBJ pickup timeout\n");
            if (game_window)
            {
                SetWindowTextA(game_window,
                               "Banana DX12 POC | Objective Failed (Timeout) | ESC to Exit");
            }
            exit_code = 2;
            break;
        }

        if (!QueryPerformanceCounter(&now_counter))
        {
            break;
        }

        dt = (float)(now_counter.QuadPart - previous_counter.QuadPart) / (float)frequency.QuadPart;
        previous_counter = now_counter;

        if (dt < 0.0f)
        {
            dt = 0.0f;
        }
        if (dt > 0.10f)
        {
            dt = 0.10f;
        }

        if (scene == BANANA_POC_SCENE_WORLD || scene == BANANA_POC_SCENE_LEVEL_EDITOR)
            update_keyboard_move_input();
        else
            engine_set_move_input(0.0f, 0.0f);

        tick_result = engine_tick(dt);
        if (tick_result != 0)
        {
            if (tick_result < 0)
            {
                exit_code = 1;
            }
            break;
        }

        objective_collected = engine_get_pbj_pickup_collected();
        render_ui_overlay(game_window,
                          scene,
                          menu_index,
                          character_index,
                          class_index,
                          options_index,
                          auto_target_enabled,
                          target_hotkey_enabled,
                          telemetry_enabled,
                          level_editor_height,
                          elapsed_seconds,
                          banana_poc_objective_policy_timeout_seconds(&objective_policy),
                          objective_collected,
                          startup_smoke_mode,
                          &proto_config);

        if (banana_poc_objective_policy_on_objective_collected(&objective_policy,
                                                                scene,
                                                                objective_collected))
        {
            printf("[dx12-poc] objective complete: PBJ pickup collected\n");
            if (game_window)
            {
                update_game_hud_title(game_window,
                                      scene,
                                      1,
                                      elapsed_seconds,
                                      banana_poc_objective_policy_timeout_seconds(&objective_policy));
            }
        }

        if (telemetry_enabled && (now_ms - telemetry_started_ms) >= (ULONGLONG)telemetry_interval_ms)
        {
            int entity_count = engine_get_entity_count();
            telemetry_started_ms = now_ms;
            printf("[dx12-poc] entities=%d clicks=%d target-reached=%d objective=%s elapsed=%ds\n",
                   entity_count,
                   engine_get_click_count(),
                   engine_get_target_reached_count(),
                   objective_collected ? "complete" : "in-progress",
                   elapsed_seconds);
            printf("[dx12-poc] dx12-telemetry %s\n", banana_dx12_runtime_telemetry());
        }
    }

    engine_ui_shutdown();
    engine_shutdown();
    save_profile(menu_index,
                 character_index,
                 class_index,
                 options_index,
                 auto_target_enabled,
                 target_hotkey_enabled,
                 telemetry_enabled,
                 level_editor_height,
                 &proto_config);
    free(s_ui_bgra_buffer);
    s_ui_bgra_buffer = NULL;
    s_ui_bgra_capacity = 0;
    printf("[dx12-poc] shutdown complete\n");
    return exit_code;
}

#else
int main(void)
{
    return 0;
}
#endif
