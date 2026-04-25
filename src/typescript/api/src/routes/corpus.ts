import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

// Per spec 008 route-ownership: TypeScript-only surface lives here.
const FeedbackEntry = z.object({
  text: z.string().min(1),
  label: z.enum(['banana', 'not-banana']),
  source: z.string().default('user'),
});

export async function registerCorpusRoutes(app: FastifyInstance) {
  app.post('/corpus/feedback', async (req, reply) => {
    const parsed = FeedbackEntry.safeParse(req.body);
    if (!parsed.success)
      return reply.status(400).send(
          {error: 'invalid_argument', issues: parsed.error.flatten()});
    // TODO(008): persist via Prisma (ApiCall + domain table) once migrations
    // land.
    return reply.status(202).send({accepted: parsed.data});
  });
}
