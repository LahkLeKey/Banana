import {describe, expect, test} from 'bun:test';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

import {createDefaultAntiCheatInteropAdapter, InMemoryAntiCheatInteropAdapter,} from './anti-cheat-interop';

type EnvSnapshot = Record<string, string|undefined>;

function snapshotEnv(keys: readonly string[]): EnvSnapshot {
  const snapshot: EnvSnapshot = {};
  for (const key of keys) {
    snapshot[key] = process.env[key];
  }
  return snapshot;
}

function restoreEnv(snapshot: EnvSnapshot): void {
  for (const [key, value] of Object.entries(snapshot)) {
    if (value === undefined) {
      delete process.env[key];
      continue;
    }
    process.env[key] = value;
  }
}

describe('anti-cheat adapter fail-fast bootstrap', () => {
  test(
      'fails fast by default when native anti-cheat bindings are unavailable',
      () => {
        const envSnapshot = snapshotEnv([
          'BANANA_NATIVE_PATH',
          'BANANA_NATIVE_SEARCH_ROOTS',
          'BANANA_ANTICHEAT_ADAPTER',
        ]);
        const originalCwd = process.cwd();

        const cwd = fs.mkdtempSync(
            path.join(os.tmpdir(), 'banana-anti-cheat-fail-fast-'));
        try {
          process.chdir(cwd);
          process.env.BANANA_ANTICHEAT_ADAPTER = 'ffi';
          process.env.BANANA_NATIVE_PATH =
              path.join(cwd, 'missing-native-library.so');
          process.env.BANANA_NATIVE_SEARCH_ROOTS = cwd;

          expect(() => createDefaultAntiCheatInteropAdapter())
              .toThrow(
                  'Unable to load Banana native library for anti-cheat FFI');
        } finally {
          process.chdir(originalCwd);
          restoreEnv(envSnapshot);
          fs.rmSync(cwd, {recursive: true, force: true});
        }
      });

  test('supports explicit in-memory adapter mode when requested', () => {
    const envSnapshot = snapshotEnv(['BANANA_ANTICHEAT_ADAPTER']);
    try {
      process.env.BANANA_ANTICHEAT_ADAPTER = 'inmemory';
      const adapter = createDefaultAntiCheatInteropAdapter();
      expect(adapter).toBeInstanceOf(InMemoryAntiCheatInteropAdapter);
    } finally {
      restoreEnv(envSnapshot);
    }
  });
});
