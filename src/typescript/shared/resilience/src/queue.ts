// Slice 029 -- RequestQueue implementation.
// - Persistent (StorageAdapter-backed).
// - Dedupes by `key`: enqueueing a job with an existing key replaces
//   the prior payload (newer payload wins; matches "user retypes
//   while offline" UX).
// - Single-pass drain: caller invokes drain() (typically on online
//   event); each job is attempted once. Failures bump `attempts`
//   and remain queued unless the policy is exhausted (then dropped
//   with a `failed` outcome).

import {shouldRetry} from './retry';
import type {DrainOutcome, DrainReport, QueuedJob, RequestQueue, StorageAdapter} from './types';

const PREFIX = 'banana:queue:';

export type CreateRequestQueueOptions = {
  adapter: StorageAdapter;
  /**
     Storage namespace (defaults to "default"). Allows multiple queues per app.
   */
  namespace?: string;
};

export function createRequestQueue<TPayload, TResult>(
    options: CreateRequestQueueOptions,
    ): RequestQueue<TPayload, TResult> {
  const ns = options.namespace ?? 'default';
  const prefix = `${PREFIX}${ns}:`;

  const storageKey = (jobKey: string) =>
      `${prefix}${encodeURIComponent(jobKey)}`;

  async function readAll(): Promise<QueuedJob<TPayload>[]> {
    const keys = await options.adapter.keys(prefix);
    const jobs: QueuedJob<TPayload>[] = [];
    for (const k of keys) {
      const raw = await options.adapter.get(k);
      if (!raw) continue;
      try {
        jobs.push(JSON.parse(raw) as QueuedJob<TPayload>);
      } catch {
        // Skip corrupt entries silently; they'll be cleared on next clear().
      }
    }
    // Stable order: older first.
    jobs.sort((a, b) => a.enqueuedAt - b.enqueuedAt);
    return jobs;
  }

  return {
    async enqueue(job) {
      // Dedupe: same key overwrites prior payload.
      await options.adapter.set(
          storageKey(job.key),
          JSON.stringify({...job, attempts: job.attempts ?? 0}));
    },
    async peek() {
      return readAll();
    },
    async clear(key) {
      if (key !== undefined) {
        await options.adapter.delete(storageKey(key));
        return;
      }
      const keys = await options.adapter.keys(prefix);
      for (const k of keys) {
        await options.adapter.delete(k);
      }
    },
    async drain(handler): Promise<DrainReport<TResult>> {
      const jobs = await readAll();
      const outcomes: DrainOutcome<TResult>[] = [];
      for (const job of jobs) {
        const attempts = (job.attempts ?? 0) + 1;
        try {
          const result = await handler(job.payload);
          await options.adapter.delete(storageKey(job.key));
          outcomes.push({key: job.key, status: 'ok', result});
        } catch (err) {
          if (shouldRetry(job.retry, attempts)) {
            await options.adapter.set(
                storageKey(job.key),
                JSON.stringify({...job, attempts}),
            );
            outcomes.push(
                {key: job.key, status: 'retry', nextAttempts: attempts});
          } else {
            await options.adapter.delete(storageKey(job.key));
            outcomes.push({
              key: job.key,
              status: 'failed',
              error: err instanceof Error ? err.message : String(err),
            });
          }
        }
      }
      return {outcomes};
    },
  };
}
