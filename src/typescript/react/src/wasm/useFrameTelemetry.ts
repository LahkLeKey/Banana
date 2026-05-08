/**
 * useFrameTelemetry.ts — Sparse frame telemetry hook.
 *
 * Spec 261 T010: Emits frame_ms to telemetry API every 60 frames.
 *
 * Reads from the game loop refs (lastFrameMs, frameCount) and posts
 * a telemetry event when the frame counter crosses a 60-frame boundary.
 */

import { useEffect } from "react";

import type { GameLoopHandle } from "./useGameLoop";

const EMIT_INTERVAL_FRAMES = 60;
const TELEMETRY_ENDPOINT = "/api/telemetry/frame";

export function useFrameTelemetry(loop: GameLoopHandle): void {
  const { lastFrameMs, frameCount } = loop;

  useEffect(() => {
    let lastEmittedFrame = 0;

    const interval = setInterval(() => {
      const current = frameCount.current;
      if (current - lastEmittedFrame < EMIT_INTERVAL_FRAMES) return;

      lastEmittedFrame = current;
      const frameMsSnapshot = lastFrameMs.current;

      // Fire-and-forget — telemetry is best-effort
      void fetch(TELEMETRY_ENDPOINT, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ frame_ms: frameMsSnapshot, frame: current }),
        keepalive: true,
      }).catch(() => {
        /* telemetry is non-blocking */
      });
    }, 1000);

    return () => clearInterval(interval);
  }, [lastFrameMs, frameCount]);
}
