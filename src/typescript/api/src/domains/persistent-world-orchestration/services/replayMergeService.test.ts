import {beforeEach, describe, expect, test} from 'bun:test';

import {resetBaselineRepositoryForTests} from '../persistence/baselineRepository.ts';
import {resetDeltaRepositoryForTests} from '../persistence/deltaRepository.ts';

import {resetAreaStateVersionServiceForTests} from './areaStateVersionService.ts';
import {createReplayMergeService} from './replayMergeService.ts';

describe('replay merge service', () => {
  beforeEach(() => {
    resetBaselineRepositoryForTests();
    resetDeltaRepositoryForTests();
    resetAreaStateVersionServiceForTests();
  });

  test(
      'applies deterministic ordered deltas and replays canonical signature',
      () => {
        const service = createReplayMergeService();

        const areaIdentity = {
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-a',
          chunkX: 4,
          chunkY: 2,
          partitionEpoch: 1,
          identityHash: 'abcdabcdabcdabcd',
          areaId: 'area-4-2',
        };

        service.ensureAuthoritativeBaseline({
          areaIdentity,
          stateVersion: {
            areaId: areaIdentity.areaId,
            areaStateVersion: 0,
            appliedDeltaThroughSequence: 0,
            canonicalStateSignature: 'sig0',
          },
          baselineId: '550e8400-e29b-41d4-a716-446655440000',
          orchestrationPathId: 'baseline-plus-delta-replay-v1',
        });

        const applied = service.applyDelta({
          areaIdentity,
          baselineId: '550e8400-e29b-41d4-a716-446655440000',
          baseAreaStateVersion: 0,
          idempotencyKey: 'k1',
          deltaPayload: {op: 'place', item: 'tree'},
        });

        expect(applied.areaStateVersion).toBe(1);
        expect(applied.appliedDeltaThroughSequence).toBe(1);

        const replayed = service.replay(areaIdentity.areaId);
        expect(replayed?.canonicalStateSignature)
            .toBe(applied.canonicalStateSignature);
        expect(replayed?.areaStateVersion).toBe(1);
      });

  test(
      'rejects mesh-like delta payloads at authoritative persistence boundary',
      () => {
        const service = createReplayMergeService();

        const areaIdentity = {
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-a',
          chunkX: 4,
          chunkY: 2,
          partitionEpoch: 1,
          identityHash: 'abcdabcdabcdabcd',
          areaId: 'area-4-2',
        };

        service.ensureAuthoritativeBaseline({
          areaIdentity,
          stateVersion: {
            areaId: areaIdentity.areaId,
            areaStateVersion: 0,
            appliedDeltaThroughSequence: 0,
            canonicalStateSignature: 'sig0',
          },
          baselineId: '550e8400-e29b-41d4-a716-446655440000',
          orchestrationPathId: 'baseline-plus-delta-replay-v1',
        });

        expect(() => service.applyDelta({
          areaIdentity,
          baselineId: '550e8400-e29b-41d4-a716-446655440000',
          baseAreaStateVersion: 0,
          idempotencyKey: 'k-mesh',
          deltaPayload: {
            op: 'replace',
            baselineMesh: {vertices: [1, 2, 3]},
          },
        })).toThrow('persistent_world_mesh_blob_not_allowed');
      });
});
