/**
 * WasmViewport.tsx — Full-screen WASM canvas background.
 *
 * Spec 261 T002/T003/T008/T009: WasmViewport component — canvas at z-index 0,
 * loads the WASM engine, drives the requestAnimationFrame game loop,
 * handles input routing, and emits frame telemetry.
 *
 * CSS contract:
 *   position: fixed; inset: 0; z-index: 0; width: 100vw; height: 100vh
 *
 * The component renders nothing visible when the engine is loading or has
 * errored — callers compose WasmErrorBanner and overlays on top.
 */

import { useRef } from "react";

import { useFrameTelemetry } from "./useFrameTelemetry";
import { useGameLoop } from "./useGameLoop";
import { useInputRouter } from "./useInputRouter";
import { useWasmLoader } from "./useWasmLoader";

interface WasmViewportProps {
  /** Called with the error message when WASM fails to load. */
  onError?: (message: string) => void;
}

export function WasmViewport({ onError }: WasmViewportProps) {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const { engine, error } = useWasmLoader();
  const loop = useGameLoop(engine);

  useInputRouter(canvasRef, engine);
  useFrameTelemetry(loop);

  if (error && onError) {
    // Propagate error to parent on next render cycle via ref-guard
    // (avoid calling setState of parent during child render)
    void Promise.resolve().then(() => onError(error));
  }

  return (
    <canvas
      ref={canvasRef}
      aria-label="Game viewport"
      style={{
        position: "fixed",
        inset: 0,
        zIndex: 0,
        width: "100vw",
        height: "100vh",
        display: "block",
        backgroundColor: "#000",
      }}
    />
  );
}
