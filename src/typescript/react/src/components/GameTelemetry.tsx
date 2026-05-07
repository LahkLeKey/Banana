/**
 * GameTelemetry.tsx — real-time frame-time graph + engine stats panel.
 *
 * Renders a 300-sample rolling frame-time chart and key stats cards.
 */
import { useEffect, useRef } from "react";
import { Card, CardContent } from "./ui/card";

export interface TelemetrySnapshot {
    fps: number;
    frameCount: number;
    entityCount: number;
    frameTimes: number[]; /** last N frame durations in ms */
}

interface Props {
    data: TelemetrySnapshot;
}

const GRAPH_W = 300;
const GRAPH_H = 60;
const TARGET_MS = 16.67; /* 60 fps budget */

export function GameTelemetry({ data }: Props) {
    const graphRef = useRef<HTMLCanvasElement>(null);

    useEffect(() => {
        const canvas = graphRef.current;
        if (!canvas) return;
        const ctx = canvas.getContext("2d");
        if (!ctx) return;

        ctx.fillStyle = "#0f172a";
        ctx.fillRect(0, 0, GRAPH_W, GRAPH_H);

        /* 16.67 ms target line */
        const targetY = GRAPH_H - (TARGET_MS / 50) * GRAPH_H;
        ctx.strokeStyle = "#ef4444";
        ctx.setLineDash([2, 2]);
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.moveTo(0, targetY);
        ctx.lineTo(GRAPH_W, targetY);
        ctx.stroke();
        ctx.setLineDash([]);

        /* Frame time bars */
        const samples = data.frameTimes.slice(-GRAPH_W);
        const barW = GRAPH_W / Math.max(samples.length, 1);
        samples.forEach((ms, i) => {
            const barH = Math.min((ms / 50) * GRAPH_H, GRAPH_H);
            const over = ms > TARGET_MS;
            ctx.fillStyle = over ? "#ef4444" : "#22c55e";
            ctx.fillRect(i * barW, GRAPH_H - barH, barW - 0.5, barH);
        });

        /* Axis label */
        ctx.fillStyle = "#475569";
        ctx.font = "8px monospace";
        ctx.fillText("50ms", 2, 10);
        ctx.fillText("0ms", 2, GRAPH_H - 2);

    }, [data.frameTimes]);

    const avg =
        data.frameTimes.length > 0
            ? data.frameTimes.reduce((a, b) => a + b, 0) / data.frameTimes.length
            : 0;

    const over = avg > TARGET_MS;

    return (
        <div className="space-y-3">
            {/* Frame time chart */}
            <div>
                <p className="text-xs text-muted-foreground mb-1">Frame time (last {Math.min(data.frameTimes.length, GRAPH_W)} frames)</p>
                <canvas
                    ref={graphRef}
                    width={GRAPH_W}
                    height={GRAPH_H}
                    className="w-full rounded border border-border"
                    style={{ imageRendering: "pixelated" }}
                />
                <p className={`text-xs mt-1 ${over ? "text-red-500" : "text-green-600"}`}>
                    avg {avg.toFixed(2)} ms · target ≤ 16.67 ms (60 fps)
                </p>
            </div>

            {/* Stats grid */}
            <div className="grid grid-cols-3 gap-2">
                <Card>
                    <CardContent className="pt-3 pb-3">
                        <p className="text-xs text-muted-foreground">FPS</p>
                        <p className={`text-lg font-semibold tabular-nums ${data.fps < 55 ? "text-red-500" : "text-green-600"}`}>
                            {data.fps}
                        </p>
                    </CardContent>
                </Card>
                <Card>
                    <CardContent className="pt-3 pb-3">
                        <p className="text-xs text-muted-foreground">Frames</p>
                        <p className="text-lg font-semibold tabular-nums">{data.frameCount.toLocaleString()}</p>
                    </CardContent>
                </Card>
                <Card>
                    <CardContent className="pt-3 pb-3">
                        <p className="text-xs text-muted-foreground">Entities</p>
                        <p className="text-lg font-semibold tabular-nums">{data.entityCount}</p>
                    </CardContent>
                </Card>
            </div>
        </div>
    );
}
