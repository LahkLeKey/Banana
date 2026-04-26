// Slice 029 -- VerdictHistory contract coverage.
import {describe, expect, test} from 'bun:test';

import {createInMemoryAdapter} from './src/adapters/indexeddb';
import {createVerdictHistory} from './src/history';
import type {StoredVerdict} from './src/types';

function v(id: string, capturedAt: number, didEscalate = false): StoredVerdict {
  return {id, capturedAt, input: `in-${id}`, verdict: {id}, didEscalate};
}

describe('createVerdictHistory', () => {
  test('records and lists newest-first', async () => {
    const history = createVerdictHistory({adapter: createInMemoryAdapter()});
    await history.record(v('a', 1));
    await history.record(v('b', 3));
    await history.record(v('c', 2));
    const all = await history.list();
    expect(all.map((x) => x.id)).toEqual(['b', 'c', 'a']);
  });

  test('list(limit) caps the returned slice', async () => {
    const history = createVerdictHistory({adapter: createInMemoryAdapter()});
    for (let i = 0; i < 10; i += 1) await history.record(v(`v${i}`, i));
    expect(await history.list(3)).toHaveLength(3);
  });

  test('maxEntries trims the oldest', async () => {
    const history =
        createVerdictHistory({adapter: createInMemoryAdapter(), maxEntries: 2});
    await history.record(v('a', 1));
    await history.record(v('b', 2));
    await history.record(v('c', 3));
    const ids = (await history.list()).map((x) => x.id);
    expect(ids).toEqual(['c', 'b']);
  });

  test('clear removes everything', async () => {
    const history = createVerdictHistory({adapter: createInMemoryAdapter()});
    await history.record(v('a', 1));
    await history.clear();
    expect(await history.list()).toEqual([]);
  });
});
