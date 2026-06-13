import {afterEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';

import {registerWorldRoutes} from './world.ts';

type EnvSnapshot = Record<string, string | undefined>;

const tempDirs: string[] = [];

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

function createTempDirectory(): string {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'banana-world-fallback-'));
  tempDirs.push(dir);
  return dir;
}

afterEach(() => {
  while (tempDirs.length > 0) {
    const dir = tempDirs.pop();
    if (dir && fs.existsSync(dir)) {
      fs.rmSync(dir, {recursive: true, force: true});
    }
  }
});

describe('world route native fallback contract', () => {
  test('falls back to in-memory world service when native loader is unavailable', async () => {
    const envSnapshot = snapshotEnv(['BANANA_NATIVE_PATH', 'BANANA_NATIVE_SEARCH_ROOTS']);
    const originalCwd = process.cwd();

    try {
      const cwd = createTempDirectory();
      process.chdir(cwd);

      // Force candidate resolution away from repository build outputs.
      const missingLibrary = path.join(cwd, 'missing-native-library.so');
      process.env.BANANA_NATIVE_PATH = missingLibrary;
      process.env.BANANA_NATIVE_SEARCH_ROOTS = cwd;

      const app = Fastify({logger: false});
      await registerRequestContextMiddleware(app);
      registerFastifyErrorMapper(app);
      await registerWorldRoutes(app);
      await app.ready();

      const response = await app.inject({
        method: 'GET',
        url: '/api/world/chunk/4/9',
      });

      expect(response.statusCode).toBe(200);
      expect(response.headers['content-type']).toContain('application/octet-stream');
      expect(response.headers['x-chunk-x']).toBe('4');
      expect(response.headers['x-chunk-z']).toBe('9');
      expect(response.body.length).toBeGreaterThan(0);

      await app.close();
    } finally {
      process.chdir(originalCwd);
      restoreEnv(envSnapshot);
    }
  });
});
