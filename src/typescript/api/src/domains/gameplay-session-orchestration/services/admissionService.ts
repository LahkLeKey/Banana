import {createHash} from 'node:crypto';

import {validationError} from '../../../lib/errors/domainErrors.ts';
import {recordDomainChange} from '../persistence/domainChangeRecorder.ts';
import {getSessionForPlayer, upsertPlayerSession} from '../state/sessionStore.ts';

export type AdmissionInput = {
  playerId: string; queueIntent: string;
  actorScope: {actorId: string; actorType: string};
  correlationId: string;
};

export async function admitPlayerToSession(input: AdmissionInput): Promise<{
  sessionId: string; admissionStatus: 'admitted' | 'already-admitted';
  continuityWindowExpiresAt: string;
  continuityToken: string;
}> {
  if (input.actorScope.actorType === 'unknown') {
    throw validationError('actor_scope_required');
  }

  const existing = getSessionForPlayer(input.playerId);
  if (existing) {
    return {
      sessionId: existing.sessionId,
      admissionStatus: 'already-admitted',
      continuityWindowExpiresAt: existing.continuityWindowExpiresAt,
      continuityToken: existing.continuityToken,
    };
  }

  const sessionId = `sess_${
      createHash('sha1').update(input.playerId).digest('hex').slice(0, 12)}`;
  const continuityWindowExpiresAt =
      new Date(Date.now() + 15 * 60 * 1000).toISOString();
  const continuityToken = `ct_${
      createHash('sha1')
          .update(`${sessionId}:${input.queueIntent}`)
          .digest('hex')
          .slice(0, 16)}`;

  const created = upsertPlayerSession(
      sessionId, input.playerId, continuityToken, continuityWindowExpiresAt);
  recordDomainChange({
    boundedContext: 'gameplay-session-orchestration',
    actionType: 'session.admitted',
    actorScope: input.actorScope,
    beforeStateDigest: 'none',
    afterStateDigest: `${created.sessionId}:${created.state}`,
    correlationId: input.correlationId,
  });

  return {
    sessionId: created.sessionId,
    admissionStatus: 'admitted',
    continuityWindowExpiresAt: created.continuityWindowExpiresAt,
    continuityToken: created.continuityToken,
  };
}
