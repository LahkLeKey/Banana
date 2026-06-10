import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

import {validationError} from '../../lib/errors/domainErrors.ts';
import {assertPlayerSelfScope} from '../../middleware/playerScopeAuthorization.ts';
import {getTrainingEconomyStore} from '../../services/trainingEconomyStore.ts';

const scaffoldJobsRequestSchema = z.object({
  jobs: z.array(z.object({
           title: z.string().trim().min(1).max(120),
           sector: z.string().trim().min(1).max(80),
           rewardXp: z.number().int().min(10).max(2500),
         }))
            .min(1)
            .max(50),
});

const completeJobRequestSchema = z.object({
  score: z.number().int().min(0).max(100000),
  durationMs: z.number().int().min(1).max(86400000),
  integrityProof: z.string().trim().min(12).max(256),
});

const transitionEventRequestSchema = z.object({
  eventType: z.enum([
    'queue.scaffolded',
    'queue.execution.started',
    'queue.execution.completed',
    'reward.claim.attempted',
    'reward.claim.succeeded',
    'reward.claim.failed',
  ]),
  correlationId: z.string().trim().min(6).max(128).optional(),
  details: z.record(z.string(), z.unknown()).optional(),
});

export async function registerV1PlayerTrainingEconomyRoutes(
    app: FastifyInstance): Promise<void> {
  const store = await getTrainingEconomyStore();

  app.post('/jobs/scaffold', async (request, reply) => {
    const parseResult = scaffoldJobsRequestSchema.safeParse(request.body ?? {});
    if (!parseResult.success) {
      throw validationError('training_jobs_scaffold_invalid_payload', {
        issues: parseResult.error.issues,
      });
    }

    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);

    const jobs = await store.scaffoldJobs(actorId, parseResult.data.jobs);
    reply.status(201).send({
      playerId: actorId,
      jobs,
    });
  });

  app.get('/jobs', async (request, reply) => {
    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);
    const jobs = await store.listJobs(actorId);
    reply.send({
      playerId: actorId,
      jobs,
    });
  });

  app.post('/jobs/:jobId/execute', async (request, reply) => {
    const bodyParse = completeJobRequestSchema.safeParse(request.body ?? {});
    if (!bodyParse.success) {
      throw validationError('training_job_execute_invalid_payload', {
        issues: bodyParse.error.issues,
      });
    }

    const paramsSchema = z.object({jobId: z.string().trim().uuid()});
    const paramsParse = paramsSchema.safeParse(request.params ?? {});
    if (!paramsParse.success) {
      throw validationError('training_job_execute_invalid_job_id');
    }

    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);

    try {
      const completed = await store.completeJob({
        playerId: actorId,
        jobId: paramsParse.data.jobId,
        score: bodyParse.data.score,
        durationMs: bodyParse.data.durationMs,
        integrityProof: bodyParse.data.integrityProof,
      });
      reply.send({
        playerId: actorId,
        antiCheatPassed: completed.antiCheatPassed,
        job: completed.job,
        reward: completed.reward,
      });
    } catch (error) {
      const code = error instanceof Error ? error.message :
                                            'training_job_execute_failed';
      const statusCode = code === 'training_job_not_found' ? 404 :
          code === 'training_anti_cheat_rejected'          ? 422 :
                                                             400;
      reply.status(statusCode).send({
        error: code,
      });
    }
  });

  app.get('/leaderboard', async (request, reply) => {
    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);

    const querySchema = z.object({
      limit: z.coerce.number().int().min(1).max(200).optional(),
    });
    const queryParse = querySchema.safeParse(request.query ?? {});
    if (!queryParse.success) {
      throw validationError('training_leaderboard_invalid_query');
    }

    const leaderboard =
        await store.listLeaderboard(queryParse.data.limit ?? 50);
    reply.send({
      leaderboard,
    });
  });

  app.get('/rewards', async (request, reply) => {
    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);
    const rewards = await store.listRewards(actorId);
    reply.send({
      playerId: actorId,
      rewards,
    });
  });

  app.post('/rewards/:rewardId/claim', async (request, reply) => {
    const paramsSchema = z.object({rewardId: z.string().trim().uuid()});
    const paramsParse = paramsSchema.safeParse(request.params ?? {});
    if (!paramsParse.success) {
      throw validationError('training_reward_claim_invalid_reward_id');
    }

    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);

    try {
      const reward =
          await store.claimReward(actorId, paramsParse.data.rewardId);
      reply.send({
        playerId: actorId,
        reward,
      });
    } catch (error) {
      const code = error instanceof Error ? error.message :
                                            'training_reward_claim_failed';
      const statusCode = code === 'reward_not_found' ? 404 :
          code === 'reward_claim_denied_policy'      ? 422 :
                                                       400;
      reply.status(statusCode).send({error: code});
    }
  });

  app.post('/telemetry/transitions', async (request, reply) => {
    const parseResult =
        transitionEventRequestSchema.safeParse(request.body ?? {});
    if (!parseResult.success) {
      throw validationError('training_transition_telemetry_invalid_payload', {
        issues: parseResult.error.issues,
      });
    }

    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);

    const event = await store.storeTransitionEvent({
      playerId: actorId,
      eventType: parseResult.data.eventType,
      correlationId: parseResult.data.correlationId ??
          request.requestContext.correlationId,
      details: parseResult.data.details ?? {},
    });
    reply.status(201).send({
      event,
    });
  });

  app.get('/telemetry/transitions', async (request, reply) => {
    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);

    const querySchema = z.object({
      limit: z.coerce.number().int().min(1).max(300).optional(),
    });
    const queryParse = querySchema.safeParse(request.query ?? {});
    if (!queryParse.success) {
      throw validationError('training_transition_telemetry_invalid_query');
    }

    const events =
        await store.listTransitionEvents(actorId, queryParse.data.limit ?? 30);
    reply.send({
      playerId: actorId,
      events,
    });
  });
}
