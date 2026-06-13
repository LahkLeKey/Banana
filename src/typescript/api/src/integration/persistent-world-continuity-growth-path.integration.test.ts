import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetBaselineRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/baselineRepository.ts';
import {resetDeltaRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/deltaRepository.ts';
import {resetAreaStateVersionServiceForTests} from '../domains/persistent-world-orchestration/services/areaStateVersionService.ts';
import {createContinuityPayloadService} from '../domains/persistent-world-orchestration/services/continuityPayloadService.ts';
import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {createWorldServiceForTests} from '../routes/world.test-service.ts';
import {registerWorldRoutes} from '../routes/world.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app, {worldService: createWorldServiceForTests()});
  await app.ready();
  return app;
}

function makePayload(
    contextTag: string, routeSignatureTag = 'route:default',
    fusionLaneTag = 'fusion:lane-default',
    synthesisPassTag = 'synthesis:pass-default',
    replayPhaseTag = 'replay:phase-default',
    convergenceLaneTag = 'convergence:lane-default',
    reconciliationLaneTag = 'reconciliation:lane-default',
    observationLaneTag = 'observation:lane-default',
    attunementLaneTag = 'attunement:lane-default',
    echoLaneTag = 'echo:lane-default',
    resonanceLaneTag = 'resonance:lane-default',
    horizonLaneTag = 'horizon:lane-default',
    meridianLaneTag = 'meridian:lane-default',
    auroraLaneTag = 'aurora:lane-default',
    stellarLaneTag = 'stellar:lane-default') {
  const continuityService = createContinuityPayloadService();
  const seed = {
    contractVersion: 'v1' as const,
    worldId: '550e8400-e29b-41d4-a716-446655440000',
    laneId: 'lane-growth',
    fromVariantId: 'neo-musa',
    toVariantId: 'metro-crescent',
    routeKey: continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
    partitionEpoch: 1,
    chunkX: 2,
    chunkY: 9,
    checkpoint: {
      checkpointId: 'checkpoint-growth',
      checkpointSequence: 1,
      checkpointContextTag: contextTag,
      checkpointRouteSignatureTag: routeSignatureTag,
      checkpointFusionLaneTag: fusionLaneTag,
      checkpointSynthesisPassTag: synthesisPassTag,
      checkpointReplayPhaseTag: replayPhaseTag,
      checkpointConvergenceLaneTag: convergenceLaneTag,
      checkpointReconciliationLaneTag: reconciliationLaneTag,
      checkpointObservationLaneTag: observationLaneTag,
      checkpointAttunementLaneTag: attunementLaneTag,
      checkpointEchoLaneTag: echoLaneTag,
      checkpointResonanceLaneTag: resonanceLaneTag,
      checkpointHorizonLaneTag: horizonLaneTag,
      checkpointMeridianLaneTag: meridianLaneTag,
      checkpointAuroraLaneTag: auroraLaneTag,
      checkpointStellarLaneTag: stellarLaneTag,
      objectiveCompletionIds: ['objective-growth'],
      profileState: {faction: 'banana-guild'},
      profileFingerprint: 'profile-growth-a',
    },
    transitionSignature: '',
  };

  return {
    continuityPayload: {
      ...seed,
      transitionSignature: continuityService.expectedTransitionSignature(seed),
    },
    baseAreaStateVersion: 0,
    idempotencyKey: 'continuity-growth-0001',
  };
}

describe('US3 continuity growth-path integration', () => {
  test(
      'new checkpoint context field persists and rehydrates through owning contract path',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload(
            'district:growth-market', 'route:growth-v2',
            'fusion:lane-growth-v2', 'synthesis:pass-growth-v2',
            'replay:phase-growth-v2', 'convergence:lane-growth-v2',
            'reconciliation:lane-growth-v2', 'observation:lane-growth-v2',
            'attunement:lane-growth-v2', 'echo:lane-growth-v2',
            'resonance:lane-growth-v2', 'horizon:lane-growth-v2',
            'meridian:lane-growth-v2', 'aurora:lane-growth-v2',
            'stellar:lane-growth-v2');

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(commit.statusCode).toBe(200);
        const committed = commit.json() as {
          areaIdentity: {areaId: string};
          continuityPayload: {
            checkpoint: {
              checkpointContextTag: string; checkpointRouteSignatureTag: string;
              checkpointFusionLaneTag: string;
              checkpointSynthesisPassTag: string;
              checkpointReplayPhaseTag: string;
              checkpointConvergenceLaneTag: string;
              checkpointReconciliationLaneTag: string;
              checkpointObservationLaneTag: string;
              checkpointAttunementLaneTag: string;
              checkpointEchoLaneTag: string;
              checkpointResonanceLaneTag: string;
              checkpointHorizonLaneTag: string;
              checkpointMeridianLaneTag: string;
              checkpointAuroraLaneTag: string;
              checkpointStellarLaneTag: string;
            };
          };
        };

        const rehydrate = await app.inject({
          method: 'GET',
          url: `/api/world/continuity/checkpoint/${
              committed.areaIdentity.areaId}`,
        });

        expect(rehydrate.statusCode).toBe(200);
        expect(rehydrate.json()).toMatchObject({
          continuityPayload: {
            checkpoint: {
              checkpointContextTag: 'district:growth-market',
              checkpointRouteSignatureTag: 'route:growth-v2',
              checkpointFusionLaneTag: 'fusion:lane-growth-v2',
              checkpointSynthesisPassTag: 'synthesis:pass-growth-v2',
              checkpointReplayPhaseTag: 'replay:phase-growth-v2',
              checkpointConvergenceLaneTag: 'convergence:lane-growth-v2',
              checkpointReconciliationLaneTag: 'reconciliation:lane-growth-v2',
              checkpointObservationLaneTag: 'observation:lane-growth-v2',
              checkpointAttunementLaneTag: 'attunement:lane-growth-v2',
              checkpointEchoLaneTag: 'echo:lane-growth-v2',
              checkpointResonanceLaneTag: 'resonance:lane-growth-v2',
              checkpointHorizonLaneTag: 'horizon:lane-growth-v2',
              checkpointMeridianLaneTag: 'meridian:lane-growth-v2',
              checkpointAuroraLaneTag: 'aurora:lane-growth-v2',
              checkpointStellarLaneTag: 'stellar:lane-growth-v2',
            },
          },
        });

        await app.close();
      });

  test(
      'disabled checkpoint context values emit explicit diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload('disabled:legacy-zone');

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
});
