import {conflict} from '../../../lib/errors/domainErrors.ts';
import {markSessionTerminated} from '../state/sessionStore.ts';

export async function terminateSession(
    sessionId: string,
    reason: string,
    ): Promise<{
  terminationStatus: 'terminated' | 'already-terminated'; outcomeRef: string
}> {
  const marked = markSessionTerminated(sessionId, reason);
  if (!marked) {
    throw conflict('session_not_found');
  }

  return {
    terminationStatus: marked.alreadyTerminated ? 'already-terminated' :
                                                  'terminated',
    outcomeRef: marked.session.outcomeRef ?? `${sessionId}:outcome:0`,
  };
}
