import {describe, expect, test} from 'bun:test';

import {createContinuityPayloadService} from './continuityPayloadService.ts';

function makeValidPayload() {
  const service = createContinuityPayloadService();
  const seed = {
    contractVersion: 'v1' as const,
    worldId: '550e8400-e29b-41d4-a716-446655440000',
    laneId: 'lane-primary',
    fromVariantId: 'neo-musa',
    toVariantId: 'metro-crescent',
    routeKey: service.canonicalRouteKey('neo-musa', 'metro-crescent'),
    partitionEpoch: 2,
    chunkX: 4,
    chunkY: 8,
    checkpoint: {
      checkpointId: 'quest-intro',
      checkpointSequence: 1,
      checkpointContextTag: 'district:market-a',
      objectiveCompletionIds: ['objective-a'],
      profileState: {faction: 'banana-guild'},
      profileFingerprint: 'profile-fp-a',
    },
    transitionSignature: '',
  };

  return {
    service,
    payload: {
      ...seed,
      transitionSignature: service.expectedTransitionSignature(seed),
    },
  };
}

describe('continuity payload service', () => {
  test('accepts canonical continuity payloads', () => {
    const {service, payload} = makeValidPayload();
    const result = service.validateAndNormalize(payload);

    expect(result.routeKey).toBe('metro-crescent::neo-musa');
    expect(result.transitionSignature).toBe(payload.transitionSignature);
  });

  test('rejects mismatched route key diagnostics', () => {
    const {service, payload} = makeValidPayload();

    expect(() => service.validateAndNormalize({
      ...payload,
      routeKey: 'neo-musa::banana-docks',
    })).toThrow('persistent_world_continuity_route_key_mismatch');
  });

  test('rejects transition signature drift diagnostics', () => {
    const {service, payload} = makeValidPayload();

    expect(() => service.validateAndNormalize({
      ...payload,
      transitionSignature: 'abcd'.repeat(16),
    })).toThrow('persistent_world_continuity_signature_mismatch');
  });

  test('rejects same-variant transitions as disconnected payloads', () => {
    const {service, payload} = makeValidPayload();
    const sameVariant = {
      ...payload,
      toVariantId: payload.fromVariantId,
      routeKey: service.canonicalRouteKey(
          payload.fromVariantId, payload.fromVariantId),
    };

    expect(() => service.validateAndNormalize({
      ...sameVariant,
      transitionSignature: service.expectedTransitionSignature(sameVariant),
    })).toThrow('persistent_world_continuity_variant_disconnected');
  });

  test(
      'new checkpoint context field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointContextTag: 'district:market-b',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test('rejects disabled or unsupported checkpoint context tags', () => {
    const {service, payload} = makeValidPayload();

    const disabledTagPayload = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointContextTag: 'disabled:legacy-zone',
      },
    };

    expect(() => service.validateAndNormalize({
      ...disabledTagPayload,
      transitionSignature:
          service.expectedTransitionSignature(disabledTagPayload),
    })).toThrow('persistent_world_continuity_checkpoint_context_unsupported');
  });
});
