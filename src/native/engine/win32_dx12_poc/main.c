#if defined(_WIN32)

#include "../engine.h"
#include "../render/backend.h"
#include "../render/backend_dx12.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

static const char *BANANA_DX12_WINDOW_CLASS = "BananaDx12RuntimeWindow";
static const int BANANA_POC_WIDTH = 1280;
static const int BANANA_POC_HEIGHT = 720;

static unsigned char *s_ui_bgra_buffer = NULL;
static size_t s_ui_bgra_capacity = 0;

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
    float move_z = axis_value('W', 'S');
    float length = sqrtf((move_x * move_x) + (move_z * move_z));

    if (length > 1.0f)
    {
        move_x /= length;
        move_z /= length;
    }

    engine_set_move_input(move_x, move_z);
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

static HWND find_game_window(void)
{
    return FindWindowA(BANANA_DX12_WINDOW_CLASS, NULL);
}

static void update_game_hud_title(HWND game_window,
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

    if (objective_collected)
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
                              int elapsed_seconds,
                              int objective_timeout_seconds,
                              int objective_collected,
                              int startup_smoke_mode)
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
    const unsigned char *ui_framebuffer = NULL;

    if (!game_window)
    {
        return;
    }

    if (remaining_seconds < 0)
    {
        remaining_seconds = 0;
    }

    if (world_half_span < 1.0f)
    {
        world_half_span = 1.0f;
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

    engine_ui_begin_frame();
    engine_ui_panel(12.0f, 12.0f, 760.0f, 94.0f, 0x0A1118C0u, 1.0f);
    engine_ui_panel(12.0f, 112.0f, 420.0f, 92.0f, 0x121E28C0u, 1.0f);
    engine_ui_panel(minimap_x, minimap_y, minimap_w, minimap_h, 0x1A2A22D0u, 1.0f);

    snprintf(line_one,
             sizeof(line_one),
             "BANANA DX12 POC  |  WASD MOVE  |  RIGHT-CLICK MOVE TARGET  |  ESC QUIT");

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

        snprintf(line_five,
                 sizeof(line_five),
                 "NAV HINT: X -> %s  Z -> %s  DELTA=(%.2f, %.2f)",
                 hint_x,
                 hint_z,
                 nav_dx,
                 nav_dz);
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
    engine_ui_text(22.0f, 148.0f, line_five);
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
    ui_framebuffer = engine_ui_get_framebuffer();
    blit_ui_overlay(game_window, ui_framebuffer, BANANA_POC_WIDTH, BANANA_POC_HEIGHT);
}

int main(void)
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER previous_counter;
    ULONGLONG telemetry_started_ms = 0;
    ULONGLONG hud_updated_ms = 0;
    ULONGLONG session_started_ms = 0;
    HWND game_window = NULL;
    int objective_timeout_seconds = 0;
    int startup_smoke_mode = 0;
    int startup_smoke_seconds = 0;
    int auto_target_enabled = 1;
    int target_hotkey_enabled = 0;
    int exit_code = 0;
    int last_rbutton_down = 0;
    int last_target_hotkey_down = 0;
    int auto_target_injected = 0;

    if (!QueryPerformanceFrequency(&frequency) || !QueryPerformanceCounter(&previous_counter))
    {
        fprintf(stderr, "[dx12-poc] high resolution timer unavailable\n");
        return 1;
    }

    printf("[dx12-poc] launching Banana playable prototype\n");
    printf("[dx12-poc] controls: WASD move, right-click move target, ESC quit\n");
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
    objective_timeout_seconds = parse_env_int("BANANA_DX12_POC_OBJECTIVE_TIMEOUT_SECONDS", 0, 0, 3600);
    auto_target_enabled = parse_env_int("BANANA_DX12_POC_AUTO_TARGET", 0, 0, 1);
    target_hotkey_enabled = parse_env_int("BANANA_DX12_POC_ENABLE_TARGET_HOTKEY", 0, 0, 1);

    telemetry_started_ms = GetTickCount64();
    hud_updated_ms = telemetry_started_ms;
    session_started_ms = telemetry_started_ms;
    for (;;)
    {
        LARGE_INTEGER now_counter;
        float dt = 0.0f;
        ULONGLONG now_ms = GetTickCount64();
        int elapsed_seconds = (int)((now_ms - session_started_ms) / 1000ULL);
        int objective_collected = 0;
        int tick_result = 0;

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            break;
        }

        if (!game_window)
        {
            game_window = find_game_window();
        }

        if (game_window && (now_ms - hud_updated_ms) >= 200ULL)
        {
            objective_collected = engine_get_pbj_pickup_collected();
            update_game_hud_title(game_window,
                                  objective_collected,
                                  elapsed_seconds,
                                  objective_timeout_seconds);
            hud_updated_ms = now_ms;
        }

        if (game_window)
        {
            int rbutton_down = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ? 1 : 0;
            if (rbutton_down && !last_rbutton_down)
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

        if (startup_smoke_mode && elapsed_seconds >= startup_smoke_seconds)
        {
            printf("[dx12-poc] startup smoke passed after %d second(s)\n", elapsed_seconds);
            break;
        }

        if (!startup_smoke_mode && objective_timeout_seconds > 0 && elapsed_seconds >= objective_timeout_seconds)
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

        update_keyboard_move_input();

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
                          elapsed_seconds,
                          objective_timeout_seconds,
                          objective_collected,
                          startup_smoke_mode);

        if (objective_collected)
        {
            printf("[dx12-poc] objective complete: PBJ pickup collected\n");
            if (game_window)
            {
                update_game_hud_title(game_window,
                                      1,
                                      elapsed_seconds,
                                      objective_timeout_seconds);
            }
            break;
        }

        if ((now_ms - telemetry_started_ms) >= 1000ULL)
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
