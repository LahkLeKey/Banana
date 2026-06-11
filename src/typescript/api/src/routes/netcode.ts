import type {FastifyInstance} from 'fastify';

import {getNativeNetcodeService} from '../services/nativeNetcode.ts';

export async function registerNetcodeRoutes(app: FastifyInstance):
    Promise<void> {
  const netcode = getNativeNetcodeService();

  app.post<{
    Body: {
      callDensity: number; questPercent: number; comboStreak: number;
      branchPressure: number;
      workflowDepth: 1 | 2 | 3;
      nodeTap?: number;
      action?: number;
    }
  }>(
      '/api/netcode/learning',
      async (request, reply) => {
        const body = request.body;
        if (!body || typeof body.callDensity !== 'number' ||
            typeof body.questPercent !== 'number' ||
            typeof body.comboStreak !== 'number' ||
            typeof body.branchPressure !== 'number' ||
            (body.workflowDepth !== 1 && body.workflowDepth !== 2 &&
             body.workflowDepth !== 3)) {
          return reply.status(400).send({
            error: 'Invalid netcode learning payload',
          });
        }

        if (typeof body.nodeTap === 'number') {
          await netcode.recordNodeTap(body.nodeTap);
        }
        if (typeof body.action === 'number') {
          await netcode.recordAction(body.action);
        }

        const ledger = await netcode.getLedger();
        const learning = await netcode.buildLearning({
          callDensity: body.callDensity,
          questPercent: body.questPercent,
          comboStreak: body.comboStreak,
          branchPressure: body.branchPressure,
          workflowDepth: body.workflowDepth,
        });

        return {
          ledger,
          learning,
        };
      },
  );

  app.post('/api/netcode/reset', async () => {
    await netcode.reset();
    return {status: 'ok'};
  });
}
