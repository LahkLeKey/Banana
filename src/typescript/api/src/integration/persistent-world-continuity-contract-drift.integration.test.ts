import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetBaselineRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/baselineRepository.ts';
import {resetDeltaRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/deltaRepository.ts';
import {resetAreaStateVersionServiceForTests} from '../domains/persistent-world-orchestration/services/areaStateVersionService.ts';
import {createContinuityPayloadService} from '../domains/persistent-world-orchestration/services/continuityPayloadService.ts';
import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerWorldRoutes} from '../routes/world.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app);
  await app.ready();
  return app;
}

function makePayload() {
  const continuityService = createContinuityPayloadService();
  const seed = {
    contractVersion: 'v1' as const,
    worldId: '550e8400-e29b-41d4-a716-446655440000',
    laneId: 'lane-main',
    fromVariantId: 'neo-musa',
    toVariantId: 'metro-crescent',
    routeKey: continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
    partitionEpoch: 1,
    chunkX: 2,
    chunkY: 3,
    checkpoint: {
      checkpointId: 'checkpoint-a',
      checkpointSequence: 1,
      checkpointContextTag: 'district:market-a',
      checkpointRouteSignatureTag: 'route:default',
      checkpointFusionLaneTag: 'fusion:lane-default',
      checkpointSynthesisPassTag: 'synthesis:pass-default',
      objectiveCompletionIds: ['objective-a'],
      profileState: {faction: 'banana-guild'},
      profileFingerprint: 'profile-fp-a',
    },
    transitionSignature: '',
  };

  return {
    continuityPayload: {
      ...seed,
      transitionSignature: continuityService.expectedTransitionSignature(seed),
    },
    baseAreaStateVersion: 0,
    idempotencyKey: 'continuity-drift-0001',
  };
}

describe('US2 continuity contract drift guard integration', () => {
  test('rejects strict-schema drift in continuity payload fields', async () => {
    resetBaselineRepositoryForTests();
    resetDeltaRepositoryForTests();
    resetAreaStateVersionServiceForTests();

    const app = await createApp();
    const payload = makePayload();
    (payload.continuityPayload as unknown as Record<string, unknown>)
        .runtimeSchemaVersion = 'native-v2';

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
      'rejects contract version drift while preserving deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        (payload.continuityPayload as unknown as {
          contractVersion: string
        }).contractVersion = 'v2';

        const first = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        const second = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_payload_invalid');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_payload_invalid');

        await app.close();
      });

  test(
      'rejects unsupported variant IDs with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.fromVariantId = 'legacy-zone';
        payload.continuityPayload.routeKey =
            createContinuityPayloadService().canonicalRouteKey(
                payload.continuityPayload.fromVariantId,
                payload.continuityPayload.toVariantId);
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_variant_unsupported');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_variant_unsupported');

        await app.close();
      });

  test(
      'rejects non-canonical world identity with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.worldId =
            '550E8400-E29B-41D4-A716-446655440000';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_world_identity_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_world_identity_mismatch');

        await app.close();
      });

  test(
      'rejects non-canonical lane identity with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.laneId = ' Lane-Main ';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_lane_identity_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_lane_identity_mismatch');

        await app.close();
      });

  test(
      'rejects non-canonical checkpoint context with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
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
          payload,
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
      'rejects non-canonical profile fingerprint with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.checkpoint.profileFingerprint =
            ' Profile-FP-A ';
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
          payload,
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
      'rejects non-canonical objective completion ids ordering with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.checkpoint.objectiveCompletionIds =
            ['objective-beta', 'objective-alpha'];
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
          payload,
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
      'rejects non-canonical checkpoint id with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
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
          payload,
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
      'rejects non-canonical checkpoint route signature with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.checkpoint.checkpointRouteSignatureTag =
            ' Route:Default ';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_route_signature_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_route_signature_mismatch');

        await app.close();
      });

  test(
      'rejects non-canonical checkpoint fusion lane with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.checkpoint.checkpointFusionLaneTag =
            ' Fusion:Lane-Default ';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_fusion_lane_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_fusion_lane_mismatch');

        await app.close();
      });

  test(
      'rejects non-canonical checkpoint synthesis pass with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.checkpoint.checkpointSynthesisPassTag =
            ' Synthesis:Pass-Default ';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_synthesis_pass_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_synthesis_pass_mismatch');

        await app.close();
      });

  test(
      'rejects non-canonical checkpoint replay phase with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.checkpoint.checkpointReplayPhaseTag =
            ' Replay:Phase-Default ';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_replay_phase_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_replay_phase_mismatch');

        await app.close();
      });

  test(
      'rejects non-canonical checkpoint convergence lane with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.checkpoint.checkpointConvergenceLaneTag =
            ' Convergence:Lane-Default ';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_convergence_lane_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_convergence_lane_mismatch');

        await app.close();
      });

  test(
      'rejects non-canonical checkpoint reconciliation lane with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.checkpoint.checkpointReconciliationLaneTag =
            ' Reconciliation:Lane-Default ';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_reconciliation_lane_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_reconciliation_lane_mismatch');

        await app.close();
      });

  test(
      'rejects non-canonical checkpoint observation lane with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.checkpoint.checkpointObservationLaneTag =
            ' Observation:Lane-Default ';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_observation_lane_mismatch');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_observation_lane_mismatch');

        await app.close();
      });

  test(
      'rejects legacy route delimiter format with deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        payload.continuityPayload.routeKey = 'neo-musa->metro-crescent';
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
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_route_key_legacy_format');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_route_key_legacy_format');

        await app.close();
      });
});
