/**
 * useInputRouter.ts — Deterministic input routing between viewport and overlay modes.
 *
 * Spec 261 T005/T006: Input routing hook with explicit viewport/overlay mode transitions.
 *
 * Modes:
 *   "viewport" — canvas owns keyboard focus; WASM captures all pointer events.
 *   "overlay"  — canvas is blurred; pointer events pass through to React overlays.
 *
 * Transitions:
 *   - overlayMode=false → canvas.focus(), pointer capture active.
 *   - overlayMode=true  → canvas.blur(), pointer events pass to overlays.
 *
 * HUD (layer 10) and Debug (layer 40) never trigger overlay mode.
 * Menu (layer 20) triggers soft lock; Modal (layer 30) triggers hard lock.
 *
 * React overlay components set data-overlay="true" on their interactive roots.
 */

import { useEffect } from "react";

import type { WasmEngine } from "./useWasmLoader";

export type InputMode = "viewport" | "overlay";

function isOverlayTarget(target: EventTarget | null): boolean {
  if (!(target instanceof Element)) return false;
  return target.closest("[data-overlay]") !== null;
}

const GAME_KEYS = new Set(["ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight", " ", "Escape"]);

export function useInputRouter(
  canvasRef: React.RefObject<HTMLCanvasElement | null>,
  engine: WasmEngine | null,
  overlayMode = false
): void {
  // Transition: overlay mode → blur canvas, stop capturing
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    if (overlayMode) {
      canvas.blur();
    } else if (engine) {
      canvas.setAttribute("tabindex", "0");
      canvas.focus();
    }
  }, [overlayMode, canvasRef, engine]);

  // Bind input handlers when engine is present
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas || !engine) return;

    canvas.setAttribute("tabindex", "0");
    if (!overlayMode) canvas.focus();

    function onKeyDown(e: KeyboardEvent) {
      if (overlayMode) return;
      if (GAME_KEYS.has(e.key)) {
        e.preventDefault();
      }
    }

    function onPointerDown(e: PointerEvent) {
      if (overlayMode || isOverlayTarget(e.target)) return;
      canvas?.setPointerCapture(e.pointerId);
    }

    canvas.addEventListener("keydown", onKeyDown);
    canvas.addEventListener("pointerdown", onPointerDown);

    return () => {
      canvas.removeEventListener("keydown", onKeyDown);
      canvas.removeEventListener("pointerdown", onPointerDown);
    };
  }, [canvasRef, engine, overlayMode]);
}
