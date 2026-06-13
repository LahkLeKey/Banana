import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

import {validationError} from '../../lib/errors/domainErrors.ts';
import {assertPlayerSelfScope} from '../../middleware/playerScopeAuthorization.ts';
import {getTrainingEconomyStore} from '../../services/trainingEconomyStore.ts';

type TrainingTemplateMode = 'operations'|'controller';

const scaffoldJobsRequestSchema = z.object({
  jobs: z.array(z.object({
           title: z.string().trim().min(1).max(120),
           sector: z.string().trim().min(1).max(80),
           rewardXp: z.number().int().min(10).max(2500),
         }))
            .min(1)
            .max(50),
});

const scaffoldTemplateRequestSchema = z.object({
  mode: z.enum(['operations', 'controller']),
  selectedFile: z.string().trim().max(260).optional(),
  indexedFileCount: z.number().int().min(0).max(200000).optional(),
});

const executeCycleRequestSchema = z.object({
  mode: z.enum(['operations', 'controller']),
  selectedFile: z.string().trim().max(260).optional(),
  selectedLineCount: z.number().int().min(1).max(200000),
  indexedFileCount: z.number().int().min(0).max(200000).optional(),
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

function buildBulkQueueTemplate(
    selectedFile: string,
    indexedFileCount: number,
    mode: TrainingTemplateMode): Array<{
  title: string;
  sector: string;
  rewardXp: number;
}> {
  const normalized = selectedFile.replace(/\\/g, '/');
  const lane = normalized.split('/').filter(Boolean).pop() ?? selectedFile;
  const sectorLabel = lane.replace(/\.[^.]+$/, '')
                          .replace(/[_-]+/g, ' ')
                          .replace(/\b\w/g, (match) => match.toUpperCase());
  const targetLabel = sectorLabel.length > 0 ? sectorLabel : 'Frontier Sector';

  if (mode === 'controller') {
    const controllerSector = 'controller-training';
    const controllerScale =
        Math.max(1, Math.min(4, Math.floor(indexedFileCount / 40) + 1));
    return [
      {
        title: `Calibrate controller reward gradients in ${targetLabel}`,
        sector: controllerSector,
        rewardXp: 140 + controllerScale * 20,
      },
      {
        title: 'Run imitation pass for AI controller tuning',
        sector: controllerSector,
        rewardXp: 190 + controllerScale * 20,
      },
      {
        title:
            `Validate controller policies against ${indexedFileCount} indexed files`,
        sector: controllerSector,
        rewardXp: 260 + controllerScale * 30,
      },
      {
        title: 'Archive controller training telemetry and issue stipend',
        sector: controllerSector,
        rewardXp: 160 + controllerScale * 15,
      },
    ];
  }

  const sector = selectedFile.length > 0 ? selectedFile : 'default-sector';
  return [
    {
      title: `Survey frontier output in ${targetLabel}`,
      sector,
      rewardXp: 120,
    },
    {
      title: 'Allocate worker crews and supplies',
      sector,
      rewardXp: 180,
    },
    {
      title: 'Run production cycle for this province',
      sector,
      rewardXp: 260,
    },
    {
      title: 'Audit yields and issue stipend payout',
      sector,
      rewardXp: 140,
    },
  ];
}

function buildExecutionInput(args: {
  selectedFile: string;
  selectedLineCount: number;
  indexedFileCount: number;
  index: number;
  jobId: string;
}): {score: number; durationMs: number; integrityProof: string} {
  const durationMs =
      5_000 + args.selectedLineCount * 25 + args.index * 300;
  const scoreCap = Math.floor(durationMs / 5) + 4_500;
  const score = Math.max(
      600,
      Math.min(
          scoreCap,
          args.selectedLineCount * 28 + args.indexedFileCount + args.index * 120,
          ),
  );
  const integritySector = args.selectedFile.trim().length > 0 ?
      args.selectedFile.trim() :
      'default';
  const integrityProof =
      `${integritySector}:${args.jobId}:${durationMs}:${score}`;
  return {score, durationMs, integrityProof};
}

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

  app.post('/jobs/scaffold-template', async (request, reply) => {
    const parseResult =
        scaffoldTemplateRequestSchema.safeParse(request.body ?? {});
    if (!parseResult.success) {
      throw validationError('training_jobs_scaffold_template_invalid_payload', {
        issues: parseResult.error.issues,
      });
    }

    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);

    const selectedFile = parseResult.data.selectedFile?.trim() ?? '';
    const indexedFileCount = parseResult.data.indexedFileCount ?? 0;
    const jobs = await store.scaffoldJobs(
        actorId,
        buildBulkQueueTemplate(
            selectedFile, indexedFileCount, parseResult.data.mode),
    );

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

  app.post('/jobs/execute-cycle', async (request, reply) => {
    const parseResult = executeCycleRequestSchema.safeParse(request.body ?? {});
    if (!parseResult.success) {
      throw validationError('training_jobs_execute_cycle_invalid_payload', {
        issues: parseResult.error.issues,
      });
    }

    const actorId = request.requestContext.actorScope.actorId;
    assertPlayerSelfScope(request, actorId);

    const selectedFile = parseResult.data.selectedFile?.trim() ?? '';
    const indexedFileCount = parseResult.data.indexedFileCount ?? 0;
    const selectedLineCount = parseResult.data.selectedLineCount;

    let queue = await store.listJobs(actorId);
    let pending = queue.filter((job) => job.status !== 'completed');
    if (pending.length === 0) {
      pending = await store.scaffoldJobs(
          actorId,
          buildBulkQueueTemplate(
              selectedFile, indexedFileCount, parseResult.data.mode),
      );
    }

    const completedJobIds: string[] = [];
    for (let index = 0; index < pending.length; index += 1) {
      const job = pending[index];
      if (!job || job.status === 'completed') {
        continue;
      }

      const execution = buildExecutionInput({
        selectedFile,
        selectedLineCount,
        indexedFileCount,
        index,
        jobId: job.jobId,
      });

      await store.completeJob({
        playerId: actorId,
        jobId: job.jobId,
        score: execution.score,
        durationMs: execution.durationMs,
        integrityProof: execution.integrityProof,
      });
      completedJobIds.push(job.jobId);
    }

    queue = await store.listJobs(actorId);
    const leaderboard = await store.listLeaderboard(12);
    const rewards = await store.listRewards(actorId);

    reply.send({
      playerId: actorId,
      attemptedJobs: pending.length,
      completedJobs: completedJobIds.length,
      mode: parseResult.data.mode,
      jobs: queue,
      leaderboard,
      rewards,
    });
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
