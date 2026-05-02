/**
 * audit-log.ts — Fastify plugin that records write operations to the
 * audit_event table via Prisma (spec #069).
 *
 * Hooks onResponse for POST/PUT/PATCH/DELETE requests only.
 * Actor is extracted from the JWT `sub` claim when present.
 */
import type {FastifyInstance, FastifyRequest} from 'fastify';
import {PrismaClient} from '@prisma/client';
import {verifyToken} from '../middleware/auth.ts';

const WRITE_METHODS = new Set(['POST', 'PUT', 'PATCH', 'DELETE']);

const prisma = new PrismaClient();

async function extractActor(request: FastifyRequest): Promise<string | null> {
  const auth = request.headers.authorization;
  if (!auth?.startsWith('Bearer ')) return null;
  try {
    const payload = await verifyToken(auth.slice(7));
    return payload.sub ?? null;
  } catch {
    return null;
  }
}

export async function registerAuditLogPlugin(app: FastifyInstance): Promise<void> {
  app.addHook('onResponse', async (request, reply) => {
    if (!WRITE_METHODS.has(request.method)) return;

    const actor = await extractActor(request);
    const routeUrl = request.routeOptions?.url ?? request.url;
    const resource = routeUrl.split('/').filter(Boolean)[0] ?? 'unknown';

    try {
      await prisma.audit_event.create({
        data: {
          actor,
          action: `${request.method} ${routeUrl}`,
          resource,
          ip_address:
            (request.headers['x-forwarded-for']?.toString() ?? request.ip) ||
            null,
          metadata: {
            status_code: reply.statusCode,
            url: request.url,
          },
        },
      });
    } catch (err) {
      app.log.warn({err}, 'audit-log: failed to write audit_event');
    }
  });
}
