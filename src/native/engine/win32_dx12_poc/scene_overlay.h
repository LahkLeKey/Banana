#ifndef BANANA_ENGINE_WIN32_DX12_POC_SCENE_OVERLAY_H
#define BANANA_ENGINE_WIN32_DX12_POC_SCENE_OVERLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

#define BANANA_POC_WIDTH 1280
#define BANANA_POC_HEIGHT 720

typedef enum BananaPocScene
{
    BANANA_POC_SCENE_MAIN_MENU      = 0,
    BANANA_POC_SCENE_CHARACTER_SELECT = 1,
    BANANA_POC_SCENE_WORLD          = 2,
    BANANA_POC_SCENE_LEVEL_EDITOR   = 3,
    BANANA_POC_SCENE_OPTIONS        = 4,
    BANANA_POC_SCENE_SCENE_BROWSER  = 5,
    BANANA_POC_SCENE_CONFIG_LAB     = 6,
} BananaPocScene;

/* Prototype/dev configuration state passed into the overlay renderer.
   All fields default to 0 (safe/off) when zero-initialised. */
typedef struct BananaPocProtoConfig
{
    int scene_browser_index;  /* cursor row in Scene Browser (catalog-driven) */
    int config_lab_index;     /* cursor row in Config Lab   (0-3)   */
    int character_loadout_index; /* 0=Balanced 1=Tank 2=Burst 3=Support */
    int level_editor_brush_radius; /* 1..4 */
    int level_editor_paint_mode;   /* 0=Sculpt 1=Flatten 2=Smooth */
    int option_input_assist; /* option menu toggle */
    int option_compact_hud;  /* option menu toggle */
    int active_world_variant; /* current launched scene browser variant id */
    int render_debug_overlay; /* 0=off  1=on                        */
    int physics_wireframe;    /* 0=off  1=on                        */
    int world_seed_index;     /* 0=Default 1=42 2=1337 3=99         */
    int frame_cap_index;      /* 0=30fps 1=60fps 2=120fps 3=Uncapped */
} BananaPocProtoConfig;

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
                                     const BananaPocProtoConfig *proto);

#ifdef __cplusplus
}
#endif

#endif