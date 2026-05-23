import {describe, expect, it} from 'bun:test';

import {bootstrapDatabaseRuntime, resolveAuthoritativeDatabaseUrl} from './databaseRuntime.ts';

type PgClientLike = {
  connect: () => Promise<void>;
  query: (sql: string) => Promise<{rows: Array<Record<string, unknown>>}>;
  end: () => Promise<void>;
};

type PgModuleLike = {
  Client: new (config: Record<string, unknown>) => PgClientLike;
};

function parsePositiveInt(value: string|undefined, fallback: number): number {
  const parsed = Number(value);
  if (!Number.isFinite(parsed) || parsed <= 0) {
    return fallback;
  }
  return Math.floor(parsed);
}

function delay(ms: number): Promise<void> {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}

async function withRetry<T>(
    label: string,
    attempts: number,
    delayMs: number,
    operation: () => Promise<T>,
    ): Promise<T> {
  let lastError: unknown = null;

  for (let attempt = 1; attempt <= attempts; attempt += 1) {
    try {
      return await operation();
    } catch (error) {
      lastError = error;
      if (attempt < attempts) {
        await delay(delayMs * attempt);
      }
    }
  }

  throw new Error(
      `${label} failed after ${attempts} attempts: ${String(lastError)}`);
}

function shouldDisableSsl(databaseUrl: string): boolean {
  return databaseUrl.includes('localhost') || databaseUrl.includes('127.0.0.1');
}

describe('database runtime native Neon integration', () => {
  it('bootstraps native bridge and executes a Neon SQL probe', async () => {
    const resolved = resolveAuthoritativeDatabaseUrl(process.env);
    if (!resolved.url) {
      throw new Error(
          'NEON_DATABASE_URL (or DATABASE_URL / BANANA_PG_CONNECTION) is required for Neon integration tests.');
    }

    process.env.BANANA_NEON_STRICT = 'true';
    process.env.BANANA_NATIVE_PGBOUNCER_ENABLED = 'true';

    const connectTimeoutMs =
        parsePositiveInt(process.env.BANANA_NEON_CONNECT_TIMEOUT_MS, 8_000);
    const attempts =
        parsePositiveInt(process.env.BANANA_NEON_TEST_RETRY_ATTEMPTS, 3);
    const retryDelayMs =
        parsePositiveInt(process.env.BANANA_NEON_TEST_RETRY_DELAY_MS, 1_500);

    const status = bootstrapDatabaseRuntime({env: process.env});
    expect(status.authoritativeUrlPresent).toBe(true);
    expect(status.aliasesSynced).toBe(true);
    expect(status.nativePgBouncer.attempted).toBe(true);
    expect(status.nativePgBouncer.available).toBe(true);
    expect(status.nativePgBouncer.configured).toBe(true);

    const pg = (await import('pg')) as unknown as PgModuleLike;
    const client = new pg.Client({
      connectionString: resolved.url,
      ssl: shouldDisableSsl(resolved.url) ? false : {rejectUnauthorized: false},
      connectionTimeoutMillis: connectTimeoutMs,
    });

    await withRetry('Neon connect', attempts, retryDelayMs, async () => {
      await client.connect();
    });

    try {
      const result = await withRetry(
          'Neon SQL probe', attempts, retryDelayMs, async () => {
            return await client.query('SELECT 1::int AS ok');
          });
      expect(result.rows.length).toBeGreaterThan(0);
      expect(Number(result.rows[0]?.ok)).toBe(1);
    } finally {
      await client.end();
    }
  });
});
