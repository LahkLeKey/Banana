type SessionCommand = {
  playerId: string; commandType: string;
  commandPayload: Record<string, unknown>;
  commandSequence: number;
  authoritativeTick: number;
  createdAt: number;
};

export type GameplaySession = {
  sessionId: string; playerIds: Set<string>;
  state: 'active' | 'recovery-window' | 'completed' | 'aborted';
  continuityWindowExpiresAt: string;
  continuityToken: string;
  authoritativeTick: number;
  commands: SessionCommand[];
  outcomeRef: string | null;
  snapshotRef: string;
};

const sessions = new Map<string, GameplaySession>();
const playerSessionIndex = new Map<string, string>();

export function resetGameplaySessionStore(): void {
  sessions.clear();
  playerSessionIndex.clear();
}

export function getSessionById(sessionId: string): GameplaySession|null {
  return sessions.get(sessionId) ?? null;
}

export function getSessionForPlayer(playerId: string): GameplaySession|null {
  const sessionId = playerSessionIndex.get(playerId);
  if (!sessionId) {
    return null;
  }
  return sessions.get(sessionId) ?? null;
}

export function upsertPlayerSession(
    sessionId: string,
    playerId: string,
    continuityToken: string,
    continuityWindowExpiresAt: string,
    ): GameplaySession {
  const existing = sessions.get(sessionId);
  if (existing) {
    existing.playerIds.add(playerId);
    existing.continuityToken = continuityToken;
    existing.continuityWindowExpiresAt = continuityWindowExpiresAt;
    playerSessionIndex.set(playerId, sessionId);
    return existing;
  }

  const created: GameplaySession = {
    sessionId,
    playerIds: new Set([playerId]),
    state: 'active',
    continuityWindowExpiresAt,
    continuityToken,
    authoritativeTick: 0,
    commands: [],
    outcomeRef: null,
    snapshotRef: `${sessionId}:snapshot:0`,
  };
  sessions.set(sessionId, created);
  playerSessionIndex.set(playerId, sessionId);
  return created;
}

export function appendSessionCommand(
    sessionId: string,
    command: Omit<SessionCommand, 'authoritativeTick'|'createdAt'>,
    ): GameplaySession|null {
  const session = sessions.get(sessionId);
  if (!session) {
    return null;
  }

  session.authoritativeTick += 1;
  session.snapshotRef = `${sessionId}:snapshot:${session.authoritativeTick}`;
  session.commands.push({
    ...command,
    authoritativeTick: session.authoritativeTick,
    createdAt: Date.now(),
  });
  return session;
}

export function markSessionTerminated(
    sessionId: string,
    reason: string,
    ): {session: GameplaySession; alreadyTerminated: boolean;}|null {
  const session = sessions.get(sessionId);
  if (!session) {
    return null;
  }

  if (session.state === 'completed' || session.state === 'aborted') {
    return {session, alreadyTerminated: true};
  }

  session.state = reason === 'aborted' ? 'aborted' : 'completed';
  session.outcomeRef = `${sessionId}:outcome:${session.authoritativeTick}`;
  return {session, alreadyTerminated: false};
}
