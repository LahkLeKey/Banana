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
      checkpointRouteSignatureTag: 'route:default',
      checkpointFusionLaneTag: 'fusion:lane-default',
      checkpointSynthesisPassTag: 'synthesis:pass-default',
      checkpointReplayPhaseTag: 'replay:phase-default',
      checkpointConvergenceLaneTag: 'convergence:lane-default',
      checkpointReconciliationLaneTag: 'reconciliation:lane-default',
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

  test('rejects legacy route key delimiter diagnostics', () => {
    const {service, payload} = makeValidPayload();

    expect(() => service.validateAndNormalize({
      ...payload,
      routeKey: 'neo-musa->metro-crescent',
    })).toThrow('persistent_world_continuity_route_key_legacy_format');
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

  test('rejects non-canonical variant identity formatting', () => {
    const {service, payload} = makeValidPayload();
    const nonCanonicalVariant = {
      ...payload,
      fromVariantId: ' Neo-Musa ',
    };

    expect(() => service.validateAndNormalize({
      ...nonCanonicalVariant,
      routeKey: service.canonicalRouteKey(
          nonCanonicalVariant.fromVariantId, nonCanonicalVariant.toVariantId),
      transitionSignature:
          service.expectedTransitionSignature(nonCanonicalVariant),
    })).toThrow('persistent_world_continuity_variant_identity_mismatch');
  });

  test('rejects unsupported variant identifiers', () => {
    const {service, payload} = makeValidPayload();
    const unsupportedVariant = {
      ...payload,
      fromVariantId: 'legacy-zone',
    };

    expect(() => service.validateAndNormalize({
      ...unsupportedVariant,
      routeKey: service.canonicalRouteKey(
          unsupportedVariant.fromVariantId, unsupportedVariant.toVariantId),
      transitionSignature:
          service.expectedTransitionSignature(unsupportedVariant),
    })).toThrow('persistent_world_continuity_variant_unsupported');
  });

  test('rejects non-canonical world identity formatting', () => {
    const {service, payload} = makeValidPayload();

    expect(() => service.validateAndNormalize({
      ...payload,
      worldId: '550E8400-E29B-41D4-A716-446655440000',
    })).toThrow('persistent_world_continuity_world_identity_mismatch');
  });

  test('rejects non-canonical lane identity formatting', () => {
    const {service, payload} = makeValidPayload();

    expect(() => service.validateAndNormalize({
      ...payload,
      laneId: ' Lane-Primary ',
    })).toThrow('persistent_world_continuity_lane_identity_mismatch');
  });

  test('rejects non-canonical checkpoint context formatting', () => {
    const {service, payload} = makeValidPayload();

    expect(() => service.validateAndNormalize({
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointContextTag: ' District:Market-A ',
      },
    })).toThrow('persistent_world_continuity_checkpoint_context_mismatch');
  });

  test('rejects non-canonical profile fingerprint formatting', () => {
    const {service, payload} = makeValidPayload();

    expect(() => service.validateAndNormalize({
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        profileFingerprint: ' Profile-FP-A ',
      },
    })).toThrow('persistent_world_continuity_profile_fingerprint_mismatch');
  });

  test('rejects non-canonical checkpoint id formatting', () => {
    const {service, payload} = makeValidPayload();
    const drifted = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointId: ' Quest-Intro ',
      },
    };

    expect(() => service.validateAndNormalize({
      ...drifted,
      transitionSignature: service.expectedTransitionSignature(drifted),
    })).toThrow('persistent_world_continuity_checkpoint_id_mismatch');
  });

  test('rejects non-canonical checkpoint route signature tag formatting', () => {
    const {service, payload} = makeValidPayload();
    const drifted = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointRouteSignatureTag: ' Route:Default ',
      },
    };

    expect(() => service.validateAndNormalize({
      ...drifted,
      transitionSignature: service.expectedTransitionSignature(drifted),
    }))
        .toThrow(
            'persistent_world_continuity_checkpoint_route_signature_mismatch');
  });

  test('rejects non-canonical checkpoint fusion lane tag formatting', () => {
    const {service, payload} = makeValidPayload();
    const drifted = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointFusionLaneTag: ' Fusion:Lane-Default ',
      },
    };

    expect(() => service.validateAndNormalize({
      ...drifted,
      transitionSignature: service.expectedTransitionSignature(drifted),
    })).toThrow('persistent_world_continuity_checkpoint_fusion_lane_mismatch');
  });

  test('rejects non-canonical checkpoint synthesis pass tag formatting', () => {
    const {service, payload} = makeValidPayload();
    const drifted = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointSynthesisPassTag: ' Synthesis:Pass-Default ',
      },
    };

    expect(() => service.validateAndNormalize({
      ...drifted,
      transitionSignature: service.expectedTransitionSignature(drifted),
    }))
        .toThrow(
            'persistent_world_continuity_checkpoint_synthesis_pass_mismatch');
  });

  test('rejects non-canonical checkpoint replay phase tag formatting', () => {
    const {service, payload} = makeValidPayload();
    const drifted = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointReplayPhaseTag: ' Replay:Phase-Default ',
      },
    };

    expect(() => service.validateAndNormalize({
      ...drifted,
      transitionSignature: service.expectedTransitionSignature(drifted),
    })).toThrow('persistent_world_continuity_checkpoint_replay_phase_mismatch');
  });

  test('rejects non-canonical checkpoint convergence lane tag formatting', () => {
    const {service, payload} = makeValidPayload();
    const drifted = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointConvergenceLaneTag: ' Convergence:Lane-Default ',
      },
    };

    expect(() => service.validateAndNormalize({
      ...drifted,
      transitionSignature: service.expectedTransitionSignature(drifted),
    }))
        .toThrow(
            'persistent_world_continuity_checkpoint_convergence_lane_mismatch');
  });

  test('rejects non-canonical checkpoint reconciliation lane tag formatting', () => {
    const {service, payload} = makeValidPayload();
    const drifted = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointReconciliationLaneTag: ' Reconciliation:Lane-Default ',
      },
    };

    expect(() => service.validateAndNormalize({
      ...drifted,
      transitionSignature: service.expectedTransitionSignature(drifted),
    }))
        .toThrow(
            'persistent_world_continuity_checkpoint_reconciliation_lane_mismatch');
  });

  test('rejects non-canonical checkpoint observation lane tag formatting', () => {
    const {service, payload} = makeValidPayload();
    const drifted = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        checkpointObservationLaneTag: ' Observation:Lane-Default ',
      },
    };

    expect(() => service.validateAndNormalize({
      ...drifted,
      transitionSignature: service.expectedTransitionSignature(drifted),
    }))
        .toThrow(
            'persistent_world_continuity_checkpoint_observation_lane_mismatch');
  });

  test('rejects non-canonical objective completion ids ordering', () => {
    const {service, payload} = makeValidPayload();
    const orderingDrift = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        objectiveCompletionIds: ['objective-b', 'objective-a'],
      },
    };

    expect(() => service.validateAndNormalize({
      ...orderingDrift,
      transitionSignature: service.expectedTransitionSignature(orderingDrift),
    }))
        .toThrow(
            'persistent_world_continuity_objective_completion_ids_mismatch');
  });

  test('rejects duplicate objective completion ids as non-canonical', () => {
    const {service, payload} = makeValidPayload();
    const duplicateDrift = {
      ...payload,
      checkpoint: {
        ...payload.checkpoint,
        objectiveCompletionIds: ['objective-a', 'objective-a'],
      },
    };

    expect(() => service.validateAndNormalize({
      ...duplicateDrift,
      transitionSignature: service.expectedTransitionSignature(duplicateDrift),
    }))
        .toThrow(
            'persistent_world_continuity_objective_completion_ids_mismatch');
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

  test(
      'additive checkpoint route signature field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointRouteSignatureTag: 'route:growth-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint fusion lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointFusionLaneTag: 'fusion:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint synthesis pass field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointSynthesisPassTag: 'synthesis:pass-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint replay phase field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointReplayPhaseTag: 'replay:phase-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint convergence lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointConvergenceLaneTag: 'convergence:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint reconciliation lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointReconciliationLaneTag: 'reconciliation:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint observation lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointObservationLaneTag: 'observation:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint attunement lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointAttunementLaneTag: 'attunement:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint echo lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointEchoLaneTag: 'echo:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint resonance lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointResonanceLaneTag: 'resonance:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint horizon lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointHorizonLaneTag: 'horizon:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint meridian lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointMeridianLaneTag: 'meridian:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint aurora lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointAuroraLaneTag: 'aurora:lane-urban-v2',
          },
        };
        const mutatedSignature =
            service.expectedTransitionSignature(mutatedPayload);
        const repeatedMutated =
            service.expectedTransitionSignature(mutatedPayload);

        expect(mutatedSignature).not.toBe(baselineSignature);
        expect(repeatedMutated).toBe(mutatedSignature);
      });

  test(
      'additive checkpoint stellar lane field mutates signature deterministically',
      () => {
        const {service, payload} = makeValidPayload();

        const baselineSignature = service.expectedTransitionSignature(payload);
        const mutatedPayload = {
          ...payload,
          checkpoint: {
            ...payload.checkpoint,
            checkpointStellarLaneTag: 'stellar:lane-urban-v2',
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
