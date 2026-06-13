import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetBaselineRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/baselineRepository.ts';
import {resetDeltaRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/deltaRepository.ts';
import {resetAreaStateVersionServiceForTests} from '../domains/persistent-world-orchestration/services/areaStateVersionService.ts';
import {createContinuityPayloadService} from '../domains/persistent-world-orchestration/services/continuityPayloadService.ts';
import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerWorldRoutes} from '../routes/world.ts';
import {createWorldServiceForTests} from '../routes/world.test-service.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app, {worldService: createWorldServiceForTests()});
  await app.ready();
  return app;
}

function makePayload(checkpointSequence: number, idempotencyKey: string) {
  const continuityService = createContinuityPayloadService();
  const seed = {
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
      checkpointId: `checkpoint-${checkpointSequence}`,
      checkpointSequence,
      checkpointContextTag: `district:market-${checkpointSequence}`,
      checkpointRouteSignatureTag: 'route:default',
      checkpointFusionLaneTag: 'fusion:lane-default',
      checkpointSynthesisPassTag: 'synthesis:pass-default',
      checkpointReplayPhaseTag: 'replay:phase-default',
      checkpointConvergenceLaneTag: 'convergence:lane-default',
      checkpointReconciliationLaneTag: 'reconciliation:lane-default',
      checkpointObservationLaneTag: 'observation:lane-default',
      checkpointAttunementLaneTag: 'attunement:lane-default',
      checkpointEchoLaneTag: 'echo:lane-default',
      checkpointResonanceLaneTag: 'resonance:lane-default',
      checkpointHorizonLaneTag: 'horizon:lane-default',
      checkpointMeridianLaneTag: 'meridian:lane-default',
      checkpointAuroraLaneTag: 'aurora:lane-default',
      checkpointStellarLaneTag: 'stellar:lane-default',
      objectiveCompletionIds: ['objective-greet-warden', 'objective-open-gate'],
      profileState: {
        faction: 'banana-guild',
        sequence: checkpointSequence,
      },
      profileFingerprint: `profile-fingerprint-${checkpointSequence}`,
    },
    transitionSignature: '',
  };

  return {
    continuityPayload: {
      ...seed,
      transitionSignature: continuityService.expectedTransitionSignature(seed),
    },
    baseAreaStateVersion: Math.max(0, checkpointSequence - 1),
    idempotencyKey,
  };
}

describe('US1 continuity round-trip integration', () => {
  beforeEach(() => {
    resetBaselineRepositoryForTests();
    resetDeltaRepositoryForTests();
    resetAreaStateVersionServiceForTests();
  });

  test(
      'persists and rehydrates checkpoints deterministically over repeated runs',
      async () => {
        const app = await createApp();

        let areaId = '';
        let canonicalSignature = '';

        for (let iteration = 1; iteration <= 10; iteration++) {
          const commit = await app.inject({
            method: 'POST',
            url: '/api/world/continuity/checkpoint',
            payload:
                makePayload(iteration, `continuity-roundtrip-${iteration}`),
          });

          expect(commit.statusCode).toBe(200);
          const commitBody = commit.json() as {
            areaIdentity: {areaId: string};
            areaStateVersion:
                {areaStateVersion: number; canonicalStateSignature: string};
            continuityPayload: {
              checkpoint:
                  {checkpointSequence: number; checkpointContextTag: string;}
            };
          };

          areaId = commitBody.areaIdentity.areaId;
          canonicalSignature =
              commitBody.areaStateVersion.canonicalStateSignature;
          expect(commitBody.continuityPayload.checkpoint.checkpointSequence)
              .toBe(iteration);
          expect(commitBody.continuityPayload.checkpoint.checkpointContextTag)
              .toBe(`district:market-${iteration}`);
          expect(commitBody.areaStateVersion.areaStateVersion).toBe(iteration);

          const rehydrate = await app.inject({
            method: 'GET',
            url: `/api/world/continuity/checkpoint/${areaId}`,
          });

          expect(rehydrate.statusCode).toBe(200);
          const rehydrateBody = rehydrate.json() as {
            areaIdentity: {areaId: string};
            areaStateVersion:
                {areaStateVersion: number; canonicalStateSignature: string};
            continuityPayload: {
              checkpoint:
                  {checkpointSequence: number; checkpointContextTag: string;}
            };
          };

          expect(rehydrateBody.areaIdentity.areaId).toBe(areaId);
          expect(rehydrateBody.areaStateVersion.areaStateVersion)
              .toBe(iteration);
          expect(rehydrateBody.areaStateVersion.canonicalStateSignature)
              .toBe(canonicalSignature);
          expect(rehydrateBody.continuityPayload.checkpoint.checkpointSequence)
              .toBe(iteration);
          expect(
              rehydrateBody.continuityPayload.checkpoint.checkpointContextTag)
              .toBe(`district:market-${iteration}`);
        }

        const replayState = await app.inject({
          method: 'GET',
          url: `/api/world/state/${areaId}`,
        });

        expect(replayState.statusCode).toBe(200);
        expect((replayState.json() as {
                 areaStateVersion: {canonicalStateSignature: string};
               }).areaStateVersion.canonicalStateSignature)
            .toBe(canonicalSignature);

        await app.close();
      });

  test(
      'mixed entry path commits preserve one authoritative area lineage',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const forwardSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 12,
          chunkY: 7,
          checkpoint: {
            checkpointId: 'checkpoint-forward',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-forward',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-forward'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-forward',
          },
          transitionSignature: '',
        };

        const forwardCommit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            continuityPayload: {
              ...forwardSeed,
              transitionSignature:
                  continuityService.expectedTransitionSignature(forwardSeed),
            },
            baseAreaStateVersion: 0,
            idempotencyKey: 'mixed-entry-forward-1',
          },
        });

        expect(forwardCommit.statusCode).toBe(200);
        const forwardBody = forwardCommit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };

        const reverseSeed = {
          ...forwardSeed,
          fromVariantId: 'metro-crescent',
          toVariantId: 'neo-musa',
          routeKey:
              continuityService.canonicalRouteKey('metro-crescent', 'neo-musa'),
          checkpoint: {
            ...forwardSeed.checkpoint,
            checkpointId: 'checkpoint-reverse',
            checkpointSequence: 2,
            checkpointContextTag: 'district:market-reverse',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            profileFingerprint: 'profile-reverse',
          },
        };

        const reverseCommit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            continuityPayload: {
              ...reverseSeed,
              transitionSignature:
                  continuityService.expectedTransitionSignature(reverseSeed),
            },
            baseAreaStateVersion: 1,
            idempotencyKey: 'mixed-entry-reverse-1',
          },
        });

        expect(reverseCommit.statusCode).toBe(200);
        const reverseBody = reverseCommit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };

        expect(reverseBody.areaIdentity.areaId)
            .toBe(forwardBody.areaIdentity.areaId);
        expect(forwardBody.areaStateVersion.areaStateVersion).toBe(1);
        expect(reverseBody.areaStateVersion.areaStateVersion).toBe(2);

        const replay = await app.inject({
          method: 'GET',
          url: `/api/world/continuity/checkpoint/${
              forwardBody.areaIdentity.areaId}`,
        });

        expect(replay.statusCode).toBe(200);
        expect(replay.json()).toMatchObject({
          areaIdentity: {
            areaId: forwardBody.areaIdentity.areaId,
          },
          continuityPayload: {
            checkpoint: {
              checkpointId: 'checkpoint-reverse',
            },
          },
        });

        await app.close();
      });

  test(
      'mixed entry idempotent replay writes preserve authoritative lineage and state version',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const seed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 12,
          chunkY: 7,
          checkpoint: {
            checkpointId: 'checkpoint-idempotent',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-idempotent',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-idempotent'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-idempotent',
          },
          transitionSignature: '',
        };

        const payload = {
          continuityPayload: {
            ...seed,
            transitionSignature:
                continuityService.expectedTransitionSignature(seed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'mixed-entry-idempotent-1',
        };

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

        expect(first.statusCode).toBe(200);
        expect(second.statusCode).toBe(200);

        const firstBody = first.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        const secondBody = second.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };

        expect(secondBody.areaIdentity.areaId)
            .toBe(firstBody.areaIdentity.areaId);
        expect(firstBody.areaStateVersion.areaStateVersion).toBe(1);
        expect(secondBody.areaStateVersion.areaStateVersion).toBe(1);

        const replay = await app.inject({
          method: 'GET',
          url: `/api/world/continuity/checkpoint/${
              firstBody.areaIdentity.areaId}`,
        });

        expect(replay.statusCode).toBe(200);
        expect(replay.json()).toMatchObject({
          areaIdentity: {
            areaId: firstBody.areaIdentity.areaId,
          },
          areaStateVersion: {
            areaStateVersion: 1,
          },
          continuityPayload: {
            checkpoint: {
              checkpointId: 'checkpoint-idempotent',
            },
          },
        });

        await app.close();
      });

  test(
      'invalid checkpoint-context retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 12,
          chunkY: 7,
          checkpoint: {
            checkpointId: 'checkpoint-context-retry',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-retry',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-retry'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-retry',
          },
          transitionSignature: '',
        };

        const invalidPayload = {
          continuityPayload: {
            ...baseSeed,
            checkpoint: {
              ...baseSeed.checkpoint,
              checkpointContextTag: ' District:Market-Retry ',
            },
            transitionSignature: continuityService.expectedTransitionSignature({
              ...baseSeed,
              checkpoint: {
                ...baseSeed.checkpoint,
                checkpointContextTag: ' District:Market-Retry ',
              },
            }),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'context-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'context-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_checkpoint_context_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_checkpoint_context_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'context-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        const replay = await app.inject({
          method: 'GET',
          url: `/api/world/continuity/checkpoint/${body.areaIdentity.areaId}`,
        });
        expect(replay.statusCode).toBe(200);
        expect(replay.json()).toMatchObject({
          areaIdentity: {areaId: body.areaIdentity.areaId},
          areaStateVersion: {areaStateVersion: 1},
          continuityPayload: {
            checkpoint: {
              checkpointContextTag: 'district:market-retry',
            },
          },
        });

        await app.close();
      });

  test(
      'invalid profile-fingerprint retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 12,
          chunkY: 7,
          checkpoint: {
            checkpointId: 'checkpoint-fingerprint-retry',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-fingerprint',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-fingerprint-retry'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-fingerprint-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            profileFingerprint: ' Profile-Fingerprint-Canonical ',
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'fingerprint-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'fingerprint-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_profile_fingerprint_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_profile_fingerprint_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'fingerprint-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        const replay = await app.inject({
          method: 'GET',
          url: `/api/world/continuity/checkpoint/${body.areaIdentity.areaId}`,
        });
        expect(replay.statusCode).toBe(200);
        expect(replay.json()).toMatchObject({
          areaIdentity: {areaId: body.areaIdentity.areaId},
          areaStateVersion: {areaStateVersion: 1},
          continuityPayload: {
            checkpoint: {
              profileFingerprint: 'profile-fingerprint-canonical',
            },
          },
        });

        await app.close();
      });

  test(
      'invalid objective-completion-ids ordering retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 18,
          chunkY: 21,
          checkpoint: {
            checkpointId: 'checkpoint-objective-retry',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-objective',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-alpha', 'objective-beta'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-objective-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            objectiveCompletionIds: ['objective-beta', 'objective-alpha'],
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'objective-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'objective-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_objective_completion_ids_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_objective_completion_ids_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'objective-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        const replay = await app.inject({
          method: 'GET',
          url: `/api/world/continuity/checkpoint/${body.areaIdentity.areaId}`,
        });
        expect(replay.statusCode).toBe(200);
        expect(replay.json()).toMatchObject({
          areaIdentity: {areaId: body.areaIdentity.areaId},
          areaStateVersion: {areaStateVersion: 1},
          continuityPayload: {
            checkpoint: {
              objectiveCompletionIds: ['objective-alpha', 'objective-beta'],
            },
          },
        });

        await app.close();
      });

  test(
      'invalid checkpoint-id retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 19,
          chunkY: 23,
          checkpoint: {
            checkpointId: 'checkpoint-id-canonical',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-id',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-id-canonical'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-id-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            checkpointId: ' Checkpoint-ID-Canonical ',
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'checkpoint-id-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'checkpoint-id-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_checkpoint_id_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_checkpoint_id_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'checkpoint-id-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        const replay = await app.inject({
          method: 'GET',
          url: `/api/world/continuity/checkpoint/${body.areaIdentity.areaId}`,
        });
        expect(replay.statusCode).toBe(200);
        expect(replay.json()).toMatchObject({
          areaIdentity: {areaId: body.areaIdentity.areaId},
          areaStateVersion: {areaStateVersion: 1},
          continuityPayload: {
            checkpoint: {
              checkpointId: 'checkpoint-id-canonical',
            },
          },
        });

        await app.close();
      });

  test(
      'invalid route-signature retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 21,
          chunkY: 24,
          checkpoint: {
            checkpointId: 'checkpoint-route-signature-canonical',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-route-signature',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-route-signature'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-route-signature-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            checkpointRouteSignatureTag: ' Route:Default ',
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'route-signature-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'route-signature-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_route_signature_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_route_signature_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'route-signature-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        await app.close();
      });

  test(
      'invalid fusion-lane retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 27,
          chunkY: 31,
          checkpoint: {
            checkpointId: 'checkpoint-fusion-lane-canonical',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-fusion-lane',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-fusion-lane'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-fusion-lane-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            checkpointFusionLaneTag: ' Fusion:Lane-Default ',
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'fusion-lane-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'fusion-lane-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_fusion_lane_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_fusion_lane_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'fusion-lane-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        await app.close();
      });

  test(
      'invalid synthesis-pass retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 33,
          chunkY: 38,
          checkpoint: {
            checkpointId: 'checkpoint-synthesis-pass-canonical',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-synthesis-pass',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-synthesis-pass'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-synthesis-pass-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            checkpointSynthesisPassTag: ' Synthesis:Pass-Default ',
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'synthesis-pass-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'synthesis-pass-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_synthesis_pass_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_synthesis_pass_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'synthesis-pass-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        await app.close();
      });

  test(
      'invalid replay-phase retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 41,
          chunkY: 46,
          checkpoint: {
            checkpointId: 'checkpoint-replay-phase-canonical',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-replay-phase',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-replay-phase'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-replay-phase-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            checkpointReplayPhaseTag: ' Replay:Phase-Default ',
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'replay-phase-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'replay-phase-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_replay_phase_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_replay_phase_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'replay-phase-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        await app.close();
      });

  test(
      'invalid convergence-lane retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 49,
          chunkY: 54,
          checkpoint: {
            checkpointId: 'checkpoint-convergence-lane-canonical',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-convergence-lane',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-convergence-lane'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-convergence-lane-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            checkpointConvergenceLaneTag: ' Convergence:Lane-Default ',
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'convergence-lane-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'convergence-lane-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_convergence_lane_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_convergence_lane_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'convergence-lane-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        await app.close();
      });

  test(
      'invalid reconciliation-lane retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 51,
          chunkY: 56,
          checkpoint: {
            checkpointId: 'checkpoint-reconciliation-lane-canonical',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-reconciliation-lane',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-reconciliation-lane'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-reconciliation-lane-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            checkpointReconciliationLaneTag: ' Reconciliation:Lane-Default ',
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'reconciliation-lane-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'reconciliation-lane-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_reconciliation_lane_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_reconciliation_lane_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'reconciliation-lane-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        await app.close();
      });

  test(
      'invalid observation-lane retries do not break authoritative replay once canonical commit succeeds',
      async () => {
        const app = await createApp();
        const continuityService = createContinuityPayloadService();

        const baseSeed = {
          contractVersion: 'v1' as const,
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          fromVariantId: 'neo-musa',
          toVariantId: 'metro-crescent',
          routeKey:
              continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
          partitionEpoch: 1,
          chunkX: 53,
          chunkY: 58,
          checkpoint: {
            checkpointId: 'checkpoint-observation-lane-canonical',
            checkpointSequence: 1,
            checkpointContextTag: 'district:market-observation-lane',
            checkpointRouteSignatureTag: 'route:default',
            checkpointFusionLaneTag: 'fusion:lane-default',
            checkpointSynthesisPassTag: 'synthesis:pass-default',
            checkpointReplayPhaseTag: 'replay:phase-default',
            checkpointConvergenceLaneTag: 'convergence:lane-default',
            checkpointReconciliationLaneTag: 'reconciliation:lane-default',
            checkpointObservationLaneTag: 'observation:lane-default',
            checkpointAttunementLaneTag: 'attunement:lane-default',
            checkpointEchoLaneTag: 'echo:lane-default',
            checkpointResonanceLaneTag: 'resonance:lane-default',
            checkpointHorizonLaneTag: 'horizon:lane-default',
            checkpointMeridianLaneTag: 'meridian:lane-default',
            checkpointAuroraLaneTag: 'aurora:lane-default',
            checkpointStellarLaneTag: 'stellar:lane-default',
            objectiveCompletionIds: ['objective-observation-lane'],
            profileState: {faction: 'banana-guild'},
            profileFingerprint: 'profile-observation-lane-canonical',
          },
          transitionSignature: '',
        };

        const driftedSeed = {
          ...baseSeed,
          checkpoint: {
            ...baseSeed.checkpoint,
            checkpointObservationLaneTag: ' Observation:Lane-Default ',
          },
        };

        const invalidPayload = {
          continuityPayload: {
            ...driftedSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(driftedSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'observation-lane-retry-invalid-1',
        };

        const firstReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: invalidPayload,
        });
        const secondReject = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: {
            ...invalidPayload,
            idempotencyKey: 'observation-lane-retry-invalid-2',
          },
        });

        expect(firstReject.statusCode).toBe(400);
        expect(secondReject.statusCode).toBe(400);
        expect((firstReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_observation_lane_mismatch');
        expect(
            (secondReject.json() as {error: {message: string}}).error.message)
            .toBe(
                'persistent_world_continuity_checkpoint_observation_lane_mismatch');

        const canonicalPayload = {
          continuityPayload: {
            ...baseSeed,
            transitionSignature:
                continuityService.expectedTransitionSignature(baseSeed),
          },
          baseAreaStateVersion: 0,
          idempotencyKey: 'observation-lane-retry-canonical-1',
        };

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload: canonicalPayload,
        });

        expect(commit.statusCode).toBe(200);
        const body = commit.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion: {areaStateVersion: number};
        };
        expect(body.areaStateVersion.areaStateVersion).toBe(1);

        await app.close();
      });
});
