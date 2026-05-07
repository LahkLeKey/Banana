#ifndef BANANA_ENGINE_H
#define BANANA_ENGINE_H

/* ─────────────────────────────────────────────────────────────────────────────
 * banana_engine — public C API
 *
 * This header is the single include for the WASM orchestration layer.
 * Native code can include individual module headers directly.
 * ─────────────────────────────────────────────────────────────────────────── */

#include "render/window.h"
#include "render/shader.h"
#include "render/mesh.h"
#include "render/material.h"
#include "render/camera.h"
#include "render/renderer.h"
#include "physics/body.h"
#include "physics/collider.h"
#include "physics/dynamics.h"
#include "physics/world.h"
#include "ai/controller.h"
#include "ai/state_machine.h"
#include "ai/navigation.h"
#include "ai/perception.h"
#include "world/entity.h"
#include "world/world.h"
#include "world/signals.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ── Top-level lifecycle ────────────────────────────────────────────────────*/

/* Initialize engine subsystems (render, physics, world).
 * Must be called once before any other engine function.
 * width/height: initial framebuffer dimensions. */
int  engine_init(int width, int height);

/* Advance one game tick: physics step → controller updates → render frame.
 * dt: elapsed seconds since last tick.
 * Returns 0 on success. */
int  engine_tick(float dt);

/* Tear down all subsystems. */
void engine_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_H */
