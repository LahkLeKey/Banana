import type {FastifyInstance} from 'fastify';
import {randomUUID} from 'node:crypto';
import type {IncomingMessage} from 'node:http';
import type {Socket} from 'node:net';
import WebSocket, {type RawData, WebSocketServer} from 'ws';

import {type AntiCheatInteropAdapter, createDefaultAntiCheatInteropAdapter,} from '../lib/anti-cheat-interop.ts';
import {createDefaultNativeEngineService, type NativeEnginePlayerSync, type NativeEngineService, type NativeEngineSnapshot, type NativeEngineSnapshotEntity,} from '../services/nativeEngine.ts';

export interface GameInputCommandEnvelope {
  readonly sequence: number;
  readonly tick: number;
  readonly moveX: -1|0|1;
  readonly moveZ: -1|0|1;
  readonly timestamp: number;
}

export interface ValidationResult {
  readonly ok: boolean;
  readonly reason?:|'sequence_non_monotonic'|'tick_non_monotonic'|
      'invalid_axis'|'invalid_timestamp';
}

type SessionPlayerRecord = {
  readonly playerId: string; playerName: string; controllerKind: 'human' | 'ai'; lastTick: number; lastSequence: number; lastSeenAt: number; readonly joinedAt:
                                                                                                                                                          number;
  socket: WebSocket | null;
};

type SessionRecord = {
  readonly sessionId: string;
  readonly token: string;
  readonly playerName: string;
  lastTick: number;
  lastSequence: number;
  readonly createdAt: number;
  heartbeatLayers: {
    transport: { lastSequence: number; lastAt: number | null };
    session: { lastSequence: number; lastAt: number | null };
    integrity: { lastSequence: number; lastAt: number | null };
  };
  antiCheatSnapshots: Array<{
    at: number;
    score: number;
    flagged: boolean;
    integrityHash: number;
    source: 'usermode' | 'driver' | 'session';
  }>;
  readonly players: Map<string, SessionPlayerRecord>;
  readonly latestInputs: Map<string, GameInputCommandEnvelope>;
  lastSnapshot: NativeEngineSnapshot | null;
  serverMetrics: {
    targetTps: number;
    currentTps: number;
    averageTickMs: number;
    lagMs: number;
    lastTickAt: number | null;
    tickDurationsMs: number[];
  };
  realtimeLoop: ReturnType<typeof setInterval> | null;
  tickInFlight: boolean;
  readonly workflowEvents: Array<{
    workflowId: string;
    workflowName: string;
    issuedBy: string;
    patch: Record<string, unknown>;
    createdAt: number;
  }>;
};

type RealtimeClientPayload =|({
  readonly type: 'input';
}&GameInputCommandEnvelope)|{
  readonly type: 'ping';
};

type LabeledSnapshotEntity = NativeEngineSnapshotEntity&{
  readonly ownerPlayerId?: string;
  readonly ownerPlayerName?: string;
  readonly tooltipLabel?: string;
};

type LabeledSnapshot = Omit<NativeEngineSnapshot, 'entities'>&{
  readonly entities: Record<string, LabeledSnapshotEntity>;
};

type BuildClassType = 0|1|2;
type BuildGearSlot = 0|1|2;

const sessions = new Map<string, SessionRecord>();
const MAX_SNAPSHOTS = 64;
const REALTIME_TICK_MS = 1000 / 30;
const OVERWORLD_SESSION_ID = 'overworld';
const OVERWORLD_SESSION_TOKEN = 'gst_overworld';
const METRICS_WINDOW_SIZE = 120;
const MAX_WORKFLOW_EVENTS = 64;

let activeRealtimeSessionId: string|null = null;

function makePlayerId(): string {
  return randomUUID().toLowerCase();
}

function isGuid(value: string): boolean {
  return /^[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i
      .test(value);
}

function resolvePlayerId(playerGuidRaw: string|null): string {
  const normalized = (playerGuidRaw ?? '').trim().toLowerCase();
  return isGuid(normalized) ? normalized : makePlayerId();
}

function isAxis(value: number): value is - 1|0|1 {
  return value === -1 || value === 0 || value === 1;
}

function isNonNegativeInteger(value: number): boolean {
  return Number.isInteger(value) && value >= 0;
}

function appendSnapshot(session: SessionRecord, snapshot: {
  at: number; score: number; flagged: boolean; integrityHash: number;
  source: 'usermode' | 'driver' | 'session';
}): void {
  session.antiCheatSnapshots.push(snapshot);
  if (session.antiCheatSnapshots.length > MAX_SNAPSHOTS) {
    session.antiCheatSnapshots.shift();
  }
}

function appendWorkflowEvent(
    session: SessionRecord,
    event: {
      workflowId: string;
      workflowName: string;
      issuedBy: string;
      patch: Record<string, unknown>;
      createdAt: number;
    }): void {
  session.workflowEvents.push(event);
  if (session.workflowEvents.length > MAX_WORKFLOW_EVENTS) {
    session.workflowEvents.shift();
  }
}

function makeServerMetrics() {
  const targetTps = 1000 / REALTIME_TICK_MS;
  return {
    targetTps,
    currentTps: targetTps,
    averageTickMs: REALTIME_TICK_MS,
    lagMs: 0,
    lastTickAt: null,
    tickDurationsMs: [] as number[],
  };
}

function updateServerMetrics(
    session: SessionRecord, tickDurationMs: number): void {
  session.serverMetrics.tickDurationsMs.push(tickDurationMs);
  if (session.serverMetrics.tickDurationsMs.length > METRICS_WINDOW_SIZE) {
    session.serverMetrics.tickDurationsMs.shift();
  }

  const total = session.serverMetrics.tickDurationsMs.reduce(
      (acc, value) => acc + value, 0);
  const averageTickMs =
      total / Math.max(1, session.serverMetrics.tickDurationsMs.length);
  const currentTps = averageTickMs > 0 ? 1000 / averageTickMs :
                                         session.serverMetrics.targetTps;

  session.serverMetrics.averageTickMs = averageTickMs;
  session.serverMetrics.currentTps = currentTps;
  session.serverMetrics.lagMs = Math.max(0, averageTickMs - REALTIME_TICK_MS);
  session.serverMetrics.lastTickAt = Date.now();
}

function resolveSessionId(raw: string|undefined): string {
  const normalized = (raw ?? '').toString().trim();
  return normalized.length > 0 ? normalized : OVERWORLD_SESSION_ID;
}

async function ensureOverworldSession(
    antiCheatAdapter: AntiCheatInteropAdapter,
    nativeEngine: NativeEngineService,
    playerName: string): Promise<{session: SessionRecord; created: boolean}> {
  const existing = sessions.get(OVERWORLD_SESSION_ID);
  if (existing) {
    return {session: existing, created: false};
  }

  const now = Date.now();
  const session: SessionRecord = {
    sessionId: OVERWORLD_SESSION_ID,
    token: OVERWORLD_SESSION_TOKEN,
    playerName,
    lastTick: 0,
    lastSequence: 0,
    createdAt: now,
    heartbeatLayers: {
      transport: {lastSequence: 0, lastAt: null},
      session: {lastSequence: 0, lastAt: null},
      integrity: {lastSequence: 0, lastAt: null},
    },
    antiCheatSnapshots: [],
    players: new Map(),
    latestInputs: new Map(),
    lastSnapshot: null,
    serverMetrics: makeServerMetrics(),
    realtimeLoop: null,
    tickInFlight: false,
    workflowEvents: [],
  };

  await antiCheatAdapter.resetSession(OVERWORLD_SESSION_ID);
  const status = await antiCheatAdapter.getSessionStatus(OVERWORLD_SESSION_ID);
  await nativeEngine.reset();
  appendSnapshot(session, {
    at: now,
    score: status.score,
    flagged: status.flagged,
    integrityHash: status.integrityHash,
    source: 'session',
  });

  sessions.set(OVERWORLD_SESSION_ID, session);
  return {session, created: true};
}

function getSessionOrReply(sessionId: string, reply: {
  status:
      (code: number) => {
        send: (body: unknown) => unknown
      }
}): SessionRecord|null {
  const session = sessions.get(sessionId);
  if (!session) {
    reply.status(404).send({error: 'session_not_found'});
    return null;
  }
  return session;
}

function rejectUpgrade(socket: Socket, statusCode: number, body: string): void {
  socket.write(`HTTP/1.1 ${
      statusCode}\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: ${
      Buffer.byteLength(body)}\r\n\r\n${body}`);
  socket.destroy();
}

function listPlayers(session: SessionRecord) {
  return Array.from(session.players.values())
      .map((player) => ({
             playerId: player.playerId,
             playerName: player.playerName,
             controllerKind: player.controllerKind,
             connected: player.socket?.readyState === WebSocket.OPEN,
             lastTick: player.lastTick,
             lastSequence: player.lastSequence,
           }));
}

function broadcastJson(session: SessionRecord, payload: unknown): void {
  const message = JSON.stringify(payload);
  for (const player of session.players.values()) {
    if (player.socket?.readyState === WebSocket.OPEN) {
      player.socket.send(message);
    }
  }
}

function cleanupRealtimeLoop(session: SessionRecord): void {
  if (session.realtimeLoop) {
    clearInterval(session.realtimeLoop);
    session.realtimeLoop = null;
  }

  if (activeRealtimeSessionId === session.sessionId) {
    activeRealtimeSessionId = null;
  }
}

function pickRealtimeInput(session: SessionRecord): GameInputCommandEnvelope {
  const recentInputs =
      Array.from(session.latestInputs.values())
          .filter((input) => Date.now() - input.timestamp < 2_000);

  if (recentInputs.length === 0) {
    return {
      sequence: session.lastSequence + 1,
      tick: session.lastTick + 1,
      moveX: 0,
      moveZ: 0,
      timestamp: Date.now(),
    };
  }

  const summed = recentInputs.reduce<{
    moveX: number; moveZ: number; sequence: number; tick: number;
    timestamp: number;
  }>((state, input) => ({
       moveX: state.moveX + input.moveX,
       moveZ: state.moveZ + input.moveZ,
       sequence: Math.max(state.sequence, input.sequence),
       tick: Math.max(state.tick, input.tick),
       timestamp: Math.max(state.timestamp, input.timestamp),
     }),
     {moveX: 0, moveZ: 0, sequence: 0, tick: 0, timestamp: 0});

  return {
    sequence: summed.sequence,
    tick: summed.tick,
    moveX: summed.moveX > 0 ? 1 :
        summed.moveX < 0    ? -1 :
                              0,
    moveZ: summed.moveZ > 0 ? 1 :
        summed.moveZ < 0    ? -1 :
                              0,
    timestamp: summed.timestamp || Date.now(),
  };
}

function buildNativePlayerSyncPayload(session: SessionRecord):
    NativeEnginePlayerSync[] {
  const now = BigInt(Date.now());

  /* Mark all connected players as seen (C API will handle staleness detection)
   */
  const connectedPlayers =
      Array.from(session.players.values())
          .filter((player) => player.socket?.readyState === WebSocket.OPEN);

  return connectedPlayers.map((player) => {
    const latest = session.latestInputs.get(player.playerId);
    const moveX = latest ? latest.moveX : 0;
    const moveZ = latest ? latest.moveZ : 0;
    return {
      playerId: player.playerId,
      playerName: player.playerName,
      controllerKind: player.controllerKind,
      connected: true,
      moveX,
      moveZ,
    };
  });
}

async function ensureBuildPlayerBound(
    session: SessionRecord, nativeEngine: NativeEngineService,
    playerId: string): Promise<void> {
  const existing = session.players.get(playerId);
  const record: SessionPlayerRecord = existing ?? {
    playerId,
    playerName: `player-${playerId.slice(-6)}`,
    controllerKind: 'human',
    lastTick: session.lastTick,
    lastSequence: session.lastSequence,
    lastSeenAt: Date.now(),
    joinedAt: Date.now(),
    socket: null,
  };
  session.players.set(playerId, record);

  await nativeEngine.syncPlayers([
    {
      playerId: record.playerId,
      playerName: record.playerName,
      controllerKind: record.controllerKind,
      connected: true,
      moveX: 0,
      moveZ: 0,
    },
  ]);
}

function buildSnapshotEnvelope(
    session: SessionRecord, snapshot: NativeEngineSnapshot) {
  const players = listPlayers(session);
  const playersById =
      new Map(players.map((player) => [player.playerId, player]));

  const labeledEntities: Record<string, LabeledSnapshotEntity> =
      Object.fromEntries(
          Object.entries(snapshot.entities)
              .map(([entityKey, entity]) => [entityKey, {...entity}]));

  for (const [entityKey, entity] of Object.entries(labeledEntities)) {
    if (entity.type !== 'player') continue;
    const playerGuid = entity.playerGuid?.trim().toLowerCase();
    if (!playerGuid) continue;

    const player = playersById.get(playerGuid);
    if (!player) continue;

    const shortId = player.playerId.length > 6 ? player.playerId.slice(-6) :
                                                 player.playerId;
    labeledEntities[entityKey] = {
      ...entity,
      ownerPlayerId: player.playerId,
      ownerPlayerName: player.playerName,
      tooltipLabel: `${player.playerName} · ${shortId}`,
    };
  }

  const labeledSnapshot: LabeledSnapshot = {
    ...snapshot,
    entities: labeledEntities,
  };

  return {
    type: 'snapshot' as const,
    sessionId: session.sessionId,
    snapshot: labeledSnapshot,
    players,
    server: {
      targetTps: session.serverMetrics.targetTps,
      currentTps: session.serverMetrics.currentTps,
      averageTickMs: session.serverMetrics.averageTickMs,
      lagMs: session.serverMetrics.lagMs,
      lastTickAt: session.serverMetrics.lastTickAt,
    },
  };
}

function buildWorldStatsEnvelope(session: SessionRecord) {
  return {
    sessionId: session.sessionId,
    mode: 'overworld' as const,
    replayCursor: {
      lastTick: session.lastTick,
      lastSequence: session.lastSequence,
    },
    players: {
      connected: Array.from(session.players.values())
                     .filter(
                         (player) =>
                             player.socket?.readyState === WebSocket.OPEN)
                     .length,
      total: session.players.size,
      roster: listPlayers(session),
    },
    world: {
      entityCount: Object.keys(session.lastSnapshot?.entities ?? {}).length,
      lastSnapshotTick: session.lastSnapshot?.tick ?? session.lastTick,
      lastSnapshotTimestamp:
          session.lastSnapshot?.timestamp ?? session.serverMetrics.lastTickAt,
    },
    server: {
      targetTps: session.serverMetrics.targetTps,
      currentTps: session.serverMetrics.currentTps,
      averageTickMs: session.serverMetrics.averageTickMs,
      lagMs: session.serverMetrics.lagMs,
      lastTickAt: session.serverMetrics.lastTickAt,
      tickInFlight: session.tickInFlight,
    },
    workflows: {
      totalEvents: session.workflowEvents.length,
      recent: session.workflowEvents.slice(-10),
    },
  };
}

async function tickRealtimeSession(
    session: SessionRecord, nativeEngine: NativeEngineService): Promise<void> {
  if (session.tickInFlight) return;

  const connectedPlayers =
      Array.from(session.players.values())
          .filter((player) => player.socket?.readyState === WebSocket.OPEN);
  if (connectedPlayers.length === 0) {
    cleanupRealtimeLoop(session);
    return;
  }

  session.tickInFlight = true;
  const tickStart = Date.now();
  const nowMs = BigInt(tickStart);
  const staleThresholdMs = BigInt(2000); /* 2-second staleness window */

  try {
    /* Mark connected players as seen (ownership moved to C) */
    for (const player of connectedPlayers) {
      await nativeEngine.playerSyncMarkSeen(player.playerId, nowMs);
    }

    /* Update staleness detection (ownership moved to C) */
    await nativeEngine.playerSyncUpdateStaleness(nowMs, staleThresholdMs);
    await nativeEngine.playerSyncDeactivateStale();

    const input = pickRealtimeInput(session);
    await nativeEngine.syncPlayers(buildNativePlayerSyncPayload(session));
    const snapshot = await nativeEngine.tickSerialize({
      moveX: 0,
      moveZ: 0,
      dtSeconds: REALTIME_TICK_MS / 1000,
    });
    session.lastTick = snapshot.tick;
    session.lastSequence = Math.max(session.lastSequence, input.sequence);
    session.lastSnapshot = snapshot;
    updateServerMetrics(session, Math.max(0, Date.now() - tickStart));
    broadcastJson(session, buildSnapshotEnvelope(session, snapshot));
  } finally {
    session.tickInFlight = false;
  }
}

function startRealtimeLoop(
    session: SessionRecord, nativeEngine: NativeEngineService): void {
  if (session.realtimeLoop) return;
  session.realtimeLoop = setInterval(() => {
    void tickRealtimeSession(session, nativeEngine);
  }, REALTIME_TICK_MS);
}

function parseRealtimePayload(raw: string): RealtimeClientPayload {
  const parsed = JSON.parse(raw) as RealtimeClientPayload;
  if (typeof parsed !== 'object' || parsed === null ||
      typeof parsed.type !== 'string' ||
      (parsed.type !== 'input' && parsed.type !== 'ping')) {
    throw new Error('invalid_realtime_message');
  }

  return parsed;
}

export function validateInputCommandEnvelope(
    command: GameInputCommandEnvelope,
    previous?: {sequence: number; tick: number}): ValidationResult {
  if (!isAxis(command.moveX) || !isAxis(command.moveZ)) {
    return {ok: false, reason: 'invalid_axis'};
  }

  if (!Number.isFinite(command.timestamp) || command.timestamp <= 0) {
    return {ok: false, reason: 'invalid_timestamp'};
  }

  if (previous && command.sequence <= previous.sequence) {
    return {ok: false, reason: 'sequence_non_monotonic'};
  }

  if (previous && command.tick < previous.tick) {
    return {ok: false, reason: 'tick_non_monotonic'};
  }

  return {ok: true};
}

export function __resetGameSessionStateForTests(): void {
  for (const session of sessions.values()) {
    cleanupRealtimeLoop(session);
    for (const player of session.players.values()) {
      player.socket?.close();
    }
  }
  sessions.clear();
  activeRealtimeSessionId = null;
}

export async function registerGameSessionRoutes(
    app: FastifyInstance,
    antiCheatAdapter: AntiCheatInteropAdapter =
        createDefaultAntiCheatInteropAdapter(),
    nativeEngine: NativeEngineService =
        createDefaultNativeEngineService()): Promise<void> {
  const realtimeServer = new WebSocketServer({noServer: true});

  const upgradeHandler =
      (request: IncomingMessage, socket: Socket, head: Buffer) => {
        const requestUrl = new URL(request.url ?? '/', 'http://localhost');
        if (requestUrl.pathname !== '/game-session') {
          return;
        }

        const sessionId =
            resolveSessionId(requestUrl.searchParams.get('sessionId') ?? '');
        const playerName =
            requestUrl.searchParams.get('playerName')?.trim() ?? '';
        const playerId =
            resolvePlayerId(requestUrl.searchParams.get('playerGuid'));
        const controllerKind =
            requestUrl.searchParams.get('controllerKind') === 'ai' ? 'ai' :
                                                                     'human';

        if (!sessionId || !playerName) {
          rejectUpgrade(
              socket, 400, JSON.stringify({error: 'invalid_argument'}));
          return;
        }

        const session = sessions.get(sessionId);
        if (!session) {
          rejectUpgrade(
              socket, 404, JSON.stringify({error: 'session_not_found'}));
          return;
        }

        if (activeRealtimeSessionId && activeRealtimeSessionId !== sessionId) {
          rejectUpgrade(socket, 409, JSON.stringify({
            error: 'session_busy',
            detail:
                'Only one authoritative realtime session may be active at a time.',
          }));
          return;
        }

        realtimeServer.handleUpgrade(request, socket, head, (ws: WebSocket) => {
          const now = Date.now();
          const existingPlayer = session.players.get(playerId);
          const existingSocket = existingPlayer?.socket;
          if (existingSocket?.readyState === WebSocket.OPEN) {
            existingSocket.close(4000, 'identity_rebound');
          }

          const player: SessionPlayerRecord = existingPlayer ?? {
            playerId,
            playerName,
            controllerKind,
            lastTick: session.lastTick,
            lastSequence: session.lastSequence,
            lastSeenAt: now,
            joinedAt: now,
            socket: null,
          };

          player.playerName = playerName;
          player.controllerKind = controllerKind;
          player.lastSeenAt = now;
          player.socket = ws;

          session.players.set(playerId, player);
          activeRealtimeSessionId = sessionId;

          const connectRealtimeClient = () => {
            startRealtimeLoop(session, nativeEngine);
            ws.send(JSON.stringify({
              type: 'connected',
              sessionId,
              playerId,
              players: listPlayers(session),
            }));

            if (session.lastSnapshot) {
              ws.send(JSON.stringify(
                  buildSnapshotEnvelope(session, session.lastSnapshot)));
            }
          };

          connectRealtimeClient();

          ws.on('message', (buffer: RawData) => {
            try {
              const payload = parseRealtimePayload(buffer.toString());
              player.lastSeenAt = Date.now();

              if (payload.type === 'ping') {
                ws.send(JSON.stringify({type: 'pong', ts: Date.now()}));
                return;
              }

              const validation = validateInputCommandEnvelope(payload, {
                sequence: player.lastSequence,
                tick: player.lastTick,
              });

              if (!validation.ok) {
                ws.send(JSON.stringify({
                  type: 'error',
                  error: 'invalid_input',
                  detail: validation.reason,
                  expectedSequence: player.lastSequence,
                  expectedTick: player.lastTick,
                }));
                return;
              }

              player.lastSequence = payload.sequence;
              player.lastTick = payload.tick;
              session.latestInputs.set(player.playerId, payload);
              session.lastSequence =
                  Math.max(session.lastSequence, payload.sequence);
              session.lastTick = Math.max(session.lastTick, payload.tick);
              ws.send(JSON.stringify({
                type: 'ack',
                sequence: payload.sequence,
                tick: payload.tick,
              }));
            } catch (error: unknown) {
              ws.send(JSON.stringify({
                type: 'error',
                error: 'invalid_realtime_message',
                detail: error instanceof Error ? error.message : String(error),
              }));
            }
          });

          ws.on('close', () => {
            session.latestInputs.delete(player.playerId);
            if (player.socket === ws) {
              player.socket = null;
              player.lastSeenAt = Date.now();
            }
            cleanupRealtimeLoop(session);
            const hasConnectedPlayers =
                Array.from(session.players.values())
                    .some(
                        (candidate) =>
                            candidate.socket?.readyState === WebSocket.OPEN);
            if (hasConnectedPlayers) {
              activeRealtimeSessionId = session.sessionId;
              startRealtimeLoop(session, nativeEngine);
            }
          });
        });
      };

  app.server.on('upgrade', upgradeHandler);

  app.addHook('onClose', async () => {
    app.server.off('upgrade', upgradeHandler);
    __resetGameSessionStateForTests();
    await nativeEngine.shutdown();
    await new Promise<void>((resolve, reject) => {
      realtimeServer.close((error) => {
        if (error) {
          reject(error);
          return;
        }
        resolve();
      });
    });
  });

  app.post('/api/game/session/start', async (request, reply) => {
    const body = (request.body ?? {}) as {
      playerName?: string;
      playerGuid?: string
    };
    const playerName =
        (body.playerName ?? 'player').toString().trim() || 'player';
    const playerGuid = resolvePlayerId(body.playerGuid ?? null);

    const {session, created} = await ensureOverworldSession(
        antiCheatAdapter, nativeEngine, playerName);
    const status = await antiCheatAdapter.getSessionStatus(session.sessionId);

    return reply.status(created ? 201 : 200).send({
      sessionId: session.sessionId,
      token: session.token,
      playerName,
      playerGuid,
      worldState: {
        tick: session.lastTick,
        timestamp: Date.now(),
        entities: session.lastSnapshot?.entities ?? {},
        isAuthoritative: true as const,
      },
      antiCheat: status,
      websocketPath: '/game-session',
      mode: 'overworld',
    });
  });

  app.post('/api/game/session/rejoin', async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      playerGuid?: string;
      playerName?: string;
    };
    const sessionId = resolveSessionId(body.sessionId);
    const playerGuid = resolvePlayerId(body.playerGuid ?? null);

    const {session} = await ensureOverworldSession(
        antiCheatAdapter, nativeEngine, body.playerName ?? 'player');

    const antiCheat = await antiCheatAdapter.getSessionStatus(sessionId);

    return reply.status(200).send({
      sessionId: session.sessionId,
      token: session.token,
      playerGuid,
      replayCursor: {
        lastTick: session.lastTick,
        lastSequence: session.lastSequence,
      },
      worldState: session.lastSnapshot ?? ({
                    tick: session.lastTick,
                    timestamp: Date.now(),
                    entities: {},
                    isAuthoritative: true,
                  } satisfies NativeEngineSnapshot),
      antiCheat,
      heartbeatLayers: session.heartbeatLayers,
      antiCheatSnapshots: session.antiCheatSnapshots,
      players: listPlayers(session),
      mode: 'overworld',
    });
  });

  app.post('/api/game/session/end', async (request, reply) => {
    const body = (request.body ?? {}) as {sessionId?: string};
    const sessionId = (body.sessionId ?? '').toString().trim();
    if (!sessionId) {
      return reply.status(400).send({error: 'invalid_argument'});
    }

    const session = sessions.get(sessionId);
    if (session) {
      cleanupRealtimeLoop(session);
      for (const player of session.players.values()) {
        player.socket?.close();
      }
      session.players.clear();
      session.latestInputs.clear();
    }

    const existed = sessions.delete(sessionId);
    return reply.status(200).send({ended: existed, sessionId});
  });

  app.post('/api/game/session/player/build/class', async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      playerGuid?: string;
      classType?: number;
    };
    const sessionId = resolveSessionId(body.sessionId);
    const playerGuid = (body.playerGuid ?? '').toString().trim().toLowerCase();
    const classType = Number(body.classType);

    if (!playerGuid || !isGuid(playerGuid) || ![0, 1, 2].includes(classType)) {
      return reply.status(400).send({error: 'invalid_argument'});
    }

    const session = getSessionOrReply(sessionId, reply);
    if (!session) return reply;

    await ensureBuildPlayerBound(session, nativeEngine, playerGuid);
    await nativeEngine.setPlayerClass(playerGuid, classType as BuildClassType);

    return reply.status(200).send({ok: true, playerGuid, classType});
  });

  app.post(
      '/api/game/session/player/build/allocations', async (request, reply) => {
        const body = (request.body ?? {}) as {
          sessionId?: string;
          playerGuid?: string;
          offensePoints?: number;
          defensePoints?: number;
          utilityPoints?: number;
        };
        const sessionId = resolveSessionId(body.sessionId);
        const playerGuid =
            (body.playerGuid ?? '').toString().trim().toLowerCase();
        const offensePoints = Number(body.offensePoints ?? 0);
        const defensePoints = Number(body.defensePoints ?? 0);
        const utilityPoints = Number(body.utilityPoints ?? 0);
        const allocationTotal = offensePoints + defensePoints + utilityPoints;

        if (!playerGuid || !isGuid(playerGuid) ||
            !isNonNegativeInteger(offensePoints) ||
            !isNonNegativeInteger(defensePoints) ||
            !isNonNegativeInteger(utilityPoints) || allocationTotal > 20) {
          return reply.status(400).send({error: 'invalid_argument'});
        }

        const session = getSessionOrReply(sessionId, reply);
        if (!session) return reply;

        await ensureBuildPlayerBound(session, nativeEngine, playerGuid);
        await nativeEngine.setPlayerAllocations(
            playerGuid, offensePoints, defensePoints, utilityPoints);

        return reply.status(200).send({
          ok: true,
          playerGuid,
          offensePoints,
          defensePoints,
          utilityPoints,
        });
      });

  app.post('/api/game/session/player/build/equip', async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      playerGuid?: string;
      slot?: number;
      tier?: number;
      attackBonus?: number;
      defenseBonus?: number;
      utilityBonus?: number;
    };
    const sessionId = resolveSessionId(body.sessionId);
    const playerGuid = (body.playerGuid ?? '').toString().trim().toLowerCase();
    const slot = Number(body.slot);
    const tier = Number(body.tier);
    const attackBonus = Number(body.attackBonus ?? 0);
    const defenseBonus = Number(body.defenseBonus ?? 0);
    const utilityBonus = Number(body.utilityBonus ?? 0);

    if (!playerGuid || !isGuid(playerGuid) || ![0, 1, 2].includes(slot) ||
        !isNonNegativeInteger(tier) || !Number.isInteger(attackBonus) ||
        !Number.isInteger(defenseBonus) || !Number.isInteger(utilityBonus)) {
      return reply.status(400).send({error: 'invalid_argument'});
    }

    const session = getSessionOrReply(sessionId, reply);
    if (!session) return reply;

    await ensureBuildPlayerBound(session, nativeEngine, playerGuid);
    await nativeEngine.equipPlayerGear(
        playerGuid,
        slot as BuildGearSlot,
        tier,
        attackBonus,
        defenseBonus,
        utilityBonus,
    );

    return reply.status(200).send({
      ok: true,
      playerGuid,
      slot,
      tier,
      attackBonus,
      defenseBonus,
      utilityBonus,
    });
  });

  app.get('/api/game/session/player/build/stats', async (request, reply) => {
    const query = (request.query ?? {}) as {
      sessionId?: string;
      playerGuid?: string;
    };
    const sessionId = resolveSessionId(query.sessionId);
    const playerGuid = (query.playerGuid ?? '').toString().trim().toLowerCase();

    if (!playerGuid || !isGuid(playerGuid)) {
      return reply.status(400).send({error: 'invalid_argument'});
    }

    const session = getSessionOrReply(sessionId, reply);
    if (!session) return reply;

    await ensureBuildPlayerBound(session, nativeEngine, playerGuid);
    const stats = await nativeEngine.getPlayerBuildStats(playerGuid);
    return reply.status(200).send({playerGuid, stats});
  });

  app.post(
      '/api/game/session/player/combo/evaluate', async (request, reply) => {
        const body = (request.body ?? {}) as {
          sessionId?: string;
          playerGuid?: string;
          firstSkill?: string;
          secondSkill?: string;
          elapsedMs?: number;
          partySize?: number;
        };
        const sessionId = resolveSessionId(body.sessionId);
        const playerGuid =
            (body.playerGuid ?? '').toString().trim().toLowerCase();
        const firstSkill = (body.firstSkill ?? '').toString().trim();
        const secondSkill = (body.secondSkill ?? '').toString().trim();
        const elapsedMs = Number(body.elapsedMs ?? 0);
        const partySize = Number(body.partySize ?? 1);

        if (!playerGuid || !isGuid(playerGuid) || !firstSkill || !secondSkill ||
            !Number.isInteger(elapsedMs) || !Number.isInteger(partySize) ||
            elapsedMs <= 0 || partySize <= 0) {
          return reply.status(400).send({error: 'invalid_argument'});
        }

        const session = getSessionOrReply(sessionId, reply);
        if (!session) return reply;

        await ensureBuildPlayerBound(session, nativeEngine, playerGuid);
        const combo = await nativeEngine.evaluatePlayerCombo(
            playerGuid, firstSkill, secondSkill, elapsedMs, partySize);

        return reply.status(200).send({
          playerGuid,
          firstSkill,
          secondSkill,
          elapsedMs,
          partySize,
          combo,
        });
      });

  app.post('/api/game/session/heartbeat/transport', async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      heartbeatSequence?: number;
    };
    const sessionId = resolveSessionId(body.sessionId);
    const heartbeatSequence = Number(body.heartbeatSequence ?? 0);

    if (!Number.isFinite(heartbeatSequence) || heartbeatSequence <= 0) {
      return reply.status(400).send({error: 'invalid_argument'});
    }

    const session = getSessionOrReply(sessionId, reply);
    if (!session) return reply;

    session.heartbeatLayers.transport.lastSequence = Math.max(
        session.heartbeatLayers.transport.lastSequence, heartbeatSequence);
    session.heartbeatLayers.transport.lastAt = Date.now();
    return reply.status(202).send({accepted: true, layer: 'transport'});
  });

  app.post('/api/game/session/heartbeat/usermode', async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      heartbeatSequence?: number;
      simulationTick?: number;
      debuggerPresent?: boolean;
      codeIntegrityViolations?: number;
      memoryMapAnomalies?: number;
      timingAnomalies?: number;
    };

    const sessionId = resolveSessionId(body.sessionId);
    const heartbeatSequence = Number(body.heartbeatSequence ?? 0);
    const simulationTick = Number(body.simulationTick ?? 0);
    if (!Number.isFinite(heartbeatSequence) || heartbeatSequence <= 0 ||
        !Number.isFinite(simulationTick) || simulationTick < 0) {
      return reply.status(400).send({error: 'invalid_argument'});
    }

    const session = getSessionOrReply(sessionId, reply);
    if (!session) return reply;

    const result = await antiCheatAdapter.submitUsermodeHeartbeat({
      sessionId,
      heartbeatSequence,
      simulationTick,
      debuggerPresent: Boolean(body.debuggerPresent),
      codeIntegrityViolations: Number(body.codeIntegrityViolations ?? 0),
      memoryMapAnomalies: Number(body.memoryMapAnomalies ?? 0),
      timingAnomalies: Number(body.timingAnomalies ?? 0),
    });

    session.lastTick = Math.max(session.lastTick, simulationTick);
    session.lastSequence = Math.max(session.lastSequence, heartbeatSequence);
    session.heartbeatLayers.session.lastSequence = Math.max(
        session.heartbeatLayers.session.lastSequence, heartbeatSequence);
    session.heartbeatLayers.integrity.lastSequence = Math.max(
        session.heartbeatLayers.integrity.lastSequence, heartbeatSequence);
    session.heartbeatLayers.session.lastAt = Date.now();
    session.heartbeatLayers.integrity.lastAt = Date.now();

    appendSnapshot(session, {
      at: Date.now(),
      score: result.score,
      flagged: result.flagged,
      integrityHash: result.integrityHash,
      source: 'usermode',
    });

    return reply.status(202).send(
        {accepted: true, layer: 'session', antiCheat: result});
  });

  app.post('/api/game/session/heartbeat/driver', async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      heartbeatSequence?: number;
      suspiciousRing12DriverCount?: number;
      unsignedDriverCount?: number;
      hiddenModuleCount?: number;
    };

    const sessionId = resolveSessionId(body.sessionId);
    const heartbeatSequence = Number(body.heartbeatSequence ?? 0);
    if (!Number.isFinite(heartbeatSequence) || heartbeatSequence <= 0) {
      return reply.status(400).send({error: 'invalid_argument'});
    }

    const session = getSessionOrReply(sessionId, reply);
    if (!session) return reply;

    const result = await antiCheatAdapter.submitDriverTelemetry({
      sessionId,
      heartbeatSequence,
      suspiciousRing12DriverCount:
          Number(body.suspiciousRing12DriverCount ?? 0),
      unsignedDriverCount: Number(body.unsignedDriverCount ?? 0),
      hiddenModuleCount: Number(body.hiddenModuleCount ?? 0),
    });

    session.lastSequence = Math.max(session.lastSequence, heartbeatSequence);
    session.heartbeatLayers.integrity.lastSequence = Math.max(
        session.heartbeatLayers.integrity.lastSequence, heartbeatSequence);
    session.heartbeatLayers.integrity.lastAt = Date.now();
    appendSnapshot(session, {
      at: Date.now(),
      score: result.score,
      flagged: result.flagged,
      integrityHash: result.integrityHash,
      source: 'driver',
    });

    return reply.status(202).send(
        {accepted: true, layer: 'integrity', antiCheat: result});
  });

  app.get('/api/game/session/status', async (request, reply) => {
    const sessionId =
        resolveSessionId((request.query as {sessionId?: string}).sessionId);

    const session = sessions.get(sessionId);
    if (!session) {
      return reply.status(404).send({error: 'session_not_found'});
    }

    const antiCheat = await antiCheatAdapter.getSessionStatus(sessionId);
    return reply.status(200).send({
      sessionId,
      replayCursor: {
        lastTick: session.lastTick,
        lastSequence: session.lastSequence,
      },
      heartbeatLayers: session.heartbeatLayers,
      antiCheat,
      antiCheatSnapshots: session.antiCheatSnapshots,
      workflowEvents: session.workflowEvents,
      players: listPlayers(session),
      worldState: session.lastSnapshot,
      server: {
        targetTps: session.serverMetrics.targetTps,
        currentTps: session.serverMetrics.currentTps,
        averageTickMs: session.serverMetrics.averageTickMs,
        lagMs: session.serverMetrics.lagMs,
        lastTickAt: session.serverMetrics.lastTickAt,
      },
      mode: 'overworld',
    });
  });

  app.get('/api/game/tools/world/stats', async (request, reply) => {
    const sessionId =
        resolveSessionId((request.query as {sessionId?: string}).sessionId);

    const session = sessions.get(sessionId);
    if (!session) {
      return reply.status(404).send({error: 'session_not_found'});
    }

    return reply.status(200).send(buildWorldStatsEnvelope(session));
  });

  app.post('/api/game/tools/workflows/dispatch', async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      workflowId?: string;
      workflowName?: string;
      issuedBy?: string;
      patch?: Record<string, unknown>;
    };

    const sessionId = resolveSessionId(body.sessionId);
    const workflowName = (body.workflowName ?? '').toString().trim();
    if (!workflowName) {
      return reply.status(400).send({error: 'invalid_argument'});
    }

    const patch = typeof body.patch === 'object' && body.patch !== null ?
        body.patch :
        {};

    const session = getSessionOrReply(sessionId, reply);
    if (!session) return reply;

    const event = {
      workflowId: (body.workflowId ?? '').toString().trim() || randomUUID(),
      workflowName,
      issuedBy: (body.issuedBy ?? 'server').toString().trim() || 'server',
      patch,
      createdAt: Date.now(),
    };

    appendWorkflowEvent(session, event);
    broadcastJson(session, {
      type: 'server_workflow',
      sessionId: session.sessionId,
      workflow: event,
    });

    return reply.status(202).send({accepted: true, workflow: event});
  });

  app.get('/api/game/realtime', async (request, reply) => {
    const requestedUpgrade =
        (request.headers.upgrade ?? '').toString().toLowerCase();

    if (requestedUpgrade === 'websocket') {
      return reply.status(101).send();
    }

    return reply.status(426).send({
      error: 'upgrade_required',
      detail: 'Use WebSocket upgrade on /game-session.',
    });
  });
}
