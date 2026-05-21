#ifndef BANANA_ENGINE_H
#define BANANA_ENGINE_H

/* ─────────────────────────────────────────────────────────────────────────────
 * banana_engine — public C API
 *
 * This header is the single include for the WASM orchestration layer.
 * Native code can include individual module headers directly.
 * ─────────────────────────────────────────────────────────────────────────── */

#include "ai/controller.h"
#include "ai/navigation.h"
#include "ai/perception.h"
#include "ai/state_machine.h"
#include "physics/body.h"
#include "physics/collider.h"
#include "physics/dynamics.h"
#include "physics/world.h"
#include "render/camera.h"
#include "render/material.h"
#include "render/mesh.h"
#include "render/renderer.h"
#include "render/shader.h"
#include "render/window.h"
#include "world/entity.h"
#include "world/signals.h"
#include "world/world.h"
#include "domain/services/batch_engine_bridge.h"
#include "domain/services/ml_controller_bridge.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* ── Top-level lifecycle ────────────────────────────────────────────────────*/

    /* Initialize engine subsystems (render, physics, world).
     * Must be called once before any other engine function.
     * width/height: initial framebuffer dimensions. */
    int engine_init(int width, int height);

    /* Advance one game tick: physics step → controller updates → render frame.
     * dt: elapsed seconds since last tick.
     * Returns 0 on success. */
    int engine_tick(float dt);

    /* Tear down all subsystems. */
    void engine_shutdown(void);

    /* ── Controller helpers ─────────────────────────────────────────────────────*/

    /* Create a controller and attach it to an existing world entity.
     * Returns the controller ID (0 on failure). */
    uint32_t engine_controller_attach(uint32_t entity_id, const char *type_name);

    /* ── Entity query helpers (for React / WASM telemetry overlay) ──────────── */
    int engine_get_entity_count(void);
    float engine_get_entity_x(int idx);
    float engine_get_entity_z(int idx);
    int engine_get_entity_state(int idx); /* 0 = inactive, 1 = active */

    /* Keyboard movement intent only. Inputs are expected in [-1, 1]. */
    void engine_set_move_input(float input_x, float input_z);

    /* Debug diagnostics for de-scoped mouse movement path. */
    int engine_get_click_count(void);
    int engine_get_target_reached_count(void);
    int engine_get_has_move_target(void);
    int engine_handle_right_click(float canvas_x, float canvas_y);
    int engine_handle_right_click_normalized(float screen_x, float screen_y);

    /* ── Terrain update helpers (WFC/CA integration) ─────────────────────── */
    int engine_terrain_set_height(int x, int z, int elevation);
    void engine_terrain_mark_region_dirty(int min_x, int min_z, int max_x, int max_z);

    // Bridge for batch domain integration

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_H */
