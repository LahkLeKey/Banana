import {describe, expect, it} from 'bun:test';

import {bootstrapDatabaseRuntime, resolveAuthoritativeDatabaseUrl, syncDatabaseUrlAliases,} from './databaseRuntime.ts';

describe('database runtime contract', () => {
  it('prefers NEON_DATABASE_URL as authoritative source', () => {
    const env = {
      NEON_DATABASE_URL: 'postgres://neon',
      DATABASE_URL: 'postgres://db',
      BANANA_PG_CONNECTION: 'postgres://banana',
    } as unknown as NodeJS.ProcessEnv;

    const resolved = resolveAuthoritativeDatabaseUrl(env);
    expect(resolved.url).toBe('postgres://neon');
    expect(resolved.source).toBe('NEON_DATABASE_URL');
  });

  it('syncs all aliases from authoritative url', () => {
    const env = {
      DATABASE_URL: 'postgres://db-only',
    } as unknown as NodeJS.ProcessEnv;

    const result = syncDatabaseUrlAliases(env);

    expect(result.aliasesSynced).toBe(true);
    expect(result.source).toBe('DATABASE_URL');
    expect(env.NEON_DATABASE_URL).toBe('postgres://db-only');
    expect(env.DATABASE_URL).toBe('postgres://db-only');
    expect(env.BANANA_PG_CONNECTION).toBe('postgres://db-only');
  });

  it('returns non-throwing status when bridge is unavailable and strict mode is off',
     () => {
       const env = {
         NEON_DATABASE_URL: 'postgres://neon',
         BANANA_NATIVE_PGBOUNCER_ENABLED: 'true',
         BANANA_NEON_STRICT: 'false',
       } as unknown as NodeJS.ProcessEnv;

       const status = bootstrapDatabaseRuntime({
         env,
         bridgeFactory: () => ({
           isAvailable: () => false,
           configure: () => {
             throw new Error('should not configure when unavailable');
           },
           health: () => ({available: false, enabled: false}),
         }),
       });

       expect(status.authoritativeUrlPresent).toBe(true);
       expect(status.nativePgBouncer.attempted).toBe(true);
       expect(status.nativePgBouncer.available).toBe(false);
       expect(status.nativePgBouncer.configured).toBe(false);
     });
});
