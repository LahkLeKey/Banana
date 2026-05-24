import type {FastifyRequest} from 'fastify';

import {authenticationRequired, forbidden} from '../lib/errors/domainErrors.ts';

export function assertPlayerSelfScope(
    request: FastifyRequest,
    targetPlayerId: string,
    ): void {
  const actorScope = request.requestContext.actorScope;
  if (actorScope.actorType !== 'player') {
    throw authenticationRequired('player_authentication_required');
  }

  if (actorScope.actorId !== targetPlayerId) {
    throw forbidden('player_scope_forbidden');
  }
}
