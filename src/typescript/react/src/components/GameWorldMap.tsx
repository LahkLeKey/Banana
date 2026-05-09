/**
 * GameWorldMap.tsx — 2D top-down minimap of entity positions.
 *
 * Renders entity positions as coloured dots on a square canvas.
 * Receives live position data polled from the WASM module.
 */
import { useEffect, useRef } from "react";

export interface EntitySnapshot {
  idx: number;
  x: number;
  z: number;
  state: number /** 0 = inactive, 1 = active/patrol, 2 = investigate */;
}

interface Props {
  entities: EntitySnapshot[];
  worldSize?: number /** world-space units per side, default 30 */;
  width?: number /** canvas pixels, default 220 */;
  height?: number;
}

const STATE_COLORS: Record<number, string> = {
  0: "#6b7280" /* inactive – gray */,
  1: "#22c55e" /* patrol   – green */,
  2: "#f59e0b" /* investigate – amber */,
  3: "#3b82f6" /* return   – blue */,
};

export function GameWorldMap({ entities, worldSize = 30, width = 220, height = 220 }: Props) {
  const canvasRef = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext("2d");
    if (!ctx) return;

    /* Background */
    ctx.fillStyle = "#0f172a";
    ctx.fillRect(0, 0, width, height);

    /* Grid lines */
    ctx.strokeStyle = "#1e293b";
    ctx.lineWidth = 0.5;
    const cells = 6;
    for (let i = 0; i <= cells; i++) {
      const x = (i / cells) * width;
      const y = (i / cells) * height;
      ctx.beginPath();
      ctx.moveTo(x, 0);
      ctx.lineTo(x, height);
      ctx.stroke();
      ctx.beginPath();
      ctx.moveTo(0, y);
      ctx.lineTo(width, y);
      ctx.stroke();
    }

    /* World border */
    ctx.strokeStyle = "#334155";
    ctx.lineWidth = 1;
    ctx.strokeRect(1, 1, width - 2, height - 2);

    /* Entities */
    for (const e of entities) {
      /* Map world coords (0..worldSize) to canvas (0..width/height) */
      const px = (e.x / worldSize) * width;
      const py = (e.z / worldSize) * height;

      const color = STATE_COLORS[e.state] ?? "#6b7280";

      /* Glow */
      ctx.shadowColor = color;
      ctx.shadowBlur = 8;
      ctx.fillStyle = color;
      ctx.beginPath();
      ctx.arc(px, py, 4, 0, Math.PI * 2);
      ctx.fill();

      /* ID label */
      ctx.shadowBlur = 0;
      ctx.fillStyle = "#e2e8f0";
      ctx.font = "8px monospace";
      ctx.fillText(`${e.idx}`, px + 6, py - 2);
    }

    /* Legend */
    ctx.shadowBlur = 0;
    const legends: [string, string][] = [
      ["patrol", "#22c55e"],
      ["investigate", "#f59e0b"],
      ["return", "#3b82f6"],
    ];
    legends.forEach(([label, color], i) => {
      const lx = 6;
      const ly = height - 10 - i * 13;
      ctx.fillStyle = color;
      ctx.fillRect(lx, ly - 5, 7, 7);
      ctx.fillStyle = "#94a3b8";
      ctx.font = "8px monospace";
      ctx.fillText(label, lx + 10, ly + 1);
    });
  }, [entities, worldSize, width, height]);

  return (
    <canvas
      ref={canvasRef}
      width={width}
      height={height}
      className="rounded border border-border"
      style={{ imageRendering: "pixelated" }}
      aria-label="NPC world map"
    />
  );
}
