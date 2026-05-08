/**
 * Spec 261: Test scenario helpers for E2E validation
 *
 * Provides query parameter-based test modes and event listeners
 * to simulate failure scenarios and state transitions for playwright e2e tests.
 */

import { useEffect } from "react";

export type Spec261TestMode =
  | "fail-wasm-fetch"
  | "fail-wasm-fetch-once"
  | "hang-first-frame"
  | null;

/**
 * Parse spec261 test mode from URL query params
 */
export function useSpec261TestMode(): Spec261TestMode {
  const mode = new URLSearchParams(typeof window !== "undefined" ? window.location.search : "").get(
    "spec261-test"
  );
  return (mode as Spec261TestMode) || null;
}

/**
 * Inject fetch/WASM mocking for test scenarios
 */
export function useSpec261TestMocking() {
  const mode = useSpec261TestMode();

  useEffect(() => {
    if (!mode) return;

    let fetchAttempt = 0;
    const originalFetch = globalThis.fetch;

    const mockFetch = async (input: RequestInfo | URL, init?: RequestInit) => {
      const url = typeof input === "string" ? input : input.toString();

      if (!url.includes("/engine.wasm")) {
        return originalFetch(input, init);
      }

      // fail-wasm-fetch: always reject
      if (mode === "fail-wasm-fetch") {
        return Promise.reject(new Error("Spec261TestMode: WASM fetch failed"));
      }

      // fail-wasm-fetch-once: reject first attempt, succeed on retry
      if (mode === "fail-wasm-fetch-once") {
        fetchAttempt++;
        if (fetchAttempt === 1) {
          return Promise.reject(
            new Error("Spec261TestMode: WASM fetch failed (retry will succeed)")
          );
        }
        // After first failure, fetch from real WASM URL
        // For test purposes, return a minimal valid ArrayBuffer
        const wasmBinary = new Uint8Array([0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00]); // Minimal WASM header
        return Promise.resolve(new Response(wasmBinary.buffer));
      }

      // hang-first-frame: never emit first frame (fetch succeeds but engine hangs)
      if (mode === "hang-first-frame") {
        // Return a valid minimal WASM that will instantiate but never call engine_tick
        const wasmBinary = new Uint8Array([0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00]);
        return Promise.resolve(new Response(wasmBinary.buffer));
      }

      return originalFetch(input, init);
    };

    globalThis.fetch = mockFetch as typeof fetch;

    return () => {
      globalThis.fetch = originalFetch;
    };
  }, [mode]);
}

/**
 * Listen for spec261 overlay control events from E2E tests
 */
export function useSpec261OverlayControls(onOpenMenu?: () => void, onCloseMenu?: () => void) {
  useEffect(() => {
    const handleOpenMenu = () => onOpenMenu?.();
    const handleCloseMenu = () => onCloseMenu?.();

    window.addEventListener("spec261:openMenu", handleOpenMenu);
    window.addEventListener("spec261:closeMenu", handleCloseMenu);

    return () => {
      window.removeEventListener("spec261:openMenu", handleOpenMenu);
      window.removeEventListener("spec261:closeMenu", handleCloseMenu);
    };
  }, [onOpenMenu, onCloseMenu]);
}
