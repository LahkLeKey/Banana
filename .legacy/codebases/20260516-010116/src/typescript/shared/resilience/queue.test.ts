// Slice 029 -- queue contract coverage.
import { describe, expect, mock, test } from "bun:test";

import { createInMemoryAdapter } from "./src/adapters/indexeddb";
import { createRequestQueue } from "./src/queue";
import { DEFAULT_VERDICT_RETRY, type RetryPolicy } from "./src/types";

const FAST_RETRY: RetryPolicy = {
  maxAttempts: 3,
  backoffMs: 1,
  backoffMultiplier: 2,
  maxBackoffMs: 10,
};

function job(key: string, payload: string, retry: RetryPolicy = FAST_RETRY) {
  return { key, payload, retry, enqueuedAt: Date.now() };
}

describe("createRequestQueue", () => {
  test("happy path drains and clears the job", async () => {
    const queue = createRequestQueue<string, string>({ adapter: createInMemoryAdapter() });
    await queue.enqueue(job("verdict:1", "hello"));
    const handler = mock(async (p: string) => `got:${p}`);
    const report = await queue.drain(handler);
    expect(handler).toHaveBeenCalledTimes(1);
    expect(report.outcomes).toEqual([{ key: "verdict:1", status: "ok", result: "got:hello" }]);
    expect(await queue.peek()).toEqual([]);
  });

  test("dedupe by key keeps newest payload", async () => {
    const queue = createRequestQueue<string, string>({ adapter: createInMemoryAdapter() });
    await queue.enqueue(job("verdict:1", "first"));
    await queue.enqueue(job("verdict:1", "second"));
    const peeked = await queue.peek();
    expect(peeked).toHaveLength(1);
    expect(peeked[0].payload).toBe("second");
  });

  test("retry exhaustion drops the job after maxAttempts", async () => {
    const queue = createRequestQueue<string, string>({ adapter: createInMemoryAdapter() });
    await queue.enqueue(job("verdict:1", "x"));
    const failing = mock(async () => {
      throw new Error("boom");
    });
    // Attempt 1
    let report = await queue.drain(failing);
    expect(report.outcomes[0]).toEqual({ key: "verdict:1", status: "retry", nextAttempts: 1 });
    // Attempt 2
    report = await queue.drain(failing);
    expect(report.outcomes[0]).toEqual({ key: "verdict:1", status: "retry", nextAttempts: 2 });
    // Attempt 3 -> exhausted (maxAttempts=3 means we stop after 3 attempts).
    report = await queue.drain(failing);
    expect(report.outcomes[0].status).toBe("failed");
    expect(await queue.peek()).toEqual([]);
    expect(failing).toHaveBeenCalledTimes(3);
  });

  test("clear(key) removes a single job; clear() removes all", async () => {
    const queue = createRequestQueue<string, string>({ adapter: createInMemoryAdapter() });
    await queue.enqueue(job("a", "1"));
    await queue.enqueue(job("b", "2"));
    await queue.clear("a");
    const remaining = await queue.peek();
    expect(remaining.map((j) => j.key)).toEqual(["b"]);
    await queue.clear();
    expect(await queue.peek()).toEqual([]);
  });

  test("default verdict policy is the contract", () => {
    expect(DEFAULT_VERDICT_RETRY).toEqual({
      maxAttempts: 5,
      backoffMs: 500,
      backoffMultiplier: 2,
      maxBackoffMs: 30_000,
    });
  });
});
