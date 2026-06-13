import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetDomainChangeRecords} from '../../domains/gameplay-session-orchestration/persistence/domainChangeRecorder.ts';
import {registerFastifyErrorMapper} from '../../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../../middleware/requestContext.ts';
import {__resetTrainingEconomyStoreForTests} from '../../services/trainingEconomyStore.ts';

import {registerV1PlayerRoutes} from './player.ts';

async function createApp() {
  process.env.BANANA_TRAINING_STORE_MODE = 'inmemory';
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerV1PlayerRoutes(app);
  await app.ready();
  return app;
}

describe('v1 player training economy contracts', () => {
  beforeEach(() => {
    resetDomainChangeRecords();
    __resetTrainingEconomyStoreForTests();
  });

  test(
      'scaffolds jobs then executes a job and yields pending reward',
      async () => {
        const app = await createApp();

        const scaffold = await app.inject({
          method: 'POST',
          url: '/v1/player/training/jobs/scaffold',
          headers: {'x-actor-id': 'training-player-1'},
          payload: {
            jobs: [
              {
                title: 'Notebook source indexing pass',
                sector: 'pipeline',
                rewardXp: 120
              },
              {title: 'Model drift triage', sector: 'inference', rewardXp: 200},
            ],
          },
        });

        expect(scaffold.statusCode).toBe(201);
        const scaffoldPayload = scaffold.json() as {
          jobs: Array<{jobId: string}>;
        };
        expect(scaffoldPayload.jobs).toHaveLength(2);

        const execute = await app.inject({
          method: 'POST',
          url: `/v1/player/training/jobs/${
              scaffoldPayload.jobs[0]!.jobId}/execute`,
          headers: {'x-actor-id': 'training-player-1'},
          payload: {
            score: 3200,
            durationMs: 14_000,
            integrityProof: 'integrity-proof-abcdef-123456',
          },
        });

        expect(execute.statusCode).toBe(200);
        const executePayload = execute.json() as {
          antiCheatPassed: boolean;
          reward: {status: string};
          job: {status: string};
        };
        expect(executePayload.antiCheatPassed).toBe(true);
        expect(executePayload.job.status).toBe('completed');
        expect(executePayload.reward.status).toBe('pending');

        const rewards = await app.inject({
          method: 'GET',
          url: '/v1/player/training/rewards',
          headers: {'x-actor-id': 'training-player-1'},
        });

        expect(rewards.statusCode).toBe(200);
        const rewardsPayload = rewards.json() as {
          rewards: Array<{rewardId: string; status: string}>;
        };
        expect(rewardsPayload.rewards).toHaveLength(1);
        expect(rewardsPayload.rewards[0]?.status).toBe('pending');

        const claim = await app.inject({
          method: 'POST',
          url: `/v1/player/training/rewards/${
              rewardsPayload.rewards[0]!.rewardId}/claim`,
          headers: {'x-actor-id': 'training-player-1'},
        });

        expect(claim.statusCode).toBe(200);
        const claimPayload = claim.json() as {reward: {status: string}};
        expect(claimPayload.reward.status).toBe('claimed');

        await app.close();
      });

  test('rejects implausible high score via anti-cheat policy', async () => {
    const app = await createApp();

    const scaffold = await app.inject({
      method: 'POST',
      url: '/v1/player/training/jobs/scaffold',
      headers: {'x-actor-id': 'training-player-2'},
      payload: {
        jobs: [
          {
            title: 'Short-run exploit check',
            sector: 'integrity',
            rewardXp: 140
          },
        ],
      },
    });

    const jobId =
        (scaffold.json() as {jobs: Array<{jobId: string}>}).jobs[0]!.jobId;

    const execute = await app.inject({
      method: 'POST',
      url: `/v1/player/training/jobs/${jobId}/execute`,
      headers: {'x-actor-id': 'training-player-2'},
      payload: {
        score: 90_000,
        durationMs: 2_000,
        integrityProof: 'integrity-proof-abcdef-123456',
      },
    });

    expect(execute.statusCode).toBe(422);
    expect((execute.json() as {error: string}).error)
        .toBe('training_anti_cheat_rejected');

    await app.close();
  });

  test('records and lists training transition telemetry events', async () => {
    const app = await createApp();

    const ingest = await app.inject({
      method: 'POST',
      url: '/v1/player/training/telemetry/transitions',
      headers: {'x-actor-id': 'training-player-3'},
      payload: {
        eventType: 'queue.execution.completed',
        correlationId: 'queue-run-001',
        details: {
          completedJobs: 4,
          elapsedMs: 8277,
        },
      },
    });

    expect(ingest.statusCode).toBe(201);

    const events = await app.inject({
      method: 'GET',
      url: '/v1/player/training/telemetry/transitions?limit=10',
      headers: {'x-actor-id': 'training-player-3'},
    });

    expect(events.statusCode).toBe(200);
    const eventsPayload = events.json() as {
      events: Array<{eventType: string; correlationId: string}>;
    };
    expect(eventsPayload.events).toHaveLength(1);
    expect(eventsPayload.events[0]?.eventType)
        .toBe('queue.execution.completed');
    expect(eventsPayload.events[0]?.correlationId).toBe('queue-run-001');

    await app.close();
  });

  test('scaffolds template jobs and executes cycle server-side', async () => {
    const app = await createApp();

    const scaffold = await app.inject({
      method: 'POST',
      url: '/v1/player/training/jobs/scaffold-template',
      headers: {'x-actor-id': 'training-player-4'},
      payload: {
        mode: 'controller',
        selectedFile: 'src/native/engine/runtime/controller/combat.c',
        indexedFileCount: 144,
      },
    });

    expect(scaffold.statusCode).toBe(201);
    const scaffoldPayload = scaffold.json() as {jobs: Array<{jobId: string}>};
    expect(scaffoldPayload.jobs.length).toBeGreaterThan(0);

    const execute = await app.inject({
      method: 'POST',
      url: '/v1/player/training/jobs/execute-cycle',
      headers: {'x-actor-id': 'training-player-4'},
      payload: {
        mode: 'controller',
        selectedFile: 'src/native/engine/runtime/controller/combat.c',
        selectedLineCount: 220,
        indexedFileCount: 144,
      },
    });

    expect(execute.statusCode).toBe(200);
    const executePayload = execute.json() as {
      attemptedJobs: number;
      completedJobs: number;
      jobs: Array<{status: string}>;
      rewards: Array<{status: string}>;
      leaderboard: Array<{playerId: string}>;
    };
    expect(executePayload.attemptedJobs).toBeGreaterThan(0);
    expect(executePayload.completedJobs).toBe(executePayload.attemptedJobs);
    expect(executePayload.jobs.some((job) => job.status === 'completed'))
        .toBe(true);
    expect(executePayload.rewards.length).toBeGreaterThan(0);
    expect(executePayload.leaderboard.length).toBeGreaterThan(0);

    const events = await app.inject({
      method: 'GET',
      url: '/v1/player/training/telemetry/transitions?limit=20',
      headers: {'x-actor-id': 'training-player-4'},
    });
    expect(events.statusCode).toBe(200);
    const eventPayload = events.json() as {
      events: Array<{eventType: string}>;
    };
    const eventTypes = eventPayload.events.map((event) => event.eventType);
    expect(eventTypes.includes('queue.scaffolded')).toBe(true);
    expect(eventTypes.includes('queue.execution.started')).toBe(true);
    expect(eventTypes.includes('queue.execution.completed')).toBe(true);

    await app.close();
  });

  test(
      'claims pending rewards via claim-all endpoint with telemetry',
      async () => {
        const app = await createApp();

        const scaffold = await app.inject({
          method: 'POST',
          url: '/v1/player/training/jobs/scaffold-template',
          headers: {'x-actor-id': 'training-player-5'},
          payload: {
            mode: 'operations',
            selectedFile: 'src/native/engine/world/entity/controller.c',
            indexedFileCount: 80,
          },
        });
        expect(scaffold.statusCode).toBe(201);

        const execute = await app.inject({
          method: 'POST',
          url: '/v1/player/training/jobs/execute-cycle',
          headers: {'x-actor-id': 'training-player-5'},
          payload: {
            mode: 'operations',
            selectedFile: 'src/native/engine/world/entity/controller.c',
            selectedLineCount: 190,
            indexedFileCount: 80,
          },
        });
        expect(execute.statusCode).toBe(200);

        const claimAll = await app.inject({
          method: 'POST',
          url: '/v1/player/training/rewards/claim-all',
          headers: {'x-actor-id': 'training-player-5'},
        });
        expect(claimAll.statusCode).toBe(200);
        const claimAllPayload = claimAll.json() as {
          attemptedRewards: number;
          claimedRewards: Array<{status: string}>;
          failedRewards: Array<{error: string}>;
          rewards: Array<{status: string}>;
        };
        expect(claimAllPayload.attemptedRewards).toBeGreaterThan(0);
        expect(claimAllPayload.failedRewards).toHaveLength(0);
        expect(claimAllPayload.claimedRewards.length)
            .toBe(claimAllPayload.attemptedRewards);
        expect(claimAllPayload.rewards.every(
                   (reward) => reward.status === 'claimed'))
            .toBe(true);

        const events = await app.inject({
          method: 'GET',
          url: '/v1/player/training/telemetry/transitions?limit=50',
          headers: {'x-actor-id': 'training-player-5'},
        });
        expect(events.statusCode).toBe(200);
        const eventPayload = events.json() as {
          events: Array<{eventType: string}>;
        };
        const eventTypes = eventPayload.events.map((event) => event.eventType);
        expect(eventTypes.includes('reward.claim.attempted')).toBe(true);
        expect(eventTypes.includes('reward.claim.succeeded')).toBe(true);

        await app.close();
      });
});
