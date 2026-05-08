/**
 * useInputRouter.ts — Keyboard/pointer input routing hook.
 *
 * Spec 261 T007: Input router captures keyboard/pointer for WASM by default;
 * UI events bubble to React overlays when they originate from overlay elements.
 *
 * Strategy:
 *   - keydown/keyup: always forwarded to WASM via the engine canvas element.
 *   - pointerdown/pointermove/pointerup: forwarded to WASM unless the event
 *     target is an overlay element (data-overlay="true").
 *   - React overlay components should set data-overlay="true" on their roots
 *     so pointer events bubble to them instead of the engine.
 */

import { useEffect } from "react";

import type { WasmEngine } from "./useWasmLoader";

function isOverlayTarget(target: EventTarget | null): boolean {
  if (!(target instanceof Element)) return false;
  return target.closest("[data-overlay]") !== null;
}

export function useInputRouter(
  canvasRef: React.RefObject<HTMLCanvasElement | null>,
  engine: WasmEngine | null
): void {
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas || !engine) return;

    // Focus canvas so keyboard events land here
    canvas.setAttribute("tabindex", "0");
    canvas.focus();

    function onKeyDown(e: KeyboardEvent) {
      // Prevent default browser shortcuts that would hijack game input
      if (
        e.key === "ArrowUp" ||
        e.key === "ArrowDown" ||
        e.key === "ArrowLeft" ||
        e.key === "ArrowRight" ||
        e.key === " "
      ) {
        e.preventDefault();
      }
    }

    function onPointerDown(e: PointerEvent) {
      if (isOverlayTarget(e.target)) return;
      canvas?.setPointerCapture(e.pointerId);
    }

    canvas.addEventListener("keydown", onKeyDown);
    canvas.addEventListener("pointerdown", onPointerDown);

    return () => {
      canvas.removeEventListener("keydown", onKeyDown);
      canvas.removeEventListener("pointerdown", onPointerDown);
    };
  }, [canvasRef, engine]);
}
