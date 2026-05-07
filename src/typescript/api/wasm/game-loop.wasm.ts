/**
 * game-loop.wasm.ts — TypeScript types that mirror the C engine's WASM ABI.
 *
 * The engine is compiled via Emscripten to `public/wasm/engine.js` + `engine.wasm`.
 * React loads the JS glue at runtime via a <script> tag; this file provides the
 * interface shape for type-safe consumption of the resulting global.
 *
 * IMPORTANT: All exported C functions are prefixed with `_` by Emscripten when
 * exported via `-sEXPORTED_FUNCTIONS`.
 */

/* ── Core engine lifecycle ──────────────────────────────────────────────────── */
export interface BananaEngineModule {
    /** Called once after canvas context is ready. */
    _engine_init(): void;

    /** Advance all subsystems by one tick (physics, world, AI, render). */
    _engine_tick(dt: number): void;

    /* ── Entity query ABI ─────────────────────────────────────────────────── */

    /** Total number of entities currently tracked by the engine. */
    _engine_get_entity_count(): number;

    /** World-space X coordinate for entity at index `idx`. */
    _engine_get_entity_x(idx: number): number;

    /** World-space Z coordinate for entity at index `idx`. */
    _engine_get_entity_z(idx: number): number;

    /**
     * Controller state integer for entity at `idx`.
     * Mapped values:
     *   0 = inactive / spawning
     *   1 = patrol
     *   2 = investigate
     *   3 = return-to-base
     */
    _engine_get_entity_state(idx: number): number;

    /* ── Controller ABI ───────────────────────────────────────────────────── */

    /** Create a named controller instance; returns an opaque handle or 0 on failure. */
    _engine_controller_create(typeName: string): number;

    /** Attach an existing controller to an entity; returns non-zero on success. */
    _engine_controller_attach(entityId: number, typeName: string): number;

    /** Advance a single controller by `dt` seconds. */
    _engine_controller_update(handle: number, dt: number): void;

    /** Send a named signal (with optional float data) to a controller. */
    _engine_controller_signal(handle: number, signal: string, data: number): void;

    /* ── Physics ABI ─────────────────────────────────────────────────────── */

    /** Step the physics world by `dt` seconds. */
    _physics_step(dt: number): void;

    /** Register a new rigid body; returns its integer ID. */
    _physics_add_body(x: number, y: number, z: number, mass: number): number;

    /** Overwrite the position of body `id`. */
    _physics_update_body(id: number, x: number, y: number, z: number): void;

    /* ── World / spawn ABI ───────────────────────────────────────────────── */

    /**
     * Spawn a new entity at the given world position.
     * Returns the entity ID or 0 on failure.
     */
    _engine_world_spawn(x: number, y: number, z: number, type: number): number;

    /** Advance the world simulation. */
    _engine_world_tick(dt: number): void;
}

/* ── Helpers ─────────────────────────────────────────────────────────────── */

/**
 * Narrow `unknown` to `BananaEngineModule` at runtime.
 * Use this after loading the engine.js script tag.
 */
export function isBananaEngineModule(v: unknown): v is BananaEngineModule {
    if (typeof v !== "object" || v === null) return false;
    const m = v as Record<string, unknown>;
    return (
        typeof m["_engine_init"] === "function" &&
        typeof m["_engine_tick"] === "function" &&
        typeof m["_engine_get_entity_count"] === "function"
    );
}
