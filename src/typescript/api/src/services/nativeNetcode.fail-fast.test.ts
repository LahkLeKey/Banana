import {afterEach, describe, expect, test} from 'bun:test';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

import {__decodeK3h4BufferForTests, __resetNativeNetcodeServiceForTests, getNativeNetcodeService,} from './nativeNetcode';

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
    const tempDir =
        fs.mkdtempSync(path.join(os.tmpdir(), 'banana-netcode-fail-fast-'));

    try {
      process.chdir(tempDir);
      process.env.BANANA_NETCODE_ADAPTER = 'ffi';
      process.env.BANANA_NATIVE_PATH =
          path.join(tempDir, 'missing-native-library.so');
      process.env.BANANA_NATIVE_SEARCH_ROOTS = tempDir;

      expect(() => getNativeNetcodeService())
          .toThrow('Unable to load Banana native library for netcode FFI');
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
      expect(() => getNativeNetcodeService())
          .toThrow('Unsupported BANANA_NETCODE_ADAPTER mode');
    } finally {
      restoreEnv(envSnapshot);
    }
  });

  test('fails fast on unsupported k3h4 envelope contract version', () => {
    const buffer = Buffer.alloc(892);
    buffer.writeInt32LE(2, 872);
    buffer.writeInt32LE(0x01020304, 876);
    buffer.writeInt32LE(872, 880);
    buffer.writeInt32LE(0, 884);
    buffer.writeInt32LE(0, 888);

    expect(() => __decodeK3h4BufferForTests(buffer))
        .toThrow('Unsupported native k3h4 contract version');
  });

  test('fails fast on k3h4 payload CRC mismatch', () => {
    const buffer = Buffer.alloc(892);
    buffer.writeInt32LE(1, 872);
    buffer.writeInt32LE(0x01020304, 876);
    buffer.writeInt32LE(872, 880);
    buffer.writeInt32LE(1234, 884);
    buffer.writeInt32LE(0, 888);

    expect(() => __decodeK3h4BufferForTests(buffer))
        .toThrow('Native k3h4 payload CRC mismatch');
  });

  test('fails fast on truncated k3h4 payload buffers', () => {
    const buffer = Buffer.alloc(120);
    expect(() => __decodeK3h4BufferForTests(buffer))
        .toThrow('returned truncated payload');
  });
});
