import {afterEach, describe, expect, test} from 'bun:test';

import {__resetTrainingEconomyStoreForTests, getTrainingEconomyStore,} from './trainingEconomyStore.ts';

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
  __resetTrainingEconomyStoreForTests();
});

describe('training economy store fail-fast runtime contract', () => {
  test(
      'throws when default postgres mode has no configured database url',
      async () => {
        const envSnapshot = snapshotEnv([
          'BANANA_TRAINING_STORE_MODE',
          'BANANA_PG_CONNECTION',
          'DATABASE_URL',
          'NEON_DATABASE_URL',
          'BANANA_NEON_STRICT',
        ]);

        try {
          delete process.env.BANANA_TRAINING_STORE_MODE;
          delete process.env.BANANA_PG_CONNECTION;
          delete process.env.DATABASE_URL;
          delete process.env.NEON_DATABASE_URL;
          process.env.BANANA_NEON_STRICT = 'false';

          await expect(getTrainingEconomyStore())
              .rejects.toThrow(
                  'No authoritative database URL configured for training store.');
        } finally {
          restoreEnv(envSnapshot);
        }
      });

  test(
      'supports explicit in-memory mode for controlled environments',
      async () => {
        const envSnapshot = snapshotEnv([
          'BANANA_TRAINING_STORE_MODE',
          'BANANA_PG_CONNECTION',
          'DATABASE_URL',
          'NEON_DATABASE_URL',
        ]);

        try {
          process.env.BANANA_TRAINING_STORE_MODE = 'inmemory';
          delete process.env.BANANA_PG_CONNECTION;
          delete process.env.DATABASE_URL;
          delete process.env.NEON_DATABASE_URL;

          const store = await getTrainingEconomyStore();
          const jobs = await store.scaffoldJobs('player-1', [{
                                                  title: 'controller-drill',
                                                  sector: 'controller-training',
                                                  rewardXp: 120,
                                                }]);

          expect(jobs).toHaveLength(1);
          expect(jobs[0]?.rewardXp).toBe(120);
        } finally {
          restoreEnv(envSnapshot);
        }
      });

  test('rejects unknown training store mode', async () => {
    const envSnapshot = snapshotEnv(['BANANA_TRAINING_STORE_MODE']);

    try {
      process.env.BANANA_TRAINING_STORE_MODE = 'memory-fallback';
      await expect(getTrainingEconomyStore())
          .rejects.toThrow('Unsupported BANANA_TRAINING_STORE_MODE');
    } finally {
      restoreEnv(envSnapshot);
    }
  });
});
