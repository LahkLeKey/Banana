import {GAME_SESSION_STORAGE_KEY, type GameSessionBootstrap, rejoinGameSession, resolveApiBaseResolutionError, resolveApiBaseUrl, resolveRuntimePlayerGuid, startGameSession,} from './api';

export type SessionBootstrapResult = {
  bootstrap: GameSessionBootstrap|null; error: string | null;
};

export async function ensureGameSessionBootstrap():
    Promise<SessionBootstrapResult> {
  const runtimePlayerGuid = resolveRuntimePlayerGuid();

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

  try {
    const rejoined = await rejoinGameSession(apiBaseUrl, runtimePlayerGuid);
    const normalized = {
      ...rejoined,
      playerGuid: runtimePlayerGuid,
      playerName: rejoined.playerName || 'player',
    };
    sessionStorage.setItem(
        GAME_SESSION_STORAGE_KEY, JSON.stringify(normalized));
    return {bootstrap: normalized, error: null};
  } catch {
    // First-time clients will miss rejoin and fall back to session start.
  }

  try {
    const created =
        await startGameSession(apiBaseUrl, 'player', runtimePlayerGuid);
    const normalized = {
      ...created,
      playerGuid: runtimePlayerGuid,
      playerName: created.playerName || 'player',
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