type ErrorPayload = {
  error?: {message?: string};
};

import type {LaunchGateReasonCode, LaunchGateStatusResponse, LaunchGateVerifyResponse} from './launchGateTypes';
import type {K3h4EpochGeometryResponse} from './k3h4GeometryTypes.ts';
import type {NetcodeAbiLayerCoverage, NetcodeAbiLayerKind, NetcodeAbiLayerLedger, NetcodeAbiLayerSnapshot} from '@banana/ui';

type BananaSummaryResponse = {
  banana: number;
};

export type {BananaSummaryResponse};

export type TelemetryConfigResponse = {
  sample_rate: number; unit: string;
};

export type ApiHealthResponse = {
  status: string;
};

export type ApiRootResponse = {
  service?: string;
  status?: string;
  endpoints?: {health?: string; swagger?: string};
};

export type NetcodeAnalyticsRequest = {
  callDensity: number; questPercent: number; playerLevel: number;
  comboStreak: number;
  branchPressure: number;
  dependencyPulse: number;
  workflowDepth: 1 | 2 | 3;
  networkDimensions: number;
  modelConfidence: number;
  policyMomentum: number;
  interactionSignal?: number;
  k3h4Mode?: 'multiplicative' | 'power';
  spectralMode?: 'disabled' | 'affinity-graph';
};

export type NetcodeLearningRequest = {
  callDensity: number; questPercent: number; comboStreak: number;
  branchPressure: number;
  workflowDepth: 1 | 2 | 3;
  nodeTap?: number;
  action?: number;
};

export type NetcodeLearningResponse = {
  ledger: {
    snapshots: number; inspections: number; trainings: number; routes: number;
    nodeTaps: number;
  };
  learning: {
    modelConfidence: number; trainingAccuracy: number; policyMomentum: number;
    nodeWeights: readonly[number, number, number, number];
    recommendedNode: number;
    recommendedAction: number;
    xpByAction: readonly[number, number, number, number];
  };
};

export type NetcodeK3h4Center = {
  clusterId: number; centerQ16: readonly number[];
  memberVectorIds: readonly number[];
  memberCount: number;
};

export type NetcodeK3h4Radius = {
  clusterId: number; nearestNeighborDistanceQ16: number;
  inscribedRadiusQ16: number;
  radiusState: 'ok' | 'degenerate' | 'invalid';
};

export type NetcodeK3h4WeightedScore = {
  vectorId: number; clusterId: number; distanceToCenterQ16: number;
  weightedScoreQ16: number;
  scoreValidity: 'valid' | 'clamped' | 'invalid';
};

export type NetcodeK3h4SpectralProxy = {
  clusterId: number; frequencyProxyQ16: number; amplitudeProxyQ16: number;
  spectralState: 'ok' | 'low-signal' | 'invalid';
};

export type NetcodeK3h4Observability = {
  convergenceStatus: 'converged'|'max-iterations'|'failed';
  iterationCount: number;
  assignmentChangesLastIteration: number;
  scoringValidity: 'valid' | 'degraded' | 'invalid';
  deterministicHash: string;
};

export type NetcodeK3h4Payload = {
  centers: readonly NetcodeK3h4Center[]; radii: readonly NetcodeK3h4Radius[];
  weightedVoronoiScores: readonly NetcodeK3h4WeightedScore[];
  spectralProxy: readonly NetcodeK3h4SpectralProxy[];
  observability: NetcodeK3h4Observability;
};

export type NetcodeAnalyticsRollout = {
  enabled: boolean; cohort: string;
};

export type NetcodeAnalyticsAbiLayerKind = NetcodeAbiLayerKind;

export type NetcodeAnalyticsAbiLayerSnapshot = NetcodeAbiLayerSnapshot;

export type NetcodeAnalyticsAbiLayerCoverage = NetcodeAbiLayerCoverage;

export type NetcodeAnalyticsAbiLayerLedger = NetcodeAbiLayerLedger;

export type NetcodeAnalyticsLspRepresentation = {
  language: 'netcode.analytics.v1'; boundedContext: 'netcode';
  aggregate: 'k3h4';
  authority: 'server-native';
  contractVersion: 1;
  deterministicHash: number;
  rollout: NetcodeAnalyticsRollout;
};

export type NetcodeAnalyticsResponse = {
  contractVersion: number;
  lspRepresentation?: NetcodeAnalyticsLspRepresentation; reward: {
    neuralRelevanceScore: number; projectedRewardXp: number; rewardTier: number;
  };
  link: {intel: number; objectives: number; player: number; ops: number;};
  vector: {
    dimensions: number; nodeVectors: readonly(readonly number[])[];
    contractStrength: readonly[number, number, number, number];
  };
  k3h4Projection: {
    dimensions: number; nodes: readonly {
      x: number;
      y: number;
      z: number;
      coherence: number;
      inradius: number;
      nearestNeighborDistance: number;
    }[];
    alignment: number;
    radialStability: number;
  };
  k3h4: NetcodeK3h4Payload;
  k3h4Runtime: {
    mode: 'multiplicative' | 'power';
    spectralActivation: 'disabled' | 'affinity-graph';
  };
  abiLayers: readonly NetcodeAnalyticsAbiLayerSnapshot[];
  abiLayerCoverage: NetcodeAnalyticsAbiLayerCoverage;
  abiLayerLedger: NetcodeAnalyticsAbiLayerLedger;
  rollout: NetcodeAnalyticsRollout;
};

export type NetcodeAnalyticsErrorResponse = {
  error?: string;
  errorCode?: string;
  message?: string;
  contractVersion?: number;
  retryable?: boolean;
  rollout?: NetcodeAnalyticsRollout;
};

export class NetcodeAnalyticsError extends Error {
  readonly status: number;
  readonly payload: NetcodeAnalyticsErrorResponse;

  constructor(status: number, payload: NetcodeAnalyticsErrorResponse) {
    super(
        payload.message ?? payload.error ?? `request failed (${status})`,
    );
    this.name = 'NetcodeAnalyticsError';
    this.status = status;
    this.payload = payload;
  }
};

export type GuestAuthStartResponse = {
  token: string; guestId: string; displayName: string; expiresAt: string;
};

export type PlayerAccountResponse = {
  playerId: string; accountStatus: string; profile: Record<string, unknown>;
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

export type TrainingJob = {
  jobId: string; playerId: string; title: string; sector: string;
  status: 'queued' | 'running' | 'completed' | 'failed';
  rewardXp: number;
  score: number;
  durationMs: number;
  antiCheatVerdict: 'pending' | 'passed' | 'rejected';
  createdAt: string;
  updatedAt: string;
};

export type TrainingReward = {
  rewardId: string; playerId: string; sourceJobId: string; xp: number;
  status: 'pending' | 'claimed';
  createdAt: string;
  claimedAt: string | null;
};

export type TrainingLeaderboardEntry = {
  playerId: string; totalScore: number; totalXp: number; completedJobs: number;
  updatedAt: string;
};

export type TrainingTransitionEvent = {
  eventId: string; playerId: string; eventType: string; correlationId: string;
  details: Record<string, unknown>;
  createdAt: string;
};

export type TrainingWorkflowMode = 'operations'|'controller';

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

export type GameplayBuildStats = {
  health: number; attack: number; defense: number; utility: number;
};

export type GameplayComboResult = {
  triggered: boolean; damageBonusPct: number; mitigationBonusPct: number;
  partySynergyBonusPct: number;
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

export type TelemetryApiEvent = {
  source: string; event: string; timestamp: number; status: string;
  durationMs?: number;
  code?: number;
  channel?: string;
  variant?: string;
  layer?: string;
  details?: Record<string, string|number|boolean|null>;
};

export type TelemetryEventsResponse = {
  count: number; backend: 'postgres' | 'memory'; events: TelemetryApiEvent[];
};

export type ComponentErrorTelemetryPayload = {
  componentName: string; message: string;
  stack?: string;
  componentStack?: string;
  routePath?: string;
  userAgent?: string; timestamp: number;
  metadata?: Record<string, unknown>;
};

export type ApiBaseResolution = {
  baseUrl: string; error: string | null;
  source: 'vite' | 'localhost-default' | 'unresolved';
};

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
    return 'http://localhost:8081';
  }

  if (host === 'banana.engineer' || host === 'www.banana.engineer') {
    return 'https://api.banana.engineer';
  }

  return '';
}

export function resolveApiBaseResolution(
    viteBaseUrl = resolveViteApiBaseUrl(),
    localhostDefault = resolveLocalhostDefaultApiBaseUrl()): ApiBaseResolution {
  const normalizedVite = viteBaseUrl.trim();
  if (normalizedVite.length > 0) {
    return {baseUrl: normalizedVite, error: null, source: 'vite'};
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
        'Missing API base URL. Set VITE_BANANA_API_BASE_URL and relaunch the frontend profile.',
    source: 'unresolved',
  };
}

export function resolveApiBaseResolutionError(
    viteBaseUrl = resolveViteApiBaseUrl(),
    localhostDefault = resolveLocalhostDefaultApiBaseUrl()): string|null {
  return resolveApiBaseResolution(viteBaseUrl, localhostDefault).error;
}

export function resolveApiBaseUrl(
    viteBaseUrl = resolveViteApiBaseUrl(),
    localhostDefault = resolveLocalhostDefaultApiBaseUrl()): string {
  return resolveApiBaseResolution(viteBaseUrl, localhostDefault).baseUrl;
}

export function resolvePlatformLabel(): string {
  return 'web';
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
  if (typeof window !== 'undefined' && path.startsWith('/operator/')) {
    throw new Error(
        'Operator endpoints are disabled in the public website client.');
  }

  const response = await fetch(`${baseUrl}${path}`, init);
  if (!response.ok) {
    throw new Error(await parseApiError(response));
  }
  return (await response.json()) as T;
}

export async function fetchApiJson<T>(
    baseUrl: string,
    path: string,
    init?: RequestInit,
    ): Promise<T> {
  return requestJson<T>(baseUrl, path, init);
}

function withBearerAuth(token: string): Record<string, string> {
  return {
    authorization: `Bearer ${token}`,
  };
}

function withOptionalBearerAuth(token?: string): Record<string, string> {
  if (!token || token.trim().length === 0) {
    return {};
  }

  return withBearerAuth(token);
}

export async function fetchBananaSummary(baseUrl: string):
    Promise<BananaSummaryResponse> {
  return requestJson<BananaSummaryResponse>(
      baseUrl, '/banana?purchases=3&multiplier=2');
}

export async function fetchTelemetryConfig(baseUrl: string):
    Promise<TelemetryConfigResponse> {
  return requestJson<TelemetryConfigResponse>(
      baseUrl, '/operator/telemetry/config');
}

export async function fetchApiHealth(baseUrl: string):
    Promise<ApiHealthResponse> {
  return requestJson<ApiHealthResponse>(baseUrl, '/health');
}

export async function fetchApiRoot(baseUrl: string): Promise<ApiRootResponse> {
  return requestJson<ApiRootResponse>(baseUrl, '/');
}

export async function fetchNetcodeAnalytics(
    baseUrl: string,
    payload: NetcodeAnalyticsRequest,
    ): Promise<NetcodeAnalyticsResponse> {
  const response = await fetch(`${baseUrl}/api/netcode/analytics`, {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify(payload),
  });

  if (!response.ok) {
    let errorPayload: NetcodeAnalyticsErrorResponse = {};
    try {
      errorPayload = (await response.json()) as NetcodeAnalyticsErrorResponse;
    } catch {
      throw new NetcodeAnalyticsError(response.status, {});
    }
    throw new NetcodeAnalyticsError(response.status, errorPayload);
  }

  return (await response.json()) as NetcodeAnalyticsResponse;
}

export async function fetchNetcodeLearning(
    baseUrl: string,
    payload: NetcodeLearningRequest,
    ): Promise<NetcodeLearningResponse> {
  return requestJson<NetcodeLearningResponse>(
      baseUrl, '/api/netcode/learning', {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify(payload),
      });
}

export async function startGuestAuthSession(
    baseUrl: string, alias: string): Promise<GuestAuthStartResponse> {
  return requestJson<GuestAuthStartResponse>(baseUrl, '/auth/guest/start', {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify({alias}),
  });
}

export async function fetchPlayerAccount(
    baseUrl: string, token: string): Promise<PlayerAccountResponse> {
  return requestJson<PlayerAccountResponse>(baseUrl, '/v1/player/account', {
    method: 'GET',
    headers: withBearerAuth(token),
  });
}

export async function fetchPlayerInsights(
    baseUrl: string, token: string): Promise<PlayerInsightsResponse> {
  return requestJson<PlayerInsightsResponse>(baseUrl, '/v1/player/insights', {
    method: 'GET',
    headers: withBearerAuth(token),
  });
}

export async function scaffoldTrainingJobs(
    baseUrl: string, token: string,
    jobs: Array<{title: string; sector: string; rewardXp: number}>):
    Promise<{playerId: string; jobs: TrainingJob[]}> {
  return requestJson<{playerId: string; jobs: TrainingJob[]}>(
      baseUrl,
      '/v1/player/training/jobs/scaffold',
      {
        method: 'POST',
        headers: {
          'content-type': 'application/json',
          ...withBearerAuth(token),
        },
        body: JSON.stringify({jobs}),
      },
  );
}

export async function scaffoldTrainingTemplateJobs(
    baseUrl: string, token: string, payload: {
      mode: TrainingWorkflowMode;
      selectedFile?: string;
      indexedFileCount?: number;
    }): Promise<{playerId: string; jobs: TrainingJob[]}> {
  return requestJson<{playerId: string; jobs: TrainingJob[]}>(
      baseUrl,
      '/v1/player/training/jobs/scaffold-template',
      {
        method: 'POST',
        headers: {
          'content-type': 'application/json',
          ...withBearerAuth(token),
        },
        body: JSON.stringify(payload),
      },
  );
}

export async function fetchTrainingJobs(baseUrl: string, token: string):
    Promise<{playerId: string; jobs: TrainingJob[]}> {
  return requestJson<{playerId: string; jobs: TrainingJob[]}>(
      baseUrl, '/v1/player/training/jobs', {
        method: 'GET',
        headers: withBearerAuth(token),
      });
}

export async function executeTrainingJob(
    baseUrl: string, token: string, jobId: string,
    payload: {score: number; durationMs: number; integrityProof: string;}):
    Promise<{
      playerId: string; antiCheatPassed: boolean; job: TrainingJob;
      reward: TrainingReward;
    }> {
  return requestJson(
      baseUrl, `/v1/player/training/jobs/${encodeURIComponent(jobId)}/execute`,
      {
        method: 'POST',
        headers: {
          'content-type': 'application/json',
          ...withBearerAuth(token),
        },
        body: JSON.stringify(payload),
      });
}

export async function executeTrainingCycle(
    baseUrl: string, token: string, payload: {
      mode: TrainingWorkflowMode;
      selectedFile?: string; selectedLineCount: number;
      indexedFileCount?: number;
    }): Promise<{
  playerId: string; attemptedJobs: number; completedJobs: number;
  mode: TrainingWorkflowMode;
  jobs: TrainingJob[];
  leaderboard: TrainingLeaderboardEntry[];
  rewards: TrainingReward[];
}> {
  return requestJson(
      baseUrl,
      '/v1/player/training/jobs/execute-cycle',
      {
        method: 'POST',
        headers: {
          'content-type': 'application/json',
          ...withBearerAuth(token),
        },
        body: JSON.stringify(payload),
      },
  );
}

export async function fetchTrainingLeaderboard(
    baseUrl: string, token: string,
    limit = 25): Promise<{leaderboard: TrainingLeaderboardEntry[]}> {
  const params = new URLSearchParams({
    limit: String(Math.max(1, Math.min(200, Math.trunc(limit)))),
  });
  return requestJson<{leaderboard: TrainingLeaderboardEntry[]}>(
      baseUrl,
      `/v1/player/training/leaderboard?${params.toString()}`,
      {
        method: 'GET',
        headers: withBearerAuth(token),
      },
  );
}

export async function fetchTrainingRewards(baseUrl: string, token: string):
    Promise<{playerId: string; rewards: TrainingReward[]}> {
  return requestJson<{playerId: string; rewards: TrainingReward[]}>(
      baseUrl,
      '/v1/player/training/rewards',
      {
        method: 'GET',
        headers: withBearerAuth(token),
      },
  );
}

export async function claimTrainingReward(
    baseUrl: string, token: string,
    rewardId: string): Promise<{playerId: string; reward: TrainingReward}> {
  return requestJson(
      baseUrl,
      `/v1/player/training/rewards/${encodeURIComponent(rewardId)}/claim`, {
        method: 'POST',
        headers: withBearerAuth(token),
      });
}

export async function claimAllTrainingRewards(
    baseUrl: string, token: string): Promise<{
  playerId: string; attemptedRewards: number; claimedRewards: TrainingReward[];
  failedRewards: Array<{rewardId: string; error: string}>;
  rewards: TrainingReward[];
  leaderboard: TrainingLeaderboardEntry[];
}> {
  return requestJson(
      baseUrl,
      '/v1/player/training/rewards/claim-all',
      {
        method: 'POST',
        headers: withBearerAuth(token),
      },
  );
}

export async function recordTrainingTransitionEvent(
    baseUrl: string, token: string, payload: {
      eventType: 'queue.scaffolded'|'queue.execution.started'|
      'queue.execution.completed'|'reward.claim.attempted'|
      'reward.claim.succeeded'|'reward.claim.failed';
      correlationId?: string;
      details?: Record<string, unknown>;
    }): Promise<{event: TrainingTransitionEvent}> {
  return requestJson<{event: TrainingTransitionEvent}>(
      baseUrl,
      '/v1/player/training/telemetry/transitions',
      {
        method: 'POST',
        headers: {
          'content-type': 'application/json',
          ...withBearerAuth(token),
        },
        body: JSON.stringify(payload),
      },
  );
}

export async function startGameSession(
    baseUrl: string, playerName: string,
    playerGuid: string): Promise<GameSessionBootstrap> {
  return requestJson<GameSessionBootstrap>(
      resolveGameplayApiBaseUrl(baseUrl), '/api/game/session/start', {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({playerName, playerGuid}),
      });
}

export async function rejoinGameSession(
    baseUrl: string, playerGuid: string): Promise<GameSessionBootstrap> {
  const response = await requestJson < GameSessionBootstrap & {
    playerName?: string;
  }
  > (resolveGameplayApiBaseUrl(baseUrl), '/api/game/session/rejoin', {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify({playerGuid}),
  });

  return {
    sessionId: response.sessionId,
    token: response.token,
    playerName: response.playerName ?? 'player',
    playerGuid,
  };
}

export async function setPlayerBuildClass(
    baseUrl: string, sessionId: string, playerGuid: string,
    classType: 0|1|2): Promise<void> {
  await requestJson(
      resolveGameplayApiBaseUrl(baseUrl),
      '/api/game/session/player/build/class', {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({sessionId, playerGuid, classType}),
      });
}

export async function setPlayerBuildAllocations(
    baseUrl: string, sessionId: string, playerGuid: string,
    offensePoints: number, defensePoints: number,
    utilityPoints: number): Promise<void> {
  await requestJson(
      resolveGameplayApiBaseUrl(baseUrl),
      '/api/game/session/player/build/allocations', {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({
          sessionId,
          playerGuid,
          offensePoints,
          defensePoints,
          utilityPoints,
        }),
      });
}

export async function equipPlayerBuildGear(
    baseUrl: string, sessionId: string, playerGuid: string, slot: 0|1|2,
    tier: number, attackBonus: number, defenseBonus: number,
    utilityBonus: number): Promise<void> {
  await requestJson(
      resolveGameplayApiBaseUrl(baseUrl),
      '/api/game/session/player/build/equip', {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({
          sessionId,
          playerGuid,
          slot,
          tier,
          attackBonus,
          defenseBonus,
          utilityBonus,
        }),
      });
}

export async function fetchPlayerBuildStats(
    baseUrl: string, sessionId: string,
    playerGuid: string): Promise<GameplayBuildStats> {
  const query = new URLSearchParams({sessionId, playerGuid});
  const payload =
      await requestJson<{playerGuid: string; stats: GameplayBuildStats}>(
          resolveGameplayApiBaseUrl(baseUrl),
          `/api/game/session/player/build/stats?${query.toString()}`,
      );
  return payload.stats;
}

export async function evaluatePlayerBuildCombo(
    baseUrl: string, sessionId: string, playerGuid: string, firstSkill: string,
    secondSkill: string, elapsedMs: number,
    partySize: number): Promise<GameplayComboResult> {
  const payload = await requestJson<{combo: GameplayComboResult}>(
      resolveGameplayApiBaseUrl(baseUrl),
      '/api/game/session/player/combo/evaluate',
      {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({
          sessionId,
          playerGuid,
          firstSkill,
          secondSkill,
          elapsedMs,
          partySize,
        }),
      },
  );
  return payload.combo;
}

export async function ingestTelemetryEvent(
    baseUrl: string, event: TelemetryApiEvent): Promise<void> {
  await requestJson(baseUrl, '/operator/telemetry/events', {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify({
      source: event.source,
      event: event.event,
      timestamp: event.timestamp,
      status: event.status,
      durationMs: event.durationMs,
      code: event.code,
      channel: event.channel,
      variant: event.variant,
      layer: event.layer,
      details: event.details ?? null,
    }),
  });
}

export async function fetchTelemetryEvents(
    baseUrl: string, limit = 300,
    source?: string): Promise<TelemetryEventsResponse> {
  const clampedLimit = Math.max(1, Math.min(1000, Math.trunc(limit)));
  const params = new URLSearchParams({limit: String(clampedLimit)});
  if (source && source.trim().length > 0) {
    params.set('source', source.trim());
  }

  return requestJson<TelemetryEventsResponse>(
      baseUrl, `/operator/telemetry/events?${params.toString()}`);
}

export async function reportComponentErrorTelemetry(
    baseUrl: string, payload: ComponentErrorTelemetryPayload): Promise<void> {
  await requestJson(baseUrl, '/telemetry/component-errors', {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify(payload),
  });
}

export async function verifyLaunchGate(
    baseUrl: string,
    payload: {
      attemptId?: string; mode: string;
      steamAssertion?: {
        steamId?: string;
        validationStatus?: 'valid' | 'invalid' | 'expired' | 'indeterminate';
      };
      clientBuildChannel?: string;
      overrideContext?: Record<string, unknown>;
    },
    token?: string,
    ): Promise < LaunchGateVerifyResponse&{
  attemptId: string;
  freshnessExpiresAt: string
}
> {
  return requestJson < LaunchGateVerifyResponse & {
    attemptId: string;
    freshnessExpiresAt: string;
  }
  > (baseUrl, '/v1/player/launch-gate/verify', {
    method: 'POST',
    headers: {
      'content-type': 'application/json',
      ...withOptionalBearerAuth(token),
    },
    body: JSON.stringify(payload),
  });
}

export async function fetchLaunchGateStatus(
    baseUrl: string,
    attemptId: string,
    token?: string,
    ): Promise<LaunchGateStatusResponse> {
  return requestJson<LaunchGateStatusResponse>(
      baseUrl,
      `/v1/player/launch-gate/status/${encodeURIComponent(attemptId)}`,
      {
        method: 'GET',
        headers: withOptionalBearerAuth(token),
      },
  );
}

export async function submitLaunchGateRetryIntent(
    baseUrl: string,
    payload: {
      attemptId: string; priorReasonCode: LaunchGateReasonCode;
      clientObservedAt: string;
    },
    token?: string,
    ): Promise<{
  retryAccepted: boolean; nextAttemptId: string;
  requiresFreshVerification: boolean;
}> {
  return requestJson<{
    retryAccepted: boolean; nextAttemptId: string;
    requiresFreshVerification: boolean;
  }>(baseUrl, '/v1/player/launch-gate/retry-intent', {
    method: 'POST',
    headers: {
      'content-type': 'application/json',
      ...withOptionalBearerAuth(token),
    },
    body: JSON.stringify(payload),
  });
}

export async function createAccountLinkRemediationTicket(
    baseUrl: string,
    reasonCode: LaunchGateReasonCode,
    token?: string,
    ): Promise<{
  remediationTicketId: string; reasonCode: LaunchGateReasonCode;
  webFlowUrl: string;
  requiredActions: string[];
  status: 'pending' | 'in-progress' | 'completed' | 'expired';
  createdAt: string;
  updatedAt: string;
  retryAllowedAfter: string;
  completedAt?: string;
}> {
  return requestJson(baseUrl, '/v1/player/account-link-remediation/ticket', {
    method: 'POST',
    headers: {
      'content-type': 'application/json',
      ...withOptionalBearerAuth(token),
    },
    body: JSON.stringify({reasonCode}),
  });
}

export async function fetchAccountLinkRemediationTicket(
    baseUrl: string,
    remediationTicketId: string,
    token?: string,
    ): Promise<{
  remediationTicketId: string; reasonCode: LaunchGateReasonCode;
  webFlowUrl: string;
  requiredActions: string[];
  status: 'pending' | 'in-progress' | 'completed' | 'expired';
  createdAt: string;
  updatedAt: string;
  retryAllowedAfter: string;
  completedAt?: string;
}> {
  return requestJson(
      baseUrl,
      `/v1/player/account-link-remediation/ticket/${
          encodeURIComponent(remediationTicketId)}`,
      {
        method: 'GET',
        headers: withOptionalBearerAuth(token),
      },
  );
}

export async function completeAccountLinkRemediationTicket(
    baseUrl: string,
    remediationTicketId: string,
    token?: string,
    ): Promise<{
  remediationTicketId: string; reasonCode: LaunchGateReasonCode;
  webFlowUrl: string;
  requiredActions: string[];
  status: 'pending' | 'in-progress' | 'completed' | 'expired';
  createdAt: string;
  updatedAt: string;
  retryAllowedAfter: string;
  completedAt?: string;
}> {
  return requestJson(
      baseUrl,
      `/v1/player/account-link-remediation/ticket/${
          encodeURIComponent(remediationTicketId)}/complete`,
      {
        method: 'POST',
        headers: withOptionalBearerAuth(token),
      },
  );
}

export type K3h4ScalingSeriesEntry = {
  readonly n: number; readonly k3h4Ns: number; readonly attentionNs: number;
};

export type K3h4ScalingBenchmarkResponse = {
  readonly contractVersion: number; readonly schemaVersion: number; readonly series: readonly K3h4ScalingSeriesEntry[]; readonly metadata: {
    readonly calibratedSizes: readonly number[]; readonly extrapolatedAbove:
                                                              number | null;
  };
};

export async function fetchK3h4ScalingBenchmark(
    baseUrl: string,
    ): Promise<K3h4ScalingBenchmarkResponse> {
  return fetchApiJson<K3h4ScalingBenchmarkResponse>(
      baseUrl,
      '/api/netcode/k3h4/scaling-benchmark',
  );
}

export async function fetchK3h4EpochGeometry(
    baseUrl: string,
    sessionId: string,
    epoch: number,
    mode: 'multiplicative'|'power' = 'power',
    ): Promise<K3h4EpochGeometryResponse> {
  const query = new URLSearchParams({mode}).toString();
  return fetchApiJson<K3h4EpochGeometryResponse>(
      baseUrl,
      `/api/netcode/k3h4/training-session/${
          encodeURIComponent(sessionId)}/epoch/${epoch}/geometry?${query}`,
  );
}
