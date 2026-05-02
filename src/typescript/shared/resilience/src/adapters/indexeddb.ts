// Slice 029 -- IndexedDB-backed StorageAdapter.
// Single object store keyed by string. Used by web (React) and
// desktop (Electron renderer). Falls back to an in-memory map when
// `indexedDB` is undefined (e.g., SSR / test bootstrap before
// happy-dom polyfill); the caller decides whether that fallback is
// acceptable for their context.

import type { StorageAdapter } from "../types";

const DB_NAME = "banana-resilience";
const STORE = "kv";

type IDBLike = typeof globalThis extends { indexedDB: infer T } ? T : unknown;

function getIndexedDb(): IDBFactory | null {
  const candidate = (globalThis as unknown as { indexedDB?: IDBFactory }).indexedDB;
  return candidate ?? null;
}

async function openDb(factory: IDBFactory): Promise<IDBDatabase> {
  return new Promise((resolve, reject) => {
    const req = factory.open(DB_NAME, 1);
    req.onupgradeneeded = () => {
      const db = req.result;
      if (!db.objectStoreNames.contains(STORE)) {
        db.createObjectStore(STORE);
      }
    };
    req.onsuccess = () => resolve(req.result);
    req.onerror = () => reject(req.error);
  });
}

function tx<T>(
  db: IDBDatabase,
  mode: IDBTransactionMode,
  run: (store: IDBObjectStore) => IDBRequest<T>
): Promise<T> {
  return new Promise((resolve, reject) => {
    const transaction = db.transaction(STORE, mode);
    const store = transaction.objectStore(STORE);
    const req = run(store);
    req.onsuccess = () => resolve(req.result);
    req.onerror = () => reject(req.error);
  });
}

class InMemoryAdapter implements StorageAdapter {
  private readonly store = new Map<string, string>();
  async get(key: string) {
    return this.store.has(key) ? this.store.get(key)! : null;
  }
  async set(key: string, value: string) {
    this.store.set(key, value);
  }
  async delete(key: string) {
    this.store.delete(key);
  }
  async keys(prefix: string) {
    const out: string[] = [];
    for (const k of this.store.keys()) if (k.startsWith(prefix)) out.push(k);
    return out;
  }
}

export function createInMemoryAdapter(): StorageAdapter {
  return new InMemoryAdapter();
}

export function createIndexedDbAdapter(): StorageAdapter {
  const factory = getIndexedDb();
  if (factory === null) {
    return new InMemoryAdapter();
  }
  const resolved: IDBFactory = factory;
  let dbPromise: Promise<IDBDatabase> | null = null;
  function db(): Promise<IDBDatabase> {
    if (!dbPromise) dbPromise = openDb(resolved);
    return dbPromise;
  }
  return {
    async get(key) {
      const d = await db();
      const value = await tx<string | undefined>(d, "readonly", (s) => s.get(key));
      return value ?? null;
    },
    async set(key, value) {
      const d = await db();
      await tx<IDBValidKey>(d, "readwrite", (s) => s.put(value, key));
    },
    async delete(key) {
      const d = await db();
      await tx<undefined>(d, "readwrite", (s) => s.delete(key));
    },
    async keys(prefix) {
      const d = await db();
      return new Promise<string[]>((resolve, reject) => {
        const out: string[] = [];
        const req = d.transaction(STORE, "readonly").objectStore(STORE).openKeyCursor();
        req.onsuccess = () => {
          const cursor = req.result;
          if (!cursor) {
            resolve(out);
            return;
          }
          const k = String(cursor.key);
          if (k.startsWith(prefix)) out.push(k);
          cursor.continue();
        };
        req.onerror = () => reject(req.error);
      });
    },
  };
}

// Re-export for type-only consumers (avoids unused-warning on IDBLike).
export type { IDBLike };
