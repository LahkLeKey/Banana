/**
 * FpsOverlay.tsx — FPS counter debug overlay (z-index 40).
 *
 * Spec 261 T011: FPS counter debug overlay component.
 * Renders in the Debug slot (z-index 40) and updates from the game loop refs.
 *
 * Reads frame timing from a shared ref — no re-renders triggered by the
 * game loop. Uses a 500ms setInterval to update the displayed value.
 */

import { useEffect, useRef, useState } from "react";

import type { GameLoopHandle } from "../wasm/useGameLoop";

interface FpsOverlayProps {
  loop: GameLoopHandle;
}

export function FpsOverlay({ loop }: FpsOverlayProps) {
  const [fps, setFps] = useState<number>(0);
  const prevFrameCount = useRef<number>(0);

  useEffect(() => {
    const interval = setInterval(() => {
      const current = loop.frameCount.current;
      const delta = current - prevFrameCount.current;
      prevFrameCount.current = current;
      // Two samples per second → multiply by 2
      setFps(delta * 2);
    }, 500);

    return () => clearInterval(interval);
  }, [loop]);

  return (
    <div
      data-overlay="true"
      style={{
        position: "absolute",
        top: "0.5rem",
        right: "0.75rem",
        color: fps < 30 ? "#ef4444" : fps < 50 ? "#f59e0b" : "#22c55e",
        fontFamily: "monospace",
        fontSize: "0.8rem",
        pointerEvents: "none",
        userSelect: "none",
        lineHeight: 1,
      }}
    >
      {fps} fps
    </div>
  );
}
