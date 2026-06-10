import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetDomainChangeRecords} from '../../domains/gameplay-session-orchestration/persistence/domainChangeRecorder.ts';
import {registerFastifyErrorMapper} from '../../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../../middleware/requestContext.ts';
import {__resetTrainingEconomyStoreForTests} from '../../services/trainingEconomyStore.ts';

import {registerV1PlayerRoutes} from './player.ts';

async function createApp() {
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
});
