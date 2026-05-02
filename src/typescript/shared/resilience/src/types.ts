// Slice 029 -- @banana/resilience public types.
// Pinned by
// .specify/specs/022-frontend-resilient-offline-spike/analysis/storage-contract.md

export type RetryPolicy = {
  maxAttempts: number;
  backoffMs: number;
  backoffMultiplier: number;
  maxBackoffMs: number;
};

export const DEFAULT_VERDICT_RETRY: RetryPolicy = {
  maxAttempts: 5,
  backoffMs: 500,
  backoffMultiplier: 2,
  maxBackoffMs: 30_000,
};

export const DEFAULT_CHAT_RETRY: RetryPolicy = {
  maxAttempts: 3,
  backoffMs: 500,
  backoffMultiplier: 2,
  maxBackoffMs: 10_000,
};

export type QueuedJob<TPayload> = {
  key: string;
  payload: TPayload;
  retry: RetryPolicy;
  enqueuedAt: number;
  attempts?: number;
};

export interface RequestQueue<TPayload, TResult> {
  enqueue(job: QueuedJob<TPayload>): Promise<void>;
  drain(handler: (payload: TPayload) => Promise<TResult>): Promise<DrainReport<TResult>>;
  peek(): Promise<QueuedJob<TPayload>[]>;
  clear(key?: string): Promise<void>;
}

export type DrainOutcome<TResult> =
  | {
      key: string;
      status: "ok";
      result: TResult;
    }
  | {
      key: string;
      status: "failed";
      error: string;
    }
  | {
      key: string;
      status: "retry";
      nextAttempts: number;
    };

export type DrainReport<TResult> = {
  outcomes: DrainOutcome<TResult>[];
};

export type StoredVerdict = {
  id: string;
  capturedAt: number;
  input: string;
  verdict: unknown;
  didEscalate: boolean;
};

export interface VerdictHistory {
  record(verdict: StoredVerdict): Promise<void>;
  list(limit?: number): Promise<StoredVerdict[]>;
  clear(): Promise<void>;
}

export interface StorageAdapter {
  get(key: string): Promise<string | null>;
  set(key: string, value: string): Promise<void>;
  delete(key: string): Promise<void>;
  keys(prefix: string): Promise<string[]>;
}
