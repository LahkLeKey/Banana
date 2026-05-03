import React from "react";
import type { TrainingLane, TrainingRunResult } from "../types";

interface Props {
  lane: TrainingLane;
  runs: TrainingRunResult[];
}

function MiniBar({ value, color }: { value: number; color: string }) {
  return (
    <div className="w-full bg-muted rounded h-2 overflow-hidden">
      <div className={`h-2 rounded ${color}`} style={{ width: `${Math.round(value * 100)}%` }} />
    </div>
  );
}

export function TrainingMetricCard({ lane, runs }: Props) {
  const last3 = runs.slice(-3).reverse();

  return (
    <div className="rounded border p-3 space-y-2">
      <div className="text-xs font-semibold text-muted-foreground uppercase tracking-wide">{lane}</div>
      {last3.map((r) => {
        const acc = r.metrics?.accuracy ?? 0;
        const f1 = r.metrics?.f1 ?? 0;

        return (
          <div key={r.run_id} className="space-y-1">
            <div className="text-xs text-muted-foreground truncate">{r.run_id}</div>
            <div className="flex items-center gap-2 text-xs">
              <span className="w-10 shrink-0 text-muted-foreground">acc</span>
              <MiniBar value={acc} color="bg-primary" />
              <span className="w-10 text-right shrink-0">{(acc * 100).toFixed(0)}%</span>
            </div>
            <div className="flex items-center gap-2 text-xs">
              <span className="w-10 shrink-0 text-muted-foreground">F1</span>
              <MiniBar value={f1} color="bg-green-500" />
              <span className="w-10 text-right shrink-0">{(f1 * 100).toFixed(0)}%</span>
            </div>
          </div>
        );
      })}
      {last3.length === 0 && <div className="text-xs text-muted-foreground">No runs yet</div>}
    </div>
  );
}
