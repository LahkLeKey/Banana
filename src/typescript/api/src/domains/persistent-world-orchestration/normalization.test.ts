import {describe, expect, it} from 'bun:test';

import {buildAreaIdentity, buildGenerationFingerprint, normalizeRevisitBaselineRequest,} from './normalization.ts';

describe('persistent world normalization', () => {
  it('normalizes equivalent requests into identical area identity hash', () => {
    const a = normalizeRevisitBaselineRequest({
      worldId: '550E8400-E29B-41D4-A716-446655440000',
      laneId: ' primary ',
      chunkX: 10,
      chunkY: -5,
      partitionEpoch: 2,
    });
    const b = normalizeRevisitBaselineRequest({
      worldId: '550e8400-e29b-41d4-a716-446655440000',
      laneId: 'primary',
      chunkX: 10,
      chunkY: -5,
      partitionEpoch: 2,
    });

    const identityA = buildAreaIdentity(a);
    const identityB = buildAreaIdentity(b);

    expect(identityA.identityHash).toBe(identityB.identityHash);
    expect(identityA.areaId).toBe(identityB.areaId);
  });

  it('builds deterministic generation fingerprints for equivalent input',
     () => {
       const input = normalizeRevisitBaselineRequest({
         worldId: '550e8400-e29b-41d4-a716-446655440000',
         laneId: 'primary',
         chunkX: 3,
         chunkY: 7,
         partitionEpoch: 0,
       });

       const first = buildGenerationFingerprint(input);
       const second = buildGenerationFingerprint(input);

       expect(first.derivationPolicyVersion).toBe('v1');
       expect(first.normalizationSchemaVersion).toBe('v1');
       expect(first.generationInputFingerprint)
           .toBe(second.generationInputFingerprint);
     });

  it('rejects invalid revisit request payloads', () => {
    expect(() => {
      normalizeRevisitBaselineRequest({
        worldId: 'invalid-uuid',
        laneId: '',
        chunkX: 1.25,
        chunkY: 2,
        partitionEpoch: -1,
      });
    }).toThrow();
  });
});
