// Slice 031 -- AsyncStorage-backed StorageAdapter for React Native.
//
// Pure factory that takes the AsyncStorage-shaped client as a
// parameter so the adapter has no direct dependency on the
// `@react-native-async-storage/async-storage` package; the RN
// bootstrap injects the real one and tests inject an in-memory
// double using the exact same surface.

import type {StorageAdapter} from '../types';

/**
 * Minimal subset of `@react-native-async-storage/async-storage`'s
 * static API consumed by this adapter. Keeping this narrow makes
 * mocking trivial and avoids depending on the RN package types
 * inside the shared `@banana/resilience` package.
 */
export interface AsyncStorageLike {
  getItem(key: string): Promise<string|null>;
  setItem(key: string, value: string): Promise<void>;
  removeItem(key: string): Promise<void>;
  getAllKeys(): Promise<readonly string[]>;
}

export function createAsyncStorageAdapter(client: AsyncStorageLike):
    StorageAdapter {
  return {
    async get(key) {
      return client.getItem(key);
    },
    async set(key, value) {
      await client.setItem(key, value);
    },
    async delete (key) {
      await client.removeItem(key);
    },
    async keys(prefix) {
      const all = await client.getAllKeys();
      const out: string[] = [];
      for (const k of all) {
        if (typeof k === 'string' && k.startsWith(prefix)) out.push(k);
      }
      return out;
    },
  };
}
