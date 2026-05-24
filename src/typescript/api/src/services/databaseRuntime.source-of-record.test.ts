import {describe, expect, it} from 'bun:test';

import {bootstrapDatabaseRuntime} from './databaseRuntime.ts';

describe('database source-of-record enforcement', () => {
  it('throws on conflicting alias values', () => {
    expect(() => bootstrapDatabaseRuntime({
             env: {
               NEON_DATABASE_URL: 'postgres://one',
               DATABASE_URL: 'postgres://two',
             } as unknown as NodeJS.ProcessEnv,
           }))
        .toThrow(
            'Conflicting source-of-record aliases detected: NEON_DATABASE_URL, DATABASE_URL, and BANANA_PG_CONNECTION must match.');
  });

  it('throws in strict mode when no authoritative url is configured', () => {
    expect(() => bootstrapDatabaseRuntime({
             env: {
               BANANA_NEON_STRICT: 'true',
             } as unknown as NodeJS.ProcessEnv,
           }))
        .toThrow(
            'No authoritative database URL found while BANANA_NEON_STRICT=true.');
  });
});
