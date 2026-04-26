// Slice 031 -- AsyncStorage adapter contract suite.
// Reuses the shared queue + history flows that already cover the
// IndexedDB adapter; only the storage backend changes (FR-031-01).

import {describe, expect, mock, test} from 'bun:test';

import {type AsyncStorageLike, createAsyncStorageAdapter} from './src/adapters/async-storage';
import {createVerdictHistory} from './src/history';
import {createRequestQueue} from './src/queue';
import type {RetryPolicy, StoredVerdict} from './src/types';

const FAST_RETRY: RetryPolicy = {
  maxAttempts: 3,
  backoffMs: 1,
  backoffMultiplier: 2,
  maxBackoffMs: 10,
};

function makeMemoryAsyncStorage(): AsyncStorageLike {
  const data = new Map<string, string>();
  return {
    async getItem(key) {
      return data.has(key) ? data.get(key)! : null;
    },
    async setItem(key, value) {
      data.set(key, value);
    },
    async removeItem(key) {
      data.delete(key);
    },
    async getAllKeys() {
      return Array.from(data.keys());
    },
  };
}

describe('createAsyncStorageAdapter', () => {
  test('round-trips values and lists keys by prefix', async () => {
    const adapter = createAsyncStorageAdapter(makeMemoryAsyncStorage());
    await adapter.set('banana:queue:default:a', 'A');
    await adapter.set('banana:queue:default:b', 'B');
    await adapter.set('other:x', 'X');
    expect(await adapter.get('banana:queue:default:a')).toBe('A');
    const keys = await adapter.keys('banana:queue:default:');
    expect(keys.sort()).toEqual([
      'banana:queue:default:a', 'banana:queue:default:b'
    ]);
    await adapter.delete('banana:queue:default:a');
    expect(await adapter.get('banana:queue:default:a')).toBeNull();
  });

  test(
      'powers the same RequestQueue contract as IndexedDB adapter',
      async () => {
        const adapter = createAsyncStorageAdapter(makeMemoryAsyncStorage());
        const queue = createRequestQueue<string, string>({adapter});
        await queue.enqueue({
          key: 'verdict:1',
          payload: 'hello',
          retry: FAST_RETRY,
          enqueuedAt: 1
        });
        await queue.enqueue({
          key: 'verdict:1',
          payload: 'newer',
          retry: FAST_RETRY,
          enqueuedAt: 2
        });
        const handler = mock(async (p: string) => `got:${p}`);
        const report = await queue.drain(handler);
        expect(report.outcomes).toEqual([
          {key: 'verdict:1', status: 'ok', result: 'got:newer'}
        ]);
        expect(handler).toHaveBeenCalledTimes(1);
        expect(await queue.peek()).toEqual([]);
      });

  test('powers VerdictHistory ordering + cap', async () => {
    const adapter = createAsyncStorageAdapter(makeMemoryAsyncStorage());
    const history = createVerdictHistory({adapter, maxEntries: 2});
    const v = (id: string, t: number): StoredVerdict =>
        ({id, capturedAt: t, input: id, verdict: {id}, didEscalate: false});
    await history.record(v('a', 1));
    await history.record(v('b', 2));
    await history.record(v('c', 3));
    const ids = (await history.list()).map((x) => x.id);
    expect(ids).toEqual(['c', 'b']);
  });
});
