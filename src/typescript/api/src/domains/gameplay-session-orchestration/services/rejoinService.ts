import {getSessionById} from '../state/sessionStore.ts';

export async function restoreSessionContinuity(
    sessionId: string,
    playerId: string,
    continuityToken: string,
    ): Promise<{
  rejoinStatus: 'restored' | 'expired' | 'not-found';
  restoredSnapshotRef: string
}> {
  const session = getSessionById(sessionId);
  if (!session || !session.playerIds.has(playerId)) {
    return {rejoinStatus: 'not-found', restoredSnapshotRef: 'none'};
  }

  if (Date.parse(session.continuityWindowExpiresAt) < Date.now() ||
      continuityToken !== session.continuityToken) {
    return {rejoinStatus: 'expired', restoredSnapshotRef: 'none'};
  }

  return {
    rejoinStatus: 'restored',
    restoredSnapshotRef: session.snapshotRef,
  };
}
