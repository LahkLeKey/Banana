import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

import {getRun, listAuditTrail, listRunHistory, recordPromotionAction, startTrainingRun} from './service.ts';

const StartRunBody = z.object({
  lane: z.enum(['left-brain', 'right-brain']),
  corpus_path: z.string().min(1).optional(),
  training_profile: z.enum(['ci', 'local', 'overnight', 'auto']).default('ci'),
  session_mode: z.enum(['single', 'incremental']).default('single'),
  max_sessions: z.coerce.number().int().min(1).max(32).default(1),
  operator_id: z.string().trim().min(2).max(64),
  notes: z.string().max(500).optional(),
});

const RunParams = z.object({
  runId: z.string().trim().min(1),
});

const HistoryQuery = z.object({
  lane: z.enum(['left-brain', 'right-brain']).optional(),
});

const PromoteBody = z.object({
  target: z.enum(['candidate', 'stable']),
  operator_id: z.string().trim().min(2).max(64),
  reason: z.string().max(500).optional(),
});

export async function registerTrainingWorkbenchRoutes(app: FastifyInstance) {
  app.post('/training/workbench/runs', async (req, reply) => {
    const parsed = StartRunBody.safeParse(req.body ?? {});
    if (!parsed.success) {
      return reply.status(400).send({
        error: 'invalid_argument',
        issues: parsed.error.flatten(),
      });
    }

    const run = await startTrainingRun(parsed.data);
    return reply.status(201).send({run});
  });

  app.get('/training/workbench/runs/:runId', async (req, reply) => {
    const parsed = RunParams.safeParse(req.params);
    if (!parsed.success) {
      return reply.status(400).send({
        error: 'invalid_argument',
        issues: parsed.error.flatten(),
      });
    }

    const run = getRun(parsed.data.runId);
    if (!run) {
      return reply.status(404).send({
        error: 'run_not_found',
        message: 'training run was not found',
      });
    }

    return reply.status(200).send({
      run,
      audit: listAuditTrail(run.run_id),
    });
  });

  app.get('/training/workbench/history', async (req, reply) => {
    const parsed = HistoryQuery.safeParse(req.query ?? {});
    if (!parsed.success) {
      return reply.status(400).send({
        error: 'invalid_argument',
        issues: parsed.error.flatten(),
      });
    }

    return reply.status(200).send({
      count: listRunHistory(parsed.data.lane).length,
      rows: listRunHistory(parsed.data.lane),
      audit: listAuditTrail(),
    });
  });

  app.post('/training/workbench/runs/:runId/promote', async (req, reply) => {
    const params = RunParams.safeParse(req.params);
    const body = PromoteBody.safeParse(req.body ?? {});
    if (!params.success || !body.success) {
      return reply.status(400).send({
        error: 'invalid_argument',
        issues: {
          params: params.success ? undefined : params.error.flatten(),
          body: body.success ? undefined : body.error.flatten(),
        },
      });
    }

    const run = getRun(params.data.runId);
    if (!run) {
      return reply.status(404).send({
        error: 'run_not_found',
        message: 'training run was not found',
      });
    }

    if (body.data.target === 'stable' && !run.threshold_passed) {
      return reply.status(409).send({
        error: 'promotion_blocked',
        message: 'only passed runs can be promoted to stable',
      });
    }

    const auditEvent = recordPromotionAction(
        run,
        body.data.operator_id,
        body.data.target,
        body.data.reason,
    );

    return reply.status(200).send({
      promoted: {
        run_id: run.run_id,
        target: body.data.target,
        threshold_passed: run.threshold_passed,
      },
      audit_event: auditEvent,
    });
  });
}
