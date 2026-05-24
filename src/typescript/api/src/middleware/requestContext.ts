import type {FastifyInstance, FastifyRequest} from 'fastify';
import {randomUUID} from 'node:crypto';

export type RequestActorScope = {
  actorId: string; actorType: 'player' | 'system' | 'unknown';
};

export type RequestContext = {
  correlationId: string; actorScope: RequestActorScope;
  idempotencyKey: string | null;
};

declare module 'fastify' {
  interface FastifyRequest {
    requestContext: RequestContext;
  }
}

function parseActorScope(request: FastifyRequest): RequestActorScope {
  const actorIdHeader = (request.headers['x-actor-id'] ?? '').toString().trim();
  if (actorIdHeader.length > 0) {
    return {actorId: actorIdHeader, actorType: 'player'};
  }

  const authHeader = (request.headers.authorization ?? '').toString();
  if (authHeader.startsWith('Bearer ')) {
    return {actorId: 'bearer-subject', actorType: 'player'};
  }

  return {actorId: 'anonymous', actorType: 'unknown'};
}

export function buildRequestContext(request: FastifyRequest): RequestContext {
  const correlationHeader =
      (request.headers['x-correlation-id'] ?? '').toString().trim();
  const idempotencyHeader =
      (request.headers['idempotency-key'] ?? '').toString().trim();

  return {
    correlationId: correlationHeader || randomUUID(),
    actorScope: parseActorScope(request),
    idempotencyKey: idempotencyHeader || null,
  };
}

export async function registerRequestContextMiddleware(app: FastifyInstance):
    Promise<void> {
  app.addHook('onRequest', async (request, _reply) => {
    request.requestContext = buildRequestContext(request);
  });
}
