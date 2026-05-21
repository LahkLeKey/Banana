import crypto from "node:crypto";

const WINDOW_MAX = 1000;

interface DriftEntry {
  fingerprint: string;
  score: number;
  ts: number;
}

const driftWindow: DriftEntry[] = [];

export function getDriftStats(): {
  sampleCount: number;
  meanScore: number;
  stdDev: number;
  psi: number | null;
} {
  const n = driftWindow.length;
  if (n === 0) {
    return { sampleCount: 0, meanScore: 0, stdDev: 0, psi: null };
  }
  const mean = driftWindow.reduce((s, e) => s + e.score, 0) / n;
  const variance = driftWindow.reduce((s, e) => s + (e.score - mean) ** 2, 0) / n;
  const stdDev = Math.sqrt(variance);
  // PSI placeholder — requires baseline distribution; returns null until baseline is collected.
  return { sampleCount: n, meanScore: mean, stdDev, psi: null };
}

export function recordDriftSample(text: string, score: number): void {
  const hash = crypto.createHash("sha256").update(text).digest("hex");
  const fingerprint = hash.slice(0, 8);
  if (driftWindow.length >= WINDOW_MAX) {
    driftWindow.shift();
  }
  driftWindow.push({ fingerprint, score, ts: Date.now() });
}

export function shouldSample(
  sampleRate = Number(process.env.BANANA_DRIFT_SAMPLE_RATE ?? "0.1")
): boolean {
  return Math.random() < sampleRate;
}
