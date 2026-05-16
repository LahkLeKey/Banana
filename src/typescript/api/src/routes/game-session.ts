import type { FastifyInstance } from "fastify";

import {
  type AntiCheatInteropAdapter,
  createDefaultAntiCheatInteropAdapter,
} from "../lib/anti-cheat-interop.ts";

export interface GameInputCommandEnvelope {
  readonly sequence: number;
  readonly tick: number;
  readonly moveX: -1 | 0 | 1;
  readonly moveZ: -1 | 0 | 1;
  readonly timestamp: number;
}

export interface ValidationResult {
  readonly ok: boolean;
  readonly reason?:
    | "sequence_non_monotonic"
    | "tick_non_monotonic"
    | "invalid_axis"
    | "invalid_timestamp";
}

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
    source: "usermode" | "driver" | "session";
  }>;
};

const sessions = new Map<string, SessionRecord>();
const MAX_SNAPSHOTS = 64;

function makeSessionId(): string {
  return `session_${Date.now()}_${Math.random().toString(36).slice(2, 10)}`;
}

function makeSessionToken(): string {
  return `gst_${Math.random().toString(36).slice(2, 14)}`;
}

function isAxis(value: number): value is -1 | 0 | 1 {
  return value === -1 || value === 0 || value === 1;
}

function appendSnapshot(
  session: SessionRecord,
  snapshot: {
    at: number;
    score: number;
    flagged: boolean;
    integrityHash: number;
    source: "usermode" | "driver" | "session";
  }
): void {
  session.antiCheatSnapshots.push(snapshot);
  if (session.antiCheatSnapshots.length > MAX_SNAPSHOTS) {
    session.antiCheatSnapshots.shift();
  }
}

function getSessionOrReply(
  sessionId: string,
  reply: { status: (code: number) => { send: (body: unknown) => unknown } }
): SessionRecord | null {
  const session = sessions.get(sessionId);
  if (!session) {
    reply.status(404).send({ error: "session_not_found" });
    return null;
  }
  return session;
}

export function validateInputCommandEnvelope(
  command: GameInputCommandEnvelope,
  previous?: { sequence: number; tick: number }
): ValidationResult {
  if (!isAxis(command.moveX) || !isAxis(command.moveZ)) {
    return { ok: false, reason: "invalid_axis" };
  }

  if (!Number.isFinite(command.timestamp) || command.timestamp <= 0) {
    return { ok: false, reason: "invalid_timestamp" };
  }

  if (previous && command.sequence <= previous.sequence) {
    return { ok: false, reason: "sequence_non_monotonic" };
  }

  if (previous && command.tick < previous.tick) {
    return { ok: false, reason: "tick_non_monotonic" };
  }

  return { ok: true };
}

export function __resetGameSessionStateForTests(): void {
  sessions.clear();
}

export async function registerGameSessionRoutes(
  app: FastifyInstance,
  antiCheatAdapter: AntiCheatInteropAdapter = createDefaultAntiCheatInteropAdapter()
): Promise<void> {
  app.post("/api/game/session/start", async (request, reply) => {
    const body = (request.body ?? {}) as { playerName?: string };
    const playerName = (body.playerName ?? "player").toString().trim() || "player";

    const sessionId = makeSessionId();
    const token = makeSessionToken();
    const now = Date.now();

    await antiCheatAdapter.resetSession(sessionId);
    const status = await antiCheatAdapter.getSessionStatus(sessionId);

    const session: SessionRecord = {
      sessionId,
      token,
      playerName,
      lastTick: 0,
      lastSequence: 0,
      createdAt: now,
      heartbeatLayers: {
        transport: { lastSequence: 0, lastAt: null },
        session: { lastSequence: 0, lastAt: null },
        integrity: { lastSequence: 0, lastAt: null },
      },
      antiCheatSnapshots: [],
    };
    appendSnapshot(session, {
      at: now,
      score: status.score,
      flagged: status.flagged,
      integrityHash: status.integrityHash,
      source: "session",
    });
    sessions.set(sessionId, session);

    return reply.status(201).send({
      sessionId,
      token,
      worldState: {
        tick: 0,
        timestamp: now,
        entities: {},
        isAuthoritative: true,
      },
      antiCheat: status,
    });
  });

  app.post("/api/game/session/rejoin", async (request, reply) => {
    const body = (request.body ?? {}) as { sessionId?: string };
    const sessionId = (body.sessionId ?? "").toString().trim();
    if (!sessionId) {
      return reply.status(400).send({ error: "invalid_argument" });
    }

    const session = sessions.get(sessionId);
    if (!session) {
      return reply.status(404).send({ error: "session_not_found" });
    }

    const antiCheat = await antiCheatAdapter.getSessionStatus(sessionId);

    return reply.status(200).send({
      sessionId: session.sessionId,
      token: session.token,
      replayCursor: {
        lastTick: session.lastTick,
        lastSequence: session.lastSequence,
      },
      worldState: {
        tick: session.lastTick,
        timestamp: Date.now(),
        entities: {},
        isAuthoritative: true,
      },
      antiCheat,
      heartbeatLayers: session.heartbeatLayers,
      antiCheatSnapshots: session.antiCheatSnapshots,
    });
  });

  app.post("/api/game/session/end", async (request, reply) => {
    const body = (request.body ?? {}) as { sessionId?: string };
    const sessionId = (body.sessionId ?? "").toString().trim();
    if (!sessionId) {
      return reply.status(400).send({ error: "invalid_argument" });
    }

    const existed = sessions.delete(sessionId);
    return reply.status(200).send({ ended: existed, sessionId });
  });

  app.post("/api/game/session/heartbeat/transport", async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      heartbeatSequence?: number;
    };
    const sessionId = (body.sessionId ?? "").toString().trim();
    const heartbeatSequence = Number(body.heartbeatSequence ?? 0);

    if (!sessionId || !Number.isFinite(heartbeatSequence) || heartbeatSequence <= 0) {
      return reply.status(400).send({ error: "invalid_argument" });
    }

    const session = getSessionOrReply(sessionId, reply);
    if (!session) return reply;

    session.heartbeatLayers.transport.lastSequence = Math.max(
      session.heartbeatLayers.transport.lastSequence,
      heartbeatSequence
    );
    session.heartbeatLayers.transport.lastAt = Date.now();
    return reply.status(202).send({ accepted: true, layer: "transport" });
  });

  app.post("/api/game/session/heartbeat/usermode", async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      heartbeatSequence?: number;
      simulationTick?: number;
      debuggerPresent?: boolean;
      codeIntegrityViolations?: number;
      memoryMapAnomalies?: number;
      timingAnomalies?: number;
    };

    const sessionId = (body.sessionId ?? "").toString().trim();
    const heartbeatSequence = Number(body.heartbeatSequence ?? 0);
    const simulationTick = Number(body.simulationTick ?? 0);
    if (
      !sessionId ||
      !Number.isFinite(heartbeatSequence) ||
      heartbeatSequence <= 0 ||
      !Number.isFinite(simulationTick) ||
      simulationTick < 0
    ) {
      return reply.status(400).send({ error: "invalid_argument" });
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
      session.heartbeatLayers.session.lastSequence,
      heartbeatSequence
    );
    session.heartbeatLayers.integrity.lastSequence = Math.max(
      session.heartbeatLayers.integrity.lastSequence,
      heartbeatSequence
    );
    session.heartbeatLayers.session.lastAt = Date.now();
    session.heartbeatLayers.integrity.lastAt = Date.now();

    appendSnapshot(session, {
      at: Date.now(),
      score: result.score,
      flagged: result.flagged,
      integrityHash: result.integrityHash,
      source: "usermode",
    });

    return reply.status(202).send({ accepted: true, layer: "session", antiCheat: result });
  });

  app.post("/api/game/session/heartbeat/driver", async (request, reply) => {
    const body = (request.body ?? {}) as {
      sessionId?: string;
      heartbeatSequence?: number;
      suspiciousRing12DriverCount?: number;
      unsignedDriverCount?: number;
      hiddenModuleCount?: number;
    };

    const sessionId = (body.sessionId ?? "").toString().trim();
    const heartbeatSequence = Number(body.heartbeatSequence ?? 0);
    if (!sessionId || !Number.isFinite(heartbeatSequence) || heartbeatSequence <= 0) {
      return reply.status(400).send({ error: "invalid_argument" });
    }

    const session = getSessionOrReply(sessionId, reply);
    if (!session) return reply;

    const result = await antiCheatAdapter.submitDriverTelemetry({
      sessionId,
      heartbeatSequence,
      suspiciousRing12DriverCount: Number(body.suspiciousRing12DriverCount ?? 0),
      unsignedDriverCount: Number(body.unsignedDriverCount ?? 0),
      hiddenModuleCount: Number(body.hiddenModuleCount ?? 0),
    });

    session.lastSequence = Math.max(session.lastSequence, heartbeatSequence);
    session.heartbeatLayers.integrity.lastSequence = Math.max(
      session.heartbeatLayers.integrity.lastSequence,
      heartbeatSequence
    );
    session.heartbeatLayers.integrity.lastAt = Date.now();
    appendSnapshot(session, {
      at: Date.now(),
      score: result.score,
      flagged: result.flagged,
      integrityHash: result.integrityHash,
      source: "driver",
    });

    return reply.status(202).send({ accepted: true, layer: "integrity", antiCheat: result });
  });

  app.get("/api/game/session/status", async (request, reply) => {
    const sessionId = ((request.query as { sessionId?: string }).sessionId ?? "").toString().trim();
    if (!sessionId) {
      return reply.status(400).send({ error: "invalid_argument" });
    }

    const session = sessions.get(sessionId);
    if (!session) {
      return reply.status(404).send({ error: "session_not_found" });
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
    });
  });

  app.get("/api/game/realtime", async (request, reply) => {
    const requestedUpgrade = (request.headers.upgrade ?? "").toString().toLowerCase();

    if (requestedUpgrade === "websocket") {
      return reply.status(501).send({
        error: "websocket_not_enabled",
        detail:
          "Realtime WebSocket contract is reserved at /api/game/realtime but no WebSocket transport plugin is configured yet.",
      });
    }

    return reply.status(426).send({
      error: "upgrade_required",
      detail: "Use WebSocket upgrade on /api/game/realtime.",
    });
  });
}
