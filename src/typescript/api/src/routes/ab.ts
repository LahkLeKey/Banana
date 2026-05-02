import type { FastifyInstance } from 'fastify';
import { getAbStats } from '../plugins/ab-harness.ts';

export async function registerAbRoutes(app: FastifyInstance) {
  app.get('/operator/ab/stats', async (_req, reply) => {
    return reply.status(200).send(getAbStats());
  });
}
