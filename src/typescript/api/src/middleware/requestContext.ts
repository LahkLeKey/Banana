import type {FastifyInstance, FastifyRequest} from 'fastify';
import {randomUUID} from 'node:crypto';

import {verifyToken} from './auth.ts';

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

function parseBearerToken(authHeader: string): string|null {
  if (!authHeader.startsWith('Bearer ')) {
    return null;
  }

  const token = authHeader.slice(7).trim();
  return token.length > 0 ? token : null;
}

function resolveActorIdFromSubject(subject: string): string {
  if (subject.startsWith('steam:')) {
    return subject.slice(6);
  }

  return subject;
}

async function parseActorScope(request: FastifyRequest):
    Promise<RequestActorScope> {
  const actorIdHeader = (request.headers['x-actor-id'] ?? '').toString().trim();
  if (actorIdHeader.length > 0) {
    return {actorId: actorIdHeader, actorType: 'player'};
  }

  const authHeader = (request.headers.authorization ?? '').toString();
  const bearerToken = parseBearerToken(authHeader);
  if (bearerToken) {
    try {
      const claims = await verifyToken(bearerToken);
      const actorId = resolveActorIdFromSubject(claims.sub);
      if (actorId.length > 0) {
        return {actorId, actorType: 'player'};
      }
    } catch {
      return {actorId: 'anonymous', actorType: 'unknown'};
    }
  }

  return {actorId: 'anonymous', actorType: 'unknown'};
}

export async function buildRequestContext(request: FastifyRequest):
    Promise<RequestContext> {
  const correlationHeader =
      (request.headers['x-correlation-id'] ?? '').toString().trim();
  const idempotencyHeader =
      (request.headers['idempotency-key'] ?? '').toString().trim();

  return {
    correlationId: correlationHeader || randomUUID(),
    actorScope: await parseActorScope(request),
    idempotencyKey: idempotencyHeader || null,
  };
}

export async function registerRequestContextMiddleware(app: FastifyInstance):
    Promise<void> {
  app.addHook('onRequest', async (request, _reply) => {
    request.requestContext = await buildRequestContext(request);
  });
}
