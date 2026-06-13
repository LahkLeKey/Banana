import {afterEach, describe, expect, test} from 'bun:test';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

import {resolveBananaNativeLibraryCandidates,} from './native-library-candidates';

type EnvState = Record<string, string|undefined>;

const createdDirs: string[] = [];

function snapshotEnv(keys: readonly string[]): EnvState {
  const state: EnvState = {};
  for (const key of keys) {
    state[key] = process.env[key];
  }
  return state;
}

function restoreEnv(state: EnvState): void {
  for (const [key, value] of Object.entries(state)) {
    if (value === undefined) {
      delete process.env[key];
      continue;
    }
    process.env[key] = value;
  }
}

function createTempDir(): string {
  const dir =
      fs.mkdtempSync(path.join(os.tmpdir(), 'banana-native-candidates-'));
  createdDirs.push(dir);
  return dir;
}

function resolveLibraryBasenames(): {libName: string; plainName: string} {
  const sample =
      resolveBananaNativeLibraryCandidates({fallbackDirs: []})
          .map((candidate) => path.basename(candidate))
          .find(
              (basename) => basename.startsWith('libbanana_native.') ||
                  basename.startsWith('banana_native.'));

  if (!sample || sample.lastIndexOf('.') < 0) {
    throw new Error('Unable to determine native library extension for tests');
  }

  const extension = sample.slice(sample.lastIndexOf('.') + 1);
  return {
    libName: `libbanana_native.${extension}`,
    plainName: `banana_native.${extension}`,
  };
}

afterEach(() => {
  while (createdDirs.length > 0) {
    const dir = createdDirs.pop();
    if (dir && fs.existsSync(dir)) {
      fs.rmSync(dir, {recursive: true, force: true});
    }
  }
});

describe('native library candidates', () => {
  test('accepts BANANA_NATIVE_PATH as a direct file path', () => {
    const envState =
        snapshotEnv(['BANANA_NATIVE_PATH', 'BANANA_NATIVE_SEARCH_ROOTS']);
    try {
      const tmpDir = createTempDir();
      const {libName} = resolveLibraryBasenames();
      const directLibraryPath = path.join(tmpDir, libName);
      fs.writeFileSync(directLibraryPath, 'test-lib');

      process.env.BANANA_NATIVE_PATH = directLibraryPath;
      delete process.env.BANANA_NATIVE_SEARCH_ROOTS;

      const candidates =
          resolveBananaNativeLibraryCandidates({fallbackDirs: []});
      expect(candidates[0]).toBe(directLibraryPath);
    } finally {
      restoreEnv(envState);
    }
  });

  test('accepts BANANA_NATIVE_PATH as a directory', () => {
    const envState =
        snapshotEnv(['BANANA_NATIVE_PATH', 'BANANA_NATIVE_SEARCH_ROOTS']);
    try {
      const tmpDir = createTempDir();
      const {libName, plainName} = resolveLibraryBasenames();
      process.env.BANANA_NATIVE_PATH = tmpDir;
      delete process.env.BANANA_NATIVE_SEARCH_ROOTS;

      const candidates =
          resolveBananaNativeLibraryCandidates({fallbackDirs: []});
      expect(candidates).toContain(path.join(tmpDir, libName));
      expect(candidates).toContain(path.join(tmpDir, plainName));
    } finally {
      restoreEnv(envState);
    }
  });

  test('prioritizes existing paths before missing paths', () => {
    const envState =
        snapshotEnv(['BANANA_NATIVE_PATH', 'BANANA_NATIVE_SEARCH_ROOTS']);
    try {
      const tmpDir = createTempDir();
      const {libName, plainName} = resolveLibraryBasenames();
      const existingCandidate = path.join(tmpDir, plainName);
      const missingCandidate = path.join(tmpDir, libName);
      fs.writeFileSync(existingCandidate, 'test-lib');

      process.env.BANANA_NATIVE_PATH = tmpDir;
      delete process.env.BANANA_NATIVE_SEARCH_ROOTS;

      const candidates =
          resolveBananaNativeLibraryCandidates({fallbackDirs: []});
      expect(candidates.indexOf(existingCandidate))
          .toBeLessThan(candidates.indexOf(missingCandidate));
    } finally {
      restoreEnv(envState);
    }
  });

  test('includes BANANA_NATIVE_SEARCH_ROOTS entries', () => {
    const envState =
        snapshotEnv(['BANANA_NATIVE_PATH', 'BANANA_NATIVE_SEARCH_ROOTS']);
    try {
      const tmpDir = createTempDir();
      const {libName, plainName} = resolveLibraryBasenames();
      process.env.BANANA_NATIVE_SEARCH_ROOTS = tmpDir;
      delete process.env.BANANA_NATIVE_PATH;

      const candidates =
          resolveBananaNativeLibraryCandidates({fallbackDirs: []});
      expect(candidates).toContain(path.join(tmpDir, libName));
      expect(candidates).toContain(path.join(tmpDir, plainName));
    } finally {
      restoreEnv(envState);
    }
  });

  test('deduplicates repeated candidate roots', () => {
    const envState =
        snapshotEnv(['BANANA_NATIVE_PATH', 'BANANA_NATIVE_SEARCH_ROOTS']);
    try {
      process.env.BANANA_NATIVE_SEARCH_ROOTS = process.cwd();
      delete process.env.BANANA_NATIVE_PATH;

      const candidates =
          resolveBananaNativeLibraryCandidates({fallbackDirs: []});
      expect(new Set(candidates).size).toBe(candidates.length);
    } finally {
      restoreEnv(envState);
    }
  });
});
