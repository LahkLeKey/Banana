/**
 * WasmViewport.tsx — Full-screen WASM canvas background with lifecycle state machine.
 *
 * Spec 261 T004/T007/T008: Self-contained viewport component.
 * - Canvas at z-index 0 (full-screen background)
 * - WasmErrorBanner at z-index 30 when degraded (retry + API fallback)
 * - FpsOverlay at z-index 40 when showDebug=true
 *
 * CSS contract: position: fixed; inset: 0; z-index: 0; width: 100vw; height: 100vh
 */

import { useCallback, useEffect, useRef, useState } from "react";
import { FpsOverlay } from "../overlays/FpsOverlay";
import { useFrameTelemetry } from "./useFrameTelemetry";
import { useGameLoop } from "./useGameLoop";
import { useInputRouter } from "./useInputRouter";
import type { ViewportLifecycleState } from "./useWasmLoader";
import { useWasmLoader } from "./useWasmLoader";
import { emitTelemetry } from "./viewportTelemetry";
import { WasmErrorBanner } from "./WasmErrorBanner";

interface WasmViewportProps {
  /** Called whenever lifecycle state changes. */
  onLifecycle?: (state: ViewportLifecycleState) => void;
  /** Called when entering degraded state with reason. */
  onError?: (message: string) => void;
  /** Whether an overlay with focus-lock is active (menu/modal). */
  overlayMode?: boolean;
  /** Called when user chooses API fallback from degraded banner. */
  onApiFallback?: () => void;
  /** Show FPS counter debug overlay. */
  showDebug?: boolean;
}

const FIRST_FRAME_TIMEOUT_MS = 5_000;
const MAX_RETRY_ATTEMPTS = 3;

export function WasmViewport({
  onLifecycle,
  onError,
  overlayMode = false,
  onApiFallback,
  showDebug = false,
}: WasmViewportProps) {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const sessionIdRef = useRef<string>(crypto.randomUUID());
  const bootStartRef = useRef<number>(Date.now());
  const [lifecycleState, setLifecycleState] = useState<ViewportLifecycleState>("booting");
  const [errorMessage, setErrorMessage] = useState<string | null>(null);
  const firstFrameTimerRef = useRef<ReturnType<typeof setTimeout> | null>(null);
  const firstFrameDoneRef = useRef(false);

  const transitionTo = useCallback(
    (next: ViewportLifecycleState, reason?: string) => {
      setLifecycleState(next);
      onLifecycle?.(next);

      if (next === "degraded") {
        if (firstFrameTimerRef.current) {
          clearTimeout(firstFrameTimerRef.current);
          firstFrameTimerRef.current = null;
        }
        const msg = reason ?? "viewport degraded";
        setErrorMessage(msg);
        onError?.(msg);
        emitTelemetry({
          kind: "degraded_entry",
          sessionId: sessionIdRef.current,
          payload: { reason: msg, uptime_ms: Date.now() - bootStartRef.current },
        });
      } else if (next === "running") {
        setErrorMessage(null);
      }
    },
    [onLifecycle, onError]
  );

  const handleDegraded = useCallback(
    (reason: string) => transitionTo("degraded", reason),
    [transitionTo]
  );

  const {
    lifecycleState: loaderState,
    engine,
    error: loaderError,
    retryCount,
    retry,
  } = useWasmLoader();

  // Mirror loader state → viewport lifecycle
  useEffect(() => {
    if (loaderState === "ready" && lifecycleState === "booting") {
      transitionTo("ready");
      firstFrameTimerRef.current = setTimeout(() => {
        if (!firstFrameDoneRef.current) {
          transitionTo("degraded", "First frame timeout — engine did not start within 5 s");
        }
      }, FIRST_FRAME_TIMEOUT_MS);
    } else if (loaderState === "degraded") {
      transitionTo("degraded", loaderError ?? "WASM loader failed");
    } else if (loaderState === "recovering") {
      transitionTo("recovering");
    } else if (loaderState === "ready" && lifecycleState === "recovering") {
      transitionTo("ready");
    }
  }, [loaderState, lifecycleState, loaderError, transitionTo]);

  const loopOptions = {
    sessionId: sessionIdRef.current,
    bootStartMs: bootStartRef.current,
    onDegraded: handleDegraded,
  };

  const loop = useGameLoop(engine, loopOptions);

  // Transition to running after first game loop frame
  useEffect(() => {
    if (lifecycleState !== "ready" || !engine) return;
    const tid = setTimeout(() => {
      if (loop.frameCount.current > 0 && !firstFrameDoneRef.current) {
        firstFrameDoneRef.current = true;
        if (firstFrameTimerRef.current) {
          clearTimeout(firstFrameTimerRef.current);
          firstFrameTimerRef.current = null;
        }
        transitionTo("running");
      }
    }, 100);
    return () => clearTimeout(tid);
  });

  useInputRouter(canvasRef, engine, overlayMode);
  useFrameTelemetry(loop, sessionIdRef.current);

  return (
    <>
      <canvas
        ref={canvasRef}
        aria-label="Game viewport"
        data-lifecycle={lifecycleState}
        data-testid="wasm-canvas"
        style={{
          position: "fixed",
          inset: 0,
          zIndex: 0,
          width: "100vw",
          height: "100vh",
          display: "block",
          backgroundColor: "#000",
          opacity: lifecycleState === "degraded" ? 0.3 : 1,
          transition: "opacity 0.3s ease",
        }}
      />

      {showDebug && (
        <div style={{ position: "fixed", inset: 0, zIndex: 40, pointerEvents: "none" }}>
          <FpsOverlay loop={loop} />
        </div>
      )}

      <WasmErrorBanner
        message={errorMessage}
        retryCount={retryCount}
        maxRetries={MAX_RETRY_ATTEMPTS}
        onRetry={retry}
        onApiFallback={onApiFallback}
      />
    </>
  );
}
