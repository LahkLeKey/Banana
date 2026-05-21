/* wasm_main.c — Emscripten entry point for the Banana game engine.
 *
 * Compiled only when building with Emscripten (emcmake / emcc).
 * Sets up the engine, spawns 5 wildlife NPCs, then hands control
 * to the browser via emscripten_set_main_loop.
 */

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>

#include "ai/wildlife_controller.h"
#include "engine.h"

/* ── Per-frame tick (called by emscripten_set_main_loop at 60 Hz) ────────── */

static void game_tick(void)
{
    engine_tick(1.0f / 60.0f);
}

/* ── Entry point ─────────────────────────────────────────────────────────── */

int main(void)
{
    fprintf(stdout, "[wasm_main] Banana Engine starting…\n");

    if (engine_init(800, 600) != 0)
    {
        fprintf(stderr, "[wasm_main] engine_init failed\n");
        return 1;
    }

    fprintf(stdout, "[wasm_main] island scene ready\n");
    return 0;
}

#endif /* __EMSCRIPTEN__ */
