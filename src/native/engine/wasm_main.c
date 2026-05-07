/* wasm_main.c — Emscripten entry point for the Banana game engine.
 *
 * Compiled only when building with Emscripten (emcmake / emcc).
 * Sets up the engine, spawns 5 wildlife NPCs, then hands control
 * to the browser via emscripten_set_main_loop.
 */

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <stdlib.h>
#include <stdio.h>

#include "engine.h"
#include "ai/wildlife_controller.h"

/* Number of NPCs to spawn in the opening world. */
#define WASM_NPC_COUNT 5

/* ── Per-frame tick (called by emscripten_set_main_loop at 60 Hz) ────────── */

static void game_tick(void) {
    engine_tick(1.0f / 60.0f);
}

/* ── Entry point ─────────────────────────────────────────────────────────── */

int main(void) {
    fprintf(stdout, "[wasm_main] Banana Engine starting…\n");

    /* Register AI controller types before engine_init so that
     * engine_controller_attach can look them up during world setup. */
    wildlife_controller_register();

    if (engine_init(800, 600) != 0) {
        fprintf(stderr, "[wasm_main] engine_init failed\n");
        return 1;
    }

    /* Spawn NPC wildlife entities in a loose grid. */
    float starts[WASM_NPC_COUNT][2] = {
        { 2.0f,  2.0f},
        {12.0f,  4.0f},
        { 6.0f, 14.0f},
        {18.0f, 10.0f},
        {10.0f, 20.0f},
    };

    for (int i = 0; i < WASM_NPC_COUNT; i++) {
        uint32_t eid = engine_world_spawn(ENTITY_TYPE_NPC,
                                          starts[i][0], 0.0f, starts[i][1]);
        engine_controller_attach(eid, "wildlife");
    }

    fprintf(stdout, "[wasm_main] %d NPCs spawned — starting game loop\n",
            WASM_NPC_COUNT);

    /* Hand control to the browser; main() returns immediately. */
    emscripten_set_main_loop(game_tick, 0 /* use rAF, no fps cap */, 1);

    engine_shutdown();
    return 0;
}

#endif /* __EMSCRIPTEN__ */
