// Slice 029 -- retry helpers (pure; no timers).
import type { RetryPolicy } from "./types";

/**
 * Compute the backoff delay (ms) for a given attempt number (1-based).
 * Capped at `maxBackoffMs`.
 */
export function computeBackoff(policy: RetryPolicy, attempt: number): number {
  if (attempt <= 1) return policy.backoffMs;
  const raw = policy.backoffMs * policy.backoffMultiplier ** (attempt - 1);
  return Math.min(raw, policy.maxBackoffMs);
}

export function shouldRetry(policy: RetryPolicy, attempts: number): boolean {
  return attempts < policy.maxAttempts;
}
