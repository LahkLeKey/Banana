import {readStoredAuthSession} from '../auth/session';

import {GAME_SESSION_STORAGE_KEY, type GameSessionBootstrap, rejoinGameSession, resolveApiBaseResolutionError, resolveApiBaseUrl, resolveRuntimePlayerGuid, startGameSession,} from './api';

export type SessionBootstrapResult = {
  bootstrap: GameSessionBootstrap|null; error: string | null;
};

export type SessionBootstrapOptions = {
  playerName?: string;
};

export async function ensureGameSessionBootstrap(
    options: SessionBootstrapOptions = {}): Promise<SessionBootstrapResult> {
  const runtimePlayerGuid = resolveRuntimePlayerGuid();
  const authSession = readStoredAuthSession();

  try {
    const stored = sessionStorage.getItem(GAME_SESSION_STORAGE_KEY);
    if (stored) {
      const parsed = JSON.parse(stored) as GameSessionBootstrap;
      if (parsed.sessionId && parsed.token && parsed.playerName) {
        const normalized = {
          ...parsed,
          playerGuid: parsed.playerGuid || runtimePlayerGuid,
        };
        sessionStorage.setItem(
            GAME_SESSION_STORAGE_KEY, JSON.stringify(normalized));
        return {bootstrap: normalized, error: null};
      }
    }
  } catch {
    // Invalid cached bootstrap payloads are discarded.
  }

  sessionStorage.removeItem(GAME_SESSION_STORAGE_KEY);

  const resolutionError = resolveApiBaseResolutionError();
  if (resolutionError) {
    return {bootstrap: null, error: resolutionError};
  }

  const apiBaseUrl = resolveApiBaseUrl();
  if (!authSession?.token) {
    return {
      bootstrap: null,
      error: 'Steam login required. Sign in before launching the game client.',
    };
  }

  const requestedPlayerName = (options.playerName ?? '').trim();
  const fallbackPlayerName = requestedPlayerName ||
      (authSession.subject?.trim() ?
           `player-${authSession.subject.trim().slice(-4)}` :
           'player');

  try {
    const rejoined = await rejoinGameSession(
        apiBaseUrl, runtimePlayerGuid, authSession.token);
    const normalized = {
      ...rejoined,
      playerGuid: runtimePlayerGuid,
      playerName: rejoined.playerName || fallbackPlayerName,
    };
    sessionStorage.setItem(
        GAME_SESSION_STORAGE_KEY, JSON.stringify(normalized));
    return {bootstrap: normalized, error: null};
  } catch {
    // First-time clients will miss rejoin and fall back to session start.
  }

  try {
    const created = await startGameSession(
        apiBaseUrl, fallbackPlayerName, runtimePlayerGuid, authSession.token);
    const normalized = {
      ...created,
      playerGuid: runtimePlayerGuid,
      playerName: created.playerName || fallbackPlayerName,
    };
    sessionStorage.setItem(
        GAME_SESSION_STORAGE_KEY, JSON.stringify(normalized));
    return {bootstrap: normalized, error: null};
  } catch (cause) {
    return {
      bootstrap: null,
      error: cause instanceof Error ? cause.message : String(cause),
    };
  }
}