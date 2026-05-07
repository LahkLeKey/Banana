# 022 -- Storage Contract

Public surface of the shared resilience primitive
(`src/typescript/shared/resilience/`) and the per-channel adapter
interface.

## Module layout (planned)

```
src/typescript/shared/resilience/
  src/
    index.ts              // public exports
    queue.ts              // RequestQueue
    history.ts            // VerdictHistory
    retry.ts              // RetryPolicy
    types.ts              // shared types
  package.json            // @banana/resilience
```

## Public types (TypeScript)

```ts
export type RetryPolicy = {
    maxAttempts: number;        // 1..10
    backoffMs: number;           // initial backoff
    backoffMultiplier: number;   // e.g. 2.0
    maxBackoffMs: number;        // e.g. 30000
};

export type QueuedJob<TPayload, TResult> = {
    key: string;                 // dedupe key
    payload: TPayload;
    retry: RetryPolicy;
    enqueuedAt: number;
};

export interface RequestQueue<TPayload, TResult> {
    enqueue(job: QueuedJob<TPayload, TResult>): Promise<void>;
    drain(handler: (payload: TPayload) => Promise<TResult>): Promise<void>;
    peek(): Promise<QueuedJob<TPayload, TResult>[]>;
    clear(key?: string): Promise<void>;
}

export type StoredVerdict = {
    id: string;                  // uuid
    capturedAt: number;
    input: string;
    verdict: unknown;            // shape: same as API response
    didEscalate: boolean;
};

export interface VerdictHistory {
    record(verdict: StoredVerdict): Promise<void>;
    list(limit?: number): Promise<StoredVerdict[]>;
    clear(): Promise<void>;
}
```

## StorageAdapter interface (per-channel implementation)

```ts
export interface StorageAdapter {
    get(key: string): Promise<string | null>;
    set(key: string, value: string): Promise<void>;
    delete(key: string): Promise<void>;
    keys(prefix: string): Promise<string[]>;
}
```

Per-channel implementations:

- **Web / Electron**: `IndexedDbAdapter` (single object store keyed
  by string).
- **React Native**: `AsyncStorageAdapter` wrapping
  `@react-native-async-storage/async-storage`.

## Lifecycle

1. Channel boots -> creates `StorageAdapter` -> hands it to
   `RequestQueue` and `VerdictHistory` factories.
2. Channel registers a `drain` handler that calls the API and
   persists the verdict via `VerdictHistory.record`.
3. Channel listens for "online" events (web/desktop:
   `window.addEventListener('online', ...)`; RN: NetInfo) and
   triggers `drain`.
4. UI subscribes to `VerdictHistory.list()` for the last-N display.

## Retry policy defaults

- Verdict requests: `maxAttempts=5`, `backoffMs=500`, `multiplier=2`,
  `maxBackoffMs=30000`.
- Chat messages: `maxAttempts=3` (chat is best-effort; long retries
  feel wrong).

## Out of contract

- Cross-device sync (deferred -- requires a server-side store).
- Conflict resolution (deferred -- depends on cross-device).
- Encryption at rest (deferred with trigger; relevant if PII enters
  the verdict payload).
- Background sync on RN when app is killed (RN background-fetch is
  best-effort; explicit deferred item).
