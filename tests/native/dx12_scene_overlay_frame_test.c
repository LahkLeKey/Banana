#include "../../../src/native/engine/engine.h"
#include "../../../src/native/engine/win32_dx12_poc/scene_overlay.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BANANA_POC_TEST_WIDTH 1280
#define BANANA_POC_TEST_HEIGHT 720

static void fail_test(const char *label, const char *message)
{
    fprintf(stderr, "FAIL %s: %s\n", label, message);
    exit(1);
}

static void expect_nonzero_u64(const char *label, uint64_t value)
{
    if (value == 0)
        fail_test(label, "expected non-zero hash");
}

static void expect_distinct_u64(const char *label, uint64_t left, uint64_t right)
{
    if (left == right)
        fail_test(label, "expected distinct frame hash");
}

static uint64_t fnv1a64(const unsigned char *bytes, size_t size)
{
    uint64_t hash = 1469598103934665603ULL;
    size_t i = 0;

    for (i = 0; i < size; i++)
    {
        hash ^= (uint64_t)bytes[i];
        hash *= 1099511628211ULL;
    }

    return hash;
}

static uint64_t render_scene_hash(BananaPocScene scene,
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
                                  int startup_smoke_mode)
{
    const unsigned char *frame = NULL;

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
                                    NULL);

    frame = engine_ui_get_framebuffer();
    if (!frame)
        fail_test("framebuffer", "engine_ui_get_framebuffer returned NULL");

    return fnv1a64(frame, (size_t)BANANA_POC_TEST_WIDTH * (size_t)BANANA_POC_TEST_HEIGHT * 4u);
}

int main(void)
{
    uint64_t main_menu_hash = 0;
    uint64_t character_hash = 0;
    uint64_t options_hash = 0;
    uint64_t world_hash = 0;
    uint64_t editor_hash = 0;

    if (engine_init(BANANA_POC_TEST_WIDTH, BANANA_POC_TEST_HEIGHT) != 0)
        fail_test("engine_init", "failed to initialize runtime");
    if (engine_ui_init(BANANA_POC_TEST_WIDTH, BANANA_POC_TEST_HEIGHT) != 0)
        fail_test("engine_ui_init", "failed to initialize UI");

    main_menu_hash = render_scene_hash(BANANA_POC_SCENE_MAIN_MENU,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       2,
                                       0,
                                       0,
                                       0,
                                       0);
    character_hash = render_scene_hash(BANANA_POC_SCENE_CHARACTER_SELECT,
                                       0,
                                       2,
                                       1,
                                       0,
                                       0,
                                       0,
                                       0,
                                       2,
                                       0,
                                       0,
                                       0,
                                       0);
    options_hash = render_scene_hash(BANANA_POC_SCENE_OPTIONS,
                                     0,
                                     0,
                                     0,
                                     2,
                                     1,
                                     1,
                                     0,
                                     2,
                                     0,
                                     0,
                                     0,
                                     0);

    if (engine_tick(0.016f) != 0)
        fail_test("engine_tick", "runtime frame update failed");

    world_hash = render_scene_hash(BANANA_POC_SCENE_WORLD,
                                   0,
                                   0,
                                   0,
                                   0,
                                   1,
                                   0,
                                   1,
                                   2,
                                   1,
                                   0,
                                   0,
                                   0);
    editor_hash = render_scene_hash(BANANA_POC_SCENE_LEVEL_EDITOR,
                                    0,
                                    0,
                                    0,
                                    0,
                                    1,
                                    0,
                                    1,
                                    3,
                                    1,
                                    0,
                                    0,
                                    0);

    expect_nonzero_u64("main_menu_hash", main_menu_hash);
    expect_nonzero_u64("character_hash", character_hash);
    expect_nonzero_u64("options_hash", options_hash);
    expect_nonzero_u64("world_hash", world_hash);
    expect_nonzero_u64("editor_hash", editor_hash);

    expect_distinct_u64("main_menu_vs_character", main_menu_hash, character_hash);
    expect_distinct_u64("main_menu_vs_options", main_menu_hash, options_hash);
    expect_distinct_u64("main_menu_vs_world", main_menu_hash, world_hash);
    expect_distinct_u64("main_menu_vs_editor", main_menu_hash, editor_hash);
    expect_distinct_u64("character_vs_options", character_hash, options_hash);
    expect_distinct_u64("character_vs_world", character_hash, world_hash);
    expect_distinct_u64("character_vs_editor", character_hash, editor_hash);
    expect_distinct_u64("options_vs_world", options_hash, world_hash);
    expect_distinct_u64("options_vs_editor", options_hash, editor_hash);
    expect_distinct_u64("world_vs_editor", world_hash, editor_hash);

    printf("scene hashes: main=%llu character=%llu options=%llu world=%llu editor=%llu\n",
           (unsigned long long)main_menu_hash,
           (unsigned long long)character_hash,
           (unsigned long long)options_hash,
           (unsigned long long)world_hash,
           (unsigned long long)editor_hash);

    engine_ui_shutdown();
    engine_shutdown();
    return 0;
}