// Slice 029 -- @banana/resilience public surface.

export type { AsyncStorageLike } from "./adapters/async-storage";
export { createAsyncStorageAdapter } from "./adapters/async-storage";
export { createIndexedDbAdapter, createInMemoryAdapter } from "./adapters/indexeddb";
export type { CreateVerdictHistoryOptions } from "./history";
export { createVerdictHistory } from "./history";
export type { CreateRequestQueueOptions } from "./queue";
export { createRequestQueue } from "./queue";
export { computeBackoff, shouldRetry } from "./retry";
export type {
  DrainOutcome,
  DrainReport,
  QueuedJob,
  RequestQueue,
  RetryPolicy,
  StorageAdapter,
  StoredVerdict,
  VerdictHistory,
} from "./types";
export { DEFAULT_CHAT_RETRY, DEFAULT_VERDICT_RETRY } from "./types";
