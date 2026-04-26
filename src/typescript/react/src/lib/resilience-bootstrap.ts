// Slice 029 -- React channel resilience bootstrap.
// Provides singleton queue + history wired to the IndexedDB adapter
// (falls back to in-memory when indexedDB is unavailable, e.g. SSR
// or the bun:test runner without a polyfill).

import {createIndexedDbAdapter, createRequestQueue, createVerdictHistory, DEFAULT_VERDICT_RETRY, type RequestQueue, type StoredVerdict, type VerdictHistory,} from '@banana/resilience';

export type EnsembleQueuePayload = {
  sample: string; submittedAt: number;
};

let queueRef: RequestQueue<EnsembleQueuePayload, void>|null = null;
let historyRef: VerdictHistory|null = null;

function ensureSingletons(): {
  queue: RequestQueue<EnsembleQueuePayload, void>; history: VerdictHistory;
} {
  if (!queueRef || !historyRef) {
    const adapter = createIndexedDbAdapter();
    queueRef = createRequestQueue<EnsembleQueuePayload, void>({
      adapter,
      namespace: 'react-ensemble',
    });
    historyRef = createVerdictHistory({
      adapter,
      namespace: 'react-ensemble',
      maxEntries: 25,
    });
  }
  return {queue: queueRef, history: historyRef};
}

export function getEnsembleQueue(): RequestQueue<EnsembleQueuePayload, void> {
  return ensureSingletons().queue;
}

export function getVerdictHistory(): VerdictHistory {
  return ensureSingletons().history;
}

export const ENSEMBLE_RETRY_POLICY = DEFAULT_VERDICT_RETRY;

export type RecentVerdict = StoredVerdict;
