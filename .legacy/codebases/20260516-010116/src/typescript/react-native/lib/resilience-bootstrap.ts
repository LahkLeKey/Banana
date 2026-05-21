// Slice 031 -- React Native resilience bootstrap.
//
// Wires the shared `@banana/resilience` queue + history to the
// AsyncStorage adapter. AsyncStorage and NetInfo are loaded via
// `require()` so the @banana/resilience package never imports the
// native packages directly (keeps shared package portable + tsc
// clean inside the shared package). When AsyncStorage is missing
// (e.g., bun:test, web preview), we fall back to the in-memory
// adapter.

import {
  type AsyncStorageLike,
  createAsyncStorageAdapter,
  createInMemoryAdapter,
  createRequestQueue,
  createVerdictHistory,
  DEFAULT_VERDICT_RETRY,
  type RequestQueue,
  type StoredVerdict,
  type VerdictHistory,
} from "@banana/resilience";

export type EnsembleQueuePayload = {
  sample: string;
  submittedAt: number;
};

type NetInfoLike = {
  addEventListener(handler: (state: { isConnected: boolean | null }) => void): () => void;
};

let queueRef: RequestQueue<EnsembleQueuePayload, void> | null = null;
let historyRef: VerdictHistory | null = null;
let netInfoRef: NetInfoLike | null | undefined; // undefined = not yet probed.

function loadAsyncStorage(): AsyncStorageLike | null {
  try {
    // eslint-disable-next-line @typescript-eslint/no-require-imports
    const mod = require("@react-native-async-storage/async-storage") as {
      default?: AsyncStorageLike;
    } & AsyncStorageLike;
    return mod.default ?? mod;
  } catch {
    return null;
  }
}

function loadNetInfo(): NetInfoLike | null {
  try {
    // eslint-disable-next-line @typescript-eslint/no-require-imports
    const mod = require("@react-native-community/netinfo") as {
      default?: NetInfoLike;
    } & NetInfoLike;
    return mod.default ?? mod;
  } catch {
    return null;
  }
}

function ensureSingletons(): {
  queue: RequestQueue<EnsembleQueuePayload, void>;
  history: VerdictHistory;
} {
  if (!queueRef || !historyRef) {
    const asyncStorage = loadAsyncStorage();
    const adapter = asyncStorage
      ? createAsyncStorageAdapter(asyncStorage)
      : createInMemoryAdapter();
    queueRef = createRequestQueue<EnsembleQueuePayload, void>({
      adapter,
      namespace: "rn-ensemble",
    });
    historyRef = createVerdictHistory({
      adapter,
      namespace: "rn-ensemble",
      maxEntries: 25,
    });
  }
  return { queue: queueRef, history: historyRef };
}

export function getEnsembleQueue(): RequestQueue<EnsembleQueuePayload, void> {
  return ensureSingletons().queue;
}

export function getVerdictHistory(): VerdictHistory {
  return ensureSingletons().history;
}

/**
 * Subscribe to NetInfo "online" transitions. Returns an unsubscribe
 * function. When NetInfo is unavailable (e.g., web preview), returns
 * a no-op unsubscriber so callers can wire it unconditionally.
 */
export function onOnline(handler: () => void): () => void {
  if (netInfoRef === undefined) netInfoRef = loadNetInfo();
  if (!netInfoRef) return () => {};
  let prevConnected: boolean | null = null;
  return netInfoRef.addEventListener((state) => {
    const next = state.isConnected;
    if (prevConnected === false && next === true) {
      try {
        handler();
      } catch {
        /* swallow */
      }
    }
    prevConnected = next;
  });
}

export const ENSEMBLE_RETRY_POLICY = DEFAULT_VERDICT_RETRY;
export type RecentVerdict = StoredVerdict;
