// Slice 029 -- VerdictHistory implementation.
// Persists last-N verdicts via StorageAdapter; list() returns
// newest-first. Default cap is 50 (config via options).

import type {StorageAdapter, StoredVerdict, VerdictHistory} from './types';

const PREFIX = 'banana:history:';

export type CreateVerdictHistoryOptions = {
  adapter: StorageAdapter;
  namespace?: string;
  maxEntries?: number;
};

export function createVerdictHistory(
    options: CreateVerdictHistoryOptions,
    ): VerdictHistory {
  const ns = options.namespace ?? 'default';
  const prefix = `${PREFIX}${ns}:`;
  const cap = options.maxEntries ?? 50;
  const keyOf = (id: string) => `${prefix}${id}`;

  async function readAll(): Promise<StoredVerdict[]> {
    const keys = await options.adapter.keys(prefix);
    const out: StoredVerdict[] = [];
    for (const k of keys) {
      const raw = await options.adapter.get(k);
      if (!raw) continue;
      try {
        out.push(JSON.parse(raw) as StoredVerdict);
      } catch {
        // skip corrupt
      }
    }
    out.sort((a, b) => b.capturedAt - a.capturedAt);
    return out;
  }

  return {
    async record(verdict) {
      await options.adapter.set(keyOf(verdict.id), JSON.stringify(verdict));
      const all = await readAll();
      if (all.length > cap) {
        const stale = all.slice(cap);
        for (const v of stale) await options.adapter.delete(keyOf(v.id));
      }
    },
    async list(limit) {
      const all = await readAll();
      return typeof limit === 'number' ? all.slice(0, limit) : all;
    },
    async clear() {
      const keys = await options.adapter.keys(prefix);
      for (const k of keys) await options.adapter.delete(k);
    },
  };
}
