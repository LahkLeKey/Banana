import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetBaselineRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/baselineRepository.ts';
import {resetDeltaRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/deltaRepository.ts';
import {resetAreaStateVersionServiceForTests} from '../domains/persistent-world-orchestration/services/areaStateVersionService.ts';
import {createContinuityPayloadService} from '../domains/persistent-world-orchestration/services/continuityPayloadService.ts';
import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';

import {registerWorldRoutes} from './world.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app);
  await app.ready();
  return app;
}

function makeContinuityCommitPayload() {
  const continuityService = createContinuityPayloadService();
  const continuitySeed = {
    contractVersion: 'v1' as const,
    worldId: '550e8400-e29b-41d4-a716-446655440000',
    laneId: 'lane-primary',
    fromVariantId: 'neo-musa',
    toVariantId: 'metro-crescent',
    routeKey: continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
    partitionEpoch: 1,
    chunkX: 12,
    chunkY: 7,
    checkpoint: {
      checkpointId: 'banana-county-onboarding',
      checkpointSequence: 3,
      checkpointContextTag: 'district:market-a',
      objectiveCompletionIds: ['objective-greet-warden', 'objective-open-gate'],
      profileState: {
        faction: 'banana-guild',
        loadout: 'starter',
      },
      profileFingerprint: 'profile-fingerprint-a',
    },
    transitionSignature: '',
  };

  return {
    continuityPayload: {
      ...continuitySeed,
      transitionSignature:
          continuityService.expectedTransitionSignature(continuitySeed),
    },
    baseAreaStateVersion: 0,
    idempotencyKey: 'continuity-checkpoint-0001',
  };
}

describe('world continuity checkpoint contract', () => {
  beforeEach(() => {
    resetBaselineRepositoryForTests();
    resetDeltaRepositoryForTests();
    resetAreaStateVersionServiceForTests();
  });

  test('accepts canonical continuity payload and persists state', async () => {
    const app = await createApp();

    const payload = makeContinuityCommitPayload();
    const response = await app.inject({
      method: 'POST',
      url: '/api/world/continuity/checkpoint',
      payload,
    });

    expect(response.statusCode).toBe(200);
    expect(response.json()).toMatchObject({
      areaStateVersion: {
        areaStateVersion: 1,
        appliedDeltaThroughSequence: 1,
      },
      continuityPayload: {
        routeKey: payload.continuityPayload.routeKey,
        transitionSignature: payload.continuityPayload.transitionSignature,
      },
    });

    await app.close();
  });

  test(
      'rejects continuity payload with route key drift diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.routeKey = 'neo-musa::banana-docks';

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_route_key_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical world identity diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.worldId =
            '550E8400-E29B-41D4-A716-446655440000';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_world_identity_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with legacy route delimiter diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.routeKey = 'neo-musa->metro-crescent';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_route_key_legacy_format',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with signature drift diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.transitionSignature = 'abcd'.repeat(16);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_signature_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with contract version drift diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        (payload.continuityPayload as unknown as {
          contractVersion: string
        }).contractVersion = 'v2';

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_payload_invalid',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with unknown drift field diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        (payload.continuityPayload as unknown as Record<string, unknown>)
            .runtimeOnlyField = 'unexpected';

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_payload_invalid',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity commit when continuity payload is missing',
      async () => {
        const app = await createApp();

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            baseAreaStateVersion: 0,
            idempotencyKey: 'continuity-missing-payload',
          },
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_payload_invalid',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with unsupported checkpoint context diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointContextTag =
            'unsupported:legacy-zone';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_checkpoint_context_unsupported',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with unsupported variant diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.fromVariantId = 'legacy-zone';
        payload.continuityPayload.routeKey =
            createContinuityPayloadService().canonicalRouteKey(
                payload.continuityPayload.fromVariantId,
                payload.continuityPayload.toVariantId);
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_variant_unsupported',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical lane identity diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.laneId = ' Lane-Primary ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_lane_identity_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical checkpoint context diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointContextTag =
            ' District:Market-A ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_checkpoint_context_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects repeated non-canonical checkpoint context payloads deterministically',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointContextTag =
            ' District:Market-A ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const first = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });
        const second = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...payload,
            idempotencyKey: 'continuity-checkpoint-0002',
          },
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_checkpoint_context_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_checkpoint_context_mismatch');

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical profile fingerprint diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.profileFingerprint =
            ' Profile-Fingerprint-A ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_profile_fingerprint_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects repeated non-canonical profile fingerprint payloads deterministically',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.profileFingerprint =
            ' Profile-Fingerprint-A ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const first = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });
        const second = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...payload,
            idempotencyKey: 'continuity-checkpoint-0003',
          },
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_profile_fingerprint_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_profile_fingerprint_mismatch');

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical objective completion ids diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.objectiveCompletionIds =
            ['objective-open-gate', 'objective-greet-warden'];
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_objective_completion_ids_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects repeated non-canonical objective completion ids payloads deterministically',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.objectiveCompletionIds =
            ['objective-open-gate', 'objective-greet-warden'];
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const first = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });
        const second = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...payload,
            idempotencyKey: 'continuity-checkpoint-0004',
          },
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_objective_completion_ids_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_objective_completion_ids_mismatch');

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical checkpoint id diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointId =
            ' Continuity-Checkpoint-Canonical ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message: 'persistent_world_continuity_checkpoint_id_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects repeated non-canonical checkpoint id payloads deterministically',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointId =
            ' Continuity-Checkpoint-Canonical ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const first = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });
        const second = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...payload,
            idempotencyKey: 'continuity-checkpoint-0005',
          },
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_checkpoint_id_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_checkpoint_id_mismatch');

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical checkpoint route signature diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointRouteSignatureTag =
            ' Route:Default ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_checkpoint_route_signature_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical checkpoint fusion lane diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointFusionLaneTag =
            ' Fusion:Lane-Default ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_checkpoint_fusion_lane_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical checkpoint synthesis pass diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointSynthesisPassTag =
            ' Synthesis:Pass-Default ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_checkpoint_synthesis_pass_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical checkpoint replay phase diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointReplayPhaseTag =
            ' Replay:Phase-Default ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_checkpoint_replay_phase_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical checkpoint convergence lane diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointConvergenceLaneTag =
            ' Convergence:Lane-Default ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_checkpoint_convergence_lane_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical checkpoint reconciliation lane diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointReconciliationLaneTag =
            ' Reconciliation:Lane-Default ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_checkpoint_reconciliation_lane_mismatch',
          },
        });

        await app.close();
      });

  test(
      'rejects continuity payload with non-canonical checkpoint observation lane diagnostics',
      async () => {
        const app = await createApp();

        const payload = makeContinuityCommitPayload();
        payload.continuityPayload.checkpoint.checkpointObservationLaneTag =
            ' Observation:Lane-Default ';
        payload.continuityPayload.transitionSignature =
            createContinuityPayloadService().expectedTransitionSignature(
                payload.continuityPayload);

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_checkpoint_observation_lane_mismatch',
          },
        });

        await app.close();
      });

  test('rehydrates latest continuity checkpoint after write', async () => {
    const app = await createApp();

    const payload = makeContinuityCommitPayload();
    const commit = await app.inject({
      method: 'POST',
      url: '/api/world/continuity/checkpoint',
      payload,
    });

    expect(commit.statusCode).toBe(200);
    const committed = commit.json() as {
      areaIdentity: {areaId: string};
      continuityPayload: {checkpoint: {checkpointId: string};};
    };

    const rehydrate = await app.inject({
      method: 'GET',
      url: `/api/world/continuity/checkpoint/${committed.areaIdentity.areaId}`,
    });

    expect(rehydrate.statusCode).toBe(200);
    expect(rehydrate.json()).toMatchObject({
      continuityPayload: {
        checkpoint: {
          checkpointId: committed.continuityPayload.checkpoint.checkpointId,
        },
      },
      areaStateVersion: {
        areaStateVersion: 1,
      },
    });

    await app.close();
  });
});
