import {afterEach, describe, expect, test} from 'bun:test';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

import {__resetNativeNetcodeServiceForTests, getNativeNetcodeService,} from './nativeNetcode';

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

afterEach(() => {
  __resetNativeNetcodeServiceForTests();
});

describe('native netcode fail-fast bootstrap', () => {
  test('fails fast when native netcode bindings are unavailable', () => {
    const envSnapshot = snapshotEnv([
      'BANANA_NATIVE_PATH',
      'BANANA_NATIVE_SEARCH_ROOTS',
      'BANANA_NETCODE_ADAPTER',
    ]);
    const originalCwd = process.cwd();
    const tempDir = fs.mkdtempSync(path.join(os.tmpdir(), 'banana-netcode-fail-fast-'));

    try {
      process.chdir(tempDir);
      process.env.BANANA_NETCODE_ADAPTER = 'ffi';
      process.env.BANANA_NATIVE_PATH = path.join(tempDir, 'missing-native-library.so');
      process.env.BANANA_NATIVE_SEARCH_ROOTS = tempDir;

      expect(() => getNativeNetcodeService()).toThrow(
          'Unable to load Banana native library for netcode FFI');
    } finally {
      process.chdir(originalCwd);
      restoreEnv(envSnapshot);
      fs.rmSync(tempDir, {recursive: true, force: true});
    }
  });

  test('rejects unsupported netcode adapter modes', () => {
    const envSnapshot = snapshotEnv(['BANANA_NETCODE_ADAPTER']);
    try {
      process.env.BANANA_NETCODE_ADAPTER = 'inmemory';
      expect(() => getNativeNetcodeService()).toThrow(
          'Unsupported BANANA_NETCODE_ADAPTER mode');
    } finally {
      restoreEnv(envSnapshot);
    }
  });
});
