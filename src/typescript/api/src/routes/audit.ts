/**
 * audit.ts — Operator-gated audit event read endpoint (spec #069).
 *
 * GET /operator/audit?action=&actor=&limit=50&cursor=
 * Returns { events: AuditEvent[], next_cursor: string | null }
 */
import type {FastifyInstance} from 'fastify';
import {PrismaClient} from '@prisma/client';
import {requireRole} from '../middleware/auth.ts';

const prisma = new PrismaClient();

export async function registerAuditRoutes(app: FastifyInstance): Promise<void> {
  app.get(
    '/operator/audit',
    {preHandler: requireRole('operator')},
    async (request, reply) => {
      const query = request.query as {
        action?: string;
        actor?: string;
        limit?: string;
        cursor?: string;
      };

      const limit = Math.min(Number(query.limit ?? 50), 200);
      const cursor = query.cursor ?? undefined;

      const where: Record<string, unknown> = {};
      if (query.action) where.action = {contains: query.action};
      if (query.actor) where.actor = query.actor;
      if (cursor) where.id = {gt: cursor};

      const events = await prisma.audit_event.findMany({
        where,
        orderBy: {created_at: 'desc'},
        take: limit + 1,
      });

      const hasMore = events.length > limit;
      const page = hasMore ? events.slice(0, limit) : events;
      const next_cursor = hasMore ? page[page.length - 1]?.id ?? null : null;

      return reply.send({events: page, next_cursor});
    },
  );
}
