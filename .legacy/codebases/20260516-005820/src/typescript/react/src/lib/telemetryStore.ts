import { useSyncExternalStore } from "react";

export type TelemetrySource = "runtime" | "api" | "frontend" | "wasm-worker" | "native";
export type TelemetryStatus = "ok" | "error" | "fallback" | "info";
export type NativeLayer = "wrapper" | "core" | "dal";

export interface TelemetryEvent {
  source: TelemetrySource;
  event: string;
  timestamp: number;
  status: TelemetryStatus;
  durationMs?: number;
  code?: number;
  channel?: string;
  variant?: "simd" | "scalar";
  layer?: NativeLayer;
  details?: Record<string, string | number | boolean | null>;
}

interface TelemetrySnapshot {
  events: TelemetryEvent[];
  updatedAt: number;
}

const MAX_EVENTS = 800;
const _events: TelemetryEvent[] = [];
const _listeners = new Set<() => void>();
let _snapshot: TelemetrySnapshot = {
  events: [],
  updatedAt: Date.now(),
};

function emitStoreChange(): void {
  _snapshot = {
    events: _events.slice(),
    updatedAt: Date.now(),
  };
  for (const listener of _listeners) {
    listener();
  }
}

function normalizeStatus(event: string, explicit?: string): TelemetryStatus {
  if (explicit === "ok" || explicit === "error" || explicit === "fallback" || explicit === "info") {
    return explicit;
  }
  if (event.includes("failed") || event.includes("error")) return "error";
  if (event.includes("fallback")) return "fallback";
  if (event.includes("ready") || event.includes("completed")) return "ok";
  return "info";
}

export function recordTelemetryEvent(event: TelemetryEvent): void {
  _events.push(event);
  if (_events.length > MAX_EVENTS) {
    _events.splice(0, _events.length - MAX_EVENTS);
  }
  emitStoreChange();
}

export function recordWasmWorkerTelemetry(payload: Record<string, unknown>): void {
  const event = typeof payload.event === "string" ? payload.event : "wasm.worker.unknown";
  const timestamp = typeof payload.timestamp === "number" ? payload.timestamp : Date.now();
  const durationMs =
    typeof payload.duration_ms === "number"
      ? payload.duration_ms
      : typeof payload.init_ms === "number"
        ? payload.init_ms
        : undefined;

  recordTelemetryEvent({
    source: "wasm-worker",
    event,
    timestamp,
    status: normalizeStatus(event),
    durationMs,
    code: typeof payload.code === "number" ? payload.code : undefined,
    channel: typeof payload.channel === "string" ? payload.channel : undefined,
    variant:
      payload.variant === "simd" || payload.variant === "scalar" ? payload.variant : undefined,
    details: {
      fn: typeof payload.fn === "string" ? payload.fn : null,
      reason: typeof payload.reason === "string" ? payload.reason : null,
      id: typeof payload.id === "string" ? payload.id : null,
    },
  });
}

export function recordNativeTelemetryEvent(event: Omit<TelemetryEvent, "source">): void {
  recordTelemetryEvent({
    ...event,
    source: "native",
  });
}

export function subscribeTelemetryStore(listener: () => void): () => void {
  _listeners.add(listener);
  return () => {
    _listeners.delete(listener);
  };
}

export function getTelemetrySnapshot(): TelemetrySnapshot {
  return _snapshot;
}

export function useTelemetrySnapshot(): TelemetrySnapshot {
  return useSyncExternalStore(subscribeTelemetryStore, getTelemetrySnapshot, getTelemetrySnapshot);
}

export function resetTelemetryStore(): void {
  _events.splice(0, _events.length);
  emitStoreChange();
}
