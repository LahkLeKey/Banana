import {beforeEach, describe, expect, test} from 'bun:test';

import {bootstrapDatabaseRuntime, ensurePersistentWorldSourceOfRecordReady,} from './databaseRuntime.ts';

function baseEnv(): NodeJS.ProcessEnv {
  return {
    BANANA_NEON_STRICT: 'false',
    BANANA_NATIVE_PGBOUNCER_ENABLED: 'false',
    BANANA_PERSISTENT_WORLD_REQUIRE_SOURCE_OF_RECORD: 'false',
  };
}

describe('database runtime persistent world source-of-record', () => {
  beforeEach(
      () => {
          // no-op; each test uses isolated env objects
      });

  test(
      'throws when persistent world source-of-record is required and missing',
      () => {
        const env = baseEnv();
        env.BANANA_PERSISTENT_WORLD_REQUIRE_SOURCE_OF_RECORD = 'true';

        expect(() => bootstrapDatabaseRuntime({env}))
            .toThrow('persistent_world_source_of_record_unavailable');
      });

  test('passes when authoritative url aliases are synced', () => {
    const env = baseEnv();
    env.BANANA_PERSISTENT_WORLD_REQUIRE_SOURCE_OF_RECORD = 'true';
    env.BANANA_PG_CONNECTION =
        'postgresql://postgres:postgres@localhost:5432/banana';

    const status = bootstrapDatabaseRuntime({env});
    expect(() => ensurePersistentWorldSourceOfRecordReady(status, env))
        .not.toThrow();
  });
});
