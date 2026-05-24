import {conflict, validationError} from '../../../lib/errors/domainErrors.ts';
import {applyGameplayCommandMutation} from '../../progression-inventory/state/playerTruthStore.ts';
import {recordDomainChange} from '../persistence/domainChangeRecorder.ts';
import {appendSessionCommand, getSessionById} from '../state/sessionStore.ts';

export type CommandInput = {
  sessionId: string; playerId: string; commandType: string;
  commandPayload: Record<string, unknown>;
  commandSequence: number;
  actorScope: {actorId: string; actorType: string};
  correlationId: string;
};

export async function runAuthoritativeCommand(input: CommandInput): Promise<{
  authoritativeTick: number; resolutionCode: string; conflictHandled: boolean;
}> {
  if (input.actorScope.actorType === 'unknown') {
    throw validationError('actor_scope_required');
  }

  const before = getSessionById(input.sessionId);
  if (!before) {
    throw conflict('session_not_found');
  }
  if (!before.playerIds.has(input.playerId)) {
    throw conflict('player_not_admitted');
  }

  const updated = appendSessionCommand(input.sessionId, {
    playerId: input.playerId,
    commandType: input.commandType,
    commandPayload: input.commandPayload,
    commandSequence: input.commandSequence,
  });

  if (!updated) {
    throw conflict('session_not_found');
  }

  const conflictHandled =
      input.commandSequence <= updated.authoritativeTick - 1;

  applyGameplayCommandMutation(input.playerId, input.commandPayload);

  recordDomainChange({
    boundedContext: 'gameplay-session-orchestration',
    actionType: 'session.state-updated',
    actorScope: input.actorScope,
    beforeStateDigest: `${input.sessionId}:${before.authoritativeTick}`,
    afterStateDigest: `${updated.sessionId}:${updated.authoritativeTick}`,
    correlationId: input.correlationId,
  });

  return {
    authoritativeTick: updated.authoritativeTick,
    resolutionCode: conflictHandled ? 'accepted_conflict_resolved' : 'accepted',
    conflictHandled,
  };
}
