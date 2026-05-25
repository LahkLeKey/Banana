type ErrorPayload = {
  error?: {message?: string};
};

export const GAME_SESSION_STORAGE_KEY = 'banana-game-session';
export const GAME_PLAYER_GUID_STORAGE_KEY = 'banana-player-guid';
export const GAME_RUNTIME_PLAYER_GUID_STORAGE_KEY =
    'banana-runtime-player-guid';

export type GameSessionBootstrap = {
  sessionId: string; token: string; playerName: string; playerGuid: string;
  websocketPath?: string;
};

export type GameSessionPlayer = {
  playerId: string; playerName: string; connected: boolean; lastTick: number;
  lastSequence: number;
  controllerKind?: 'human' | 'ai';
};

export type GameSessionSnapshotEntity = {
  id: number; type: string; active: boolean; x: number; y: number; z: number;
  state: number;
  playerGuid?: string;
  controllerKind?: 'human' | 'ai';
  ownerPlayerId?: string;
  ownerPlayerName?: string;
  tooltipLabel?: string;
};

export type GameSessionSnapshot = {
  tick: number; timestamp: number; isAuthoritative: true;
  entities: Record<string, GameSessionSnapshotEntity>;
};

export type GameSessionServerMetrics = {
  targetTps: number; currentTps: number; averageTickMs: number; lagMs: number;
  lastTickAt: number | null;
};

export type GameSessionSnapshotEnvelope = {
  type: 'snapshot'; sessionId: string; snapshot: GameSessionSnapshot;
  players: GameSessionPlayer[];
  server?: GameSessionServerMetrics;
};

export type PlayerAccountResponse = {
  playerId: string;
  accountStatus: string;
  profile: Record<string, unknown>;
  version: number;
  updatedAt: string;
};

export type PlayerInsightsResponse = {
  playerId: string;
  sessionSummary: {changeEvents: number; latestAction: string | null;};
  progressionSummary: {xp: number; level: number;};
  inventoryTrendSummary: {distinctItems: number; totalQuantity: number;};
  noData: boolean;
  freshnessTimestamp: string;
};

export type GameAccountOverviewResponse = {
  steamId: string;
  playerGuid: string;
  sessionId: string;
  buildStats: {health: number; attack: number; defense: number; utility: number;};
  replayCursor: {lastTick: number; lastSequence: number;};
  antiCheat: {score: number; flagged: boolean; integrityHash: number;};
  world: {entityCount: number; lastSnapshotTick: number;};
  workflows: {totalEvents: number; recent: Array<Record<string, unknown>>;};
  mode: string;
};

type ElectronBridge = {
  apiBaseUrl: string;
};

export type ApiBaseResolution = {
  baseUrl: string; error: string | null;
  source: 'vite' | 'electron' | 'localhost-default' | 'unresolved';
};

function createRandomHex(size: number): string {
  const values = new Uint8Array(size);
  if (typeof globalThis !== 'undefined' && globalThis.crypto?.getRandomValues) {
    globalThis.crypto.getRandomValues(values);
  } else {
    for (let i = 0; i < size; i += 1) {
      values[i] = Math.floor(Math.random() * 256);
    }
  }

  return Array.from(values)
      .map((value) => value.toString(16).padStart(2, '0'))
      .join('');
}

export function createStableGuid(): string {
  if (typeof globalThis !== 'undefined' && globalThis.crypto?.randomUUID) {
    return globalThis.crypto.randomUUID();
  }

  const hex = createRandomHex(16);
  return `${hex.slice(0, 8)}-${hex.slice(8, 12)}-${hex.slice(12, 16)}-${
      hex.slice(16, 20)}-${hex.slice(20, 32)}`;
}

export function resolveStablePlayerGuid(): string {
  if (typeof window === 'undefined') {
    return createStableGuid();
  }

  const existing = window.localStorage.getItem(GAME_PLAYER_GUID_STORAGE_KEY);
  if (existing && existing.trim().length > 0) {
    return existing.trim();
  }

  const created = createStableGuid();
  window.localStorage.setItem(GAME_PLAYER_GUID_STORAGE_KEY, created);
  return created;
}

export function resolveRuntimePlayerGuid(): string {
  if (typeof window === 'undefined') {
    return resolveStablePlayerGuid();
  }

  const existing =
      window.sessionStorage.getItem(GAME_RUNTIME_PLAYER_GUID_STORAGE_KEY);
  if (existing && existing.trim().length > 0) {
    return existing.trim();
  }

  const created = createStableGuid();
  window.sessionStorage.setItem(GAME_RUNTIME_PLAYER_GUID_STORAGE_KEY, created);
  return created;
}

function resolveElectronBridge(): ElectronBridge|undefined {
  if (typeof window === 'undefined') {
    return undefined;
  }

  return (window as Window & {banana?: ElectronBridge}).banana;
}

function resolveViteApiBaseUrl(): string {
  const importMeta = import.meta as {env?: {VITE_BANANA_API_BASE_URL?: string}};
  return importMeta.env?.VITE_BANANA_API_BASE_URL ?? '';
}

function resolveLocalhostDefaultApiBaseUrl(): string {
  if (typeof window === 'undefined') {
    return '';
  }

  const host = window.location?.hostname?.trim().toLowerCase() ?? '';
  if (host === 'localhost' || host === '127.0.0.1') {
    return 'http://localhost:8080';
  }

  if (host === 'banana.engineer' || host === 'www.banana.engineer') {
    return 'https://api.banana.engineer';
  }

  return '';
}

export function resolveApiBaseResolution(
    viteBaseUrl = resolveViteApiBaseUrl(),
    electronBridge = resolveElectronBridge(),
    localhostDefault = resolveLocalhostDefaultApiBaseUrl()): ApiBaseResolution {
  const normalizedVite = viteBaseUrl.trim();
  if (normalizedVite.length > 0) {
    return {baseUrl: normalizedVite, error: null, source: 'vite'};
  }

  const normalizedElectron = (electronBridge?.apiBaseUrl ?? '').trim();
  if (normalizedElectron.length > 0) {
    return {baseUrl: normalizedElectron, error: null, source: 'electron'};
  }

  const normalizedLocalhostDefault = localhostDefault.trim();
  if (normalizedLocalhostDefault.length > 0) {
    return {
      baseUrl: normalizedLocalhostDefault,
      error: null,
      source: 'localhost-default',
    };
  }

  return {
    baseUrl: '',
    error:
        'Missing API base URL. Set VITE_BANANA_API_BASE_URL for web or BANANA_API_BASE_URL for Electron preload, then relaunch via the canonical Compose frontend profile.',
    source: 'unresolved',
  };
}

export function resolveApiBaseResolutionError(
    viteBaseUrl = resolveViteApiBaseUrl(),
    electronBridge = resolveElectronBridge(),
    localhostDefault = resolveLocalhostDefaultApiBaseUrl()): string|null {
  return resolveApiBaseResolution(viteBaseUrl, electronBridge, localhostDefault)
      .error;
}

export function resolveApiBaseUrl(
    viteBaseUrl = resolveViteApiBaseUrl(),
    electronBridge = resolveElectronBridge(),
    localhostDefault = resolveLocalhostDefaultApiBaseUrl()): string {
  return resolveApiBaseResolution(viteBaseUrl, electronBridge, localhostDefault)
      .baseUrl;
}

function resolveLocalhostFastifyGameplayBase(baseUrl: string): string {
  if (baseUrl.trim().length === 0) {
    return '';
  }

  try {
    const parsed = new URL(baseUrl);
    const host = parsed.hostname.trim().toLowerCase();
    if ((host === 'localhost' || host === '127.0.0.1') &&
        parsed.port === '8080') {
      parsed.port = '8081';
      return parsed.toString().replace(/\/$/, '');
    }
  } catch {
    return '';
  }

  return '';
}

export function resolveGameplayApiBaseUrl(apiBaseUrl: string): string {
  const localhostFastify = resolveLocalhostFastifyGameplayBase(apiBaseUrl);
  if (localhostFastify.length > 0) {
    return localhostFastify;
  }

  return apiBaseUrl;
}

export function resolveGameSessionWebSocketUrl(
    apiBaseUrl: string, sessionId: string, playerName: string,
    playerGuid: string): string {
  const gameplayBaseUrl = resolveGameplayApiBaseUrl(apiBaseUrl);
  const parsed = new URL(gameplayBaseUrl);
  parsed.protocol = parsed.protocol === 'https:' ? 'wss:' : 'ws:';
  parsed.pathname = '/game-session';
  parsed.searchParams.set('sessionId', sessionId);
  parsed.searchParams.set('playerName', playerName);
  parsed.searchParams.set('playerGuid', playerGuid);
  return parsed.toString();
}

async function parseApiError(response: Response): Promise<string> {
  try {
    const payload = (await response.json()) as ErrorPayload;
    return payload.error?.message ?? `request failed (${response.status})`;
  } catch {
    return `request failed (${response.status})`;
  }
}

async function requestJson<T>(
    baseUrl: string, path: string, init?: RequestInit): Promise<T> {
  const response = await fetch(`${baseUrl}${path}`, init);
  if (!response.ok) {
    throw new Error(await parseApiError(response));
  }
  return (await response.json()) as T;
}

function withBearerAuth(token: string): Record<string, string> {
  return {authorization: `Bearer ${token}`};
}

export async function startGameSession(
    baseUrl: string, playerName: string,
    playerGuid: string, token?: string): Promise<GameSessionBootstrap> {
  const headers: Record<string, string> = {'content-type': 'application/json'};
  if (token?.trim()) {
    headers.authorization = `Bearer ${token.trim()}`;
  }

  return requestJson<GameSessionBootstrap>(
      resolveGameplayApiBaseUrl(baseUrl), '/api/game/session/start', {
        method: 'POST',
        headers,
        body: JSON.stringify({playerName, playerGuid}),
      });
}

export async function rejoinGameSession(
    baseUrl: string, playerGuid: string,
    token?: string): Promise<GameSessionBootstrap> {
  const headers: Record<string, string> = {'content-type': 'application/json'};
  if (token?.trim()) {
    headers.authorization = `Bearer ${token.trim()}`;
  }

  const response =
      await requestJson<GameSessionBootstrap&{playerName?: string}>(
          resolveGameplayApiBaseUrl(baseUrl), '/api/game/session/rejoin', {
            method: 'POST',
            headers,
            body: JSON.stringify({playerGuid}),
          });

  return {
    sessionId: response.sessionId,
    token: response.token,
    playerName: response.playerName ?? 'player',
    playerGuid,
  };
}

export async function fetchPlayerAccount(
    baseUrl: string, token: string): Promise<PlayerAccountResponse> {
  return requestJson<PlayerAccountResponse>(baseUrl, '/v1/player/account', {
    method: 'GET',
    headers: withBearerAuth(token),
  });
}

export async function updatePlayerAccountProfile(
    baseUrl: string, token: string, expectedVersion: number,
    profilePatch: Record<string, unknown>): Promise<PlayerAccountResponse> {
  return requestJson<PlayerAccountResponse>(baseUrl, '/v1/player/account', {
    method: 'PATCH',
    headers: {
      'content-type': 'application/json',
      ...withBearerAuth(token),
    },
    body: JSON.stringify({expectedVersion, profilePatch}),
  });
}

export async function fetchPlayerInsights(
    baseUrl: string, token: string): Promise<PlayerInsightsResponse> {
  return requestJson<PlayerInsightsResponse>(baseUrl, '/v1/player/insights', {
    method: 'GET',
    headers: withBearerAuth(token),
  });
}

export async function fetchGameAccountOverview(
    baseUrl: string, token: string): Promise<GameAccountOverviewResponse> {
  return requestJson<GameAccountOverviewResponse>(
      resolveGameplayApiBaseUrl(baseUrl), '/api/game/account/overview', {
        method: 'GET',
        headers: withBearerAuth(token),
      });
}