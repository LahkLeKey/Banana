/**
 * engine-binding.ts — `initializeEngine()` helper.
 *
 * Loads the Emscripten JS glue from the given URL (defaults to `/wasm/engine.js`),
 * instantiates the WASM module, calls `_engine_init()`, and returns the typed
 * `BananaEngineModule` interface.
 *
 * Usage (React):
 *   const engine = await initializeEngine();
 *   engine._engine_tick(dt);
 */

import {type BananaEngineModule, isBananaEngineModule} from "./game-loop.wasm";

export interface EngineBindingOptions {
    /** URL of the Emscripten-generated JS file. Defaults to "/wasm/engine.js". */
    scriptUrl?: string;
    /**
     * Optional canvas element ID. Emscripten expects id="canvas" by default; set
     * a custom value if your layout uses a different id.
     */
    canvasId?: string;
    /** Timeout in ms while waiting for the WASM module to become ready. Default 10 000. */
    timeoutMs?: number;
}

/**
 * Load, instantiate, and initialise the Banana game engine WASM module.
 *
 * @throws {Error} if the script fails to load, the module is not ready within
 *                 `timeoutMs`, or the exported ABI surface is incomplete.
 */
export async function initializeEngine(
    options: EngineBindingOptions = {},
    ): Promise<BananaEngineModule>
{
    const {
        scriptUrl = "/wasm/engine.js",
        canvasId = "canvas",
        timeoutMs = 10_000,
    } = options;

    /* ── 1. Inject script tag ─────────────────────────────────────────────── */
    const existing = document.querySelector<HTMLScriptElement>(`script[src="${scriptUrl}"]`);

    if (!existing)
    {
        await new Promise<void>((resolve, reject) => {
            const el = document.createElement("script");
            el.src = scriptUrl;
            el.async = true;
            el.onload = () => resolve();
            el.onerror = () => reject(new Error(`Failed to load engine script: ${scriptUrl}`));
            document.head.appendChild(el);
        });
    }

    /* ── 2. Wait for the Emscripten Module object to be ready ─────────────── */
    const mod = await waitForModule("BananaEngine", timeoutMs);

    /* ── 3. Assert ABI completeness ───────────────────────────────────────── */
    if (!isBananaEngineModule(mod))
    {
        throw new Error(
            "BananaEngine module loaded but is missing required ABI exports. " +
                "Rebuild with the correct -sEXPORTED_FUNCTIONS list.",
        );
    }

    /* ── 4. Initialise ────────────────────────────────────────────────────── */
    // Ensure canvas is available — Emscripten references it during init.
    const canvas = document.getElementById(canvasId);
    if (!canvas)
    {
        throw new Error(
            `Canvas element with id="${canvasId}" not found. ` +
                "Add <canvas id=\"canvas\"> to the page before calling initializeEngine().",
        );
    }

    mod._engine_init();

    return mod;
}

/* ── Internals ───────────────────────────────────────────────────────────── */

/**
 * Poll `window[globalName]` until the Emscripten module signals readiness
 * (resolved Promise from `Module.ready` or the factory function settles),
 * or until `timeoutMs` elapses.
 */
async function waitForModule(globalName: string, timeoutMs: number): Promise<unknown>
{
    const deadline = Date.now() + timeoutMs;

    while (Date.now() < deadline)
    {
        const candidate = (window as Record<string, unknown>)[globalName];
        if (candidate != null)
        {
            /* Emscripten with MODULARIZE=1 returns a factory; call it. */
            if (typeof candidate === "function")
            {
                const instance = await (candidate as () => Promise<unknown>)();
                return instance;
            }
            /* Already an object (pre-instantiated or MODULARIZE=0). */
            return candidate;
        }
        await sleep(50);
    }

    throw new Error(
        `Timed out waiting for Emscripten module "${globalName}" after ${timeoutMs} ms.`,
    );
}

function sleep(ms: number)
{
    return new Promise<void>((r) => setTimeout(r, ms));
}
