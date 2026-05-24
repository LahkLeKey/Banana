#ifndef BANANA_RUNTIME_ARCHITECTURE_GUARD_RULES_H
#define BANANA_RUNTIME_ARCHITECTURE_GUARD_RULES_H

typedef struct FileRule
{
    const char *relative_path;
    const char *description;
    const char *forbidden_patterns[8];
    int forbidden_count;
} FileRule;

static const FileRule kRuntimeArchitectureGuardRules[] = {
    {
        "runtime/orchestration/tick/player_tick_orchestration.c",
        "orchestration must not depend on render/ui adapters",
        {"#include \"../render/", "#include \"../../render/", "#include \"../ui/", "#include \"../../ui/"},
        4,
    },
    {
        "runtime/orchestration/tick/render_tick_orchestration.c",
        "orchestration must not depend on render/ui adapters",
        {"#include \"../render/", "#include \"../../render/", "#include \"../ui/", "#include \"../../ui/"},
        4,
    },
    {
        "runtime/orchestration/tick/terrain_tick_orchestration.c",
        "orchestration must not depend on render/ui adapters",
        {"#include \"../render/", "#include \"../../render/", "#include \"../ui/", "#include \"../../ui/"},
        4,
    },
    {
        "runtime/orchestration/tick/runtime_tick_orchestration.h",
        "orchestration contracts must stay render-agnostic",
        {"#include \"../render/", "#include \"../../render/"},
        2,
    },
    {
        "runtime/camera/follow/camera_follow_policy.c",
        "domain services must not depend on orchestration",
        {"orchestration/"},
        1,
    },
    {
        "runtime/engine/gameplay_service.c",
        "domain services must not depend on orchestration",
        {"orchestration/"},
        1,
    },
    {
        "runtime/input/move_target_service.c",
        "domain services must not depend on orchestration",
        {"orchestration/"},
        1,
    },
    {
        "runtime/player/player_runtime_service.c",
        "domain services must not depend on orchestration",
        {"orchestration/"},
        1,
    },
    {
        "runtime/terrain/terrain_runtime.c",
        "domain services must not depend on orchestration",
        {"orchestration/"},
        1,
    },
    {
        "runtime/engine/engine_composition.c",
        "composition should keep service ports in explicit state, not hidden globals",
        {"static const RuntimeApplicationServicePorts *s_service_ports"},
        1,
    },
    {
        "runtime/engine/engine_tick.c",
        "tick orchestration should delegate click normalization policy",
        {"(click_x / (float)input_width) * 2.f - 1.f",
         "1.f - (click_y / (float)input_height) * 2.f"},
        2,
    },
    {
        "runtime/engine/engine_tick.c",
        "tick orchestration should delegate terrain budget policy",
        {"runtime_phase_terrain_budget(context, terrain_rebuild, 2)"},
        1,
    },
    {
        "runtime/engine/engine_tick.c",
        "tick orchestration should delegate right-click polling to tick input phase",
        {"window_take_right_click("},
        1,
    },
    {
        "runtime/engine/engine_tick.c",
        "tick orchestration should delegate post-gameplay sequencing to tick post phase",
        {"runtime_sync_controller_positions("},
        1,
    },
    {
        "runtime/engine/engine_tick.c",
        "engine tick adapter should delegate sequencing to runtime tick orchestration",
        {"runtime_tick_input_phase_process(",
         "runtime_phase_viewport_resize(",
         "runtime_phase_terrain_budget(",
         "runtime_tick_post_phase_execute("},
        4,
    },
    {
        "render/backend/backend_dx12.c",
        "dx12 backend should delegate diagnostic state ownership to helper module",
        {"static char s_dx12_telemetry[", "static int s_dx12_probe_state", "static const char *s_dx12_probe_status"},
        3,
    },
    {
        "render/backend/backend_dx12.c",
        "render adapters must not depend on orchestration",
        {"runtime/orchestration/", "../runtime/orchestration/"},
        2,
    },
    {
        "render/dx12/backend_dx12_diagnostics.c",
        "dx12 diagnostics helper must stay render/runtime boundary agnostic",
        {"runtime/orchestration/", "../runtime/orchestration/", "win32_dx12_poc/"},
        3,
    },
    {
        "render/renderer/renderer.c",
        "render adapters must not depend on orchestration",
        {"runtime/orchestration/", "../runtime/orchestration/"},
        2,
    },
    {
        "render/window/window.c",
        "render adapters must not depend on orchestration",
        {"runtime/orchestration/", "../runtime/orchestration/"},
        2,
    },
};

static int runtime_architecture_guard_rule_count(void)
{
    return (int)(sizeof(kRuntimeArchitectureGuardRules) / sizeof(kRuntimeArchitectureGuardRules[0]));
}

#endif /* BANANA_RUNTIME_ARCHITECTURE_GUARD_RULES_H */