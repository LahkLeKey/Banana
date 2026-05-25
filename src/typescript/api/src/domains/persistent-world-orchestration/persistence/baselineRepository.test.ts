import {beforeEach, describe, expect, test} from 'bun:test';

import {createBaselineRepository, resetBaselineRepositoryForTests} from './baselineRepository.ts';

describe('baseline repository', () => {
  beforeEach(() => {
    resetBaselineRepositoryForTests();
  });

  test('upsert stores and loads baseline by area and baseline id', () => {
    const repository = createBaselineRepository();
    const saved = repository.upsert({
      baselineId: '550e8400-e29b-41d4-a716-446655440000',
      areaIdentity: {
        worldId: '550e8400-e29b-41d4-a716-446655440000',
        laneId: 'lane-a',
        chunkX: 1,
        chunkY: 2,
        partitionEpoch: 3,
        identityHash: 'abcd1234abcd1234',
        areaId: 'area:1:2',
      },
      areaStateVersion: {
        areaId: 'area:1:2',
        areaStateVersion: 4,
        appliedDeltaThroughSequence: 7,
        canonicalStateSignature: 'sig-1',
      },
      baselinePayload: {terrain: 'payload'},
      updatedAt: new Date(0).toISOString(),
    });

    expect(repository.getByAreaId('area:1:2')?.baselineId)
        .toBe(saved.baselineId);
    expect(repository.getByBaselineId(saved.baselineId)
               ?.areaStateVersion.areaStateVersion)
        .toBe(4);
  });
});
