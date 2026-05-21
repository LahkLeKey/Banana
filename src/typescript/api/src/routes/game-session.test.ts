import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';
import WebSocket from 'ws';

import {InMemoryAntiCheatInteropAdapter} from '../lib/anti-cheat-interop.ts';
import {InMemoryEngineService} from '../services/nativeEngine.ts';

import {__resetGameSessionStateForTests, registerGameSessionRoutes, validateInputCommandEnvelope,} from './game-session.ts';

async function createApp() {
  const app = Fastify({logger: false});
  __resetGameSessionStateForTests();
  await registerGameSessionRoutes(
      app, new InMemoryAntiCheatInteropAdapter(), new InMemoryEngineService());
  await app.ready();
  return app;
}

describe('game session routes', () => {
  test('starts and rejoins the static overworld session', async () => {
    const app = await createApp();

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      payload: {playerName: 'tester'},
    });

    expect(start.statusCode).toBe(201);
    const started = start.json() as {
      sessionId: string;
      token: string;
      mode: string;
    };
    expect(started.sessionId).toBe('overworld');
    expect(started.token).toBe('gst_overworld');
    expect(started.mode).toBe('overworld');

    const rejoin = await app.inject({
      method: 'POST',
      url: '/api/game/session/rejoin',
      payload: {},
    });

    expect(rejoin.statusCode).toBe(200);
    const rejoined = rejoin.json() as {
      sessionId: string;
      mode: string
    };
    expect(rejoined.sessionId).toBe(started.sessionId);
    expect(rejoined.mode).toBe('overworld');

    await app.close();
  });

  test('accepts websocket connections on /game-session', async () => {
    const app = await createApp();

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      payload: {playerName: 'ws-tester'},
    });
    expect(start.statusCode).toBe(201);

    const listenAddress = await app.listen({port: 0, host: '127.0.0.1'});
    const url = new URL(listenAddress);
    const wsUrl =
        `ws://${url.hostname}:${url.port}/game-session?playerName=ws-tester`;

    await new Promise<void>((resolve, reject) => {
      const socket = new WebSocket(wsUrl);
      socket.on('open', () => {
        socket.close();
        resolve();
      });
      socket.on('error', reject);
    });

    await app.close();
  });

  test('broadcasts authoritative snapshots to websocket clients', async () => {
    const app = await createApp();

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      payload: {playerName: 'snapshot-tester'},
    });
    const started = start.json() as {sessionId: string};

    const listenAddress = await app.listen({port: 0, host: '127.0.0.1'});
    const httpUrl = new URL(listenAddress);
    const wsUrl =
        `ws://${httpUrl.hostname}:${httpUrl.port}/game-session?sessionId=${
            encodeURIComponent(started.sessionId)}&playerName=snapshot-tester`;

    await new Promise<void>((resolve, reject) => {
      const timeout = setTimeout(() => {
        reject(new Error('timed out waiting for snapshot broadcast'));
      }, 5000);
      const socket = new WebSocket(wsUrl);

      socket.on('open', () => {
        socket.send(JSON.stringify({
          type: 'input',
          sequence: 1,
          tick: 1,
          moveX: 1,
          moveZ: 0,
          timestamp: Date.now(),
        }));
      });

      socket.on('message', (raw) => {
        const payload = JSON.parse(raw.toString()) as | {type: 'connected'} | {
          type: 'snapshot';
          sessionId: string;
          players: Array<{playerName: string}>;
          server: {
            targetTps: number; currentTps: number; averageTickMs: number;
            lagMs: number;
          };
          snapshot: {
            tick: number; isAuthoritative: boolean;
            entities: Record<
                string,
                {
                  type?: string;
                  ownerPlayerId?: string;
                  ownerPlayerName?: string;
                  tooltipLabel?: string;
                }>;
          };
        };

        if (payload.type !== 'snapshot') {
          return;
        }

        expect(payload.sessionId).toBe(started.sessionId);
        expect(payload.players[0]?.playerName).toBe('snapshot-tester');
        expect(payload.snapshot.tick).toBeGreaterThan(0);
        expect(payload.snapshot.isAuthoritative).toBe(true);
        expect(Object.keys(payload.snapshot.entities).length)
            .toBeGreaterThan(0);
        expect(payload.server.targetTps).toBeGreaterThan(0);
        expect(payload.server.currentTps).toBeGreaterThan(0);
        expect(payload.server.averageTickMs).toBeGreaterThanOrEqual(0);
        expect(payload.server.lagMs).toBeGreaterThanOrEqual(0);

        const labeledPlayerEntity =
            Object.values(payload.snapshot.entities)
                .find(
                    (entity) => entity.type === 'player' &&
                        typeof entity.tooltipLabel === 'string');
        expect(labeledPlayerEntity).toBeDefined();
        expect(labeledPlayerEntity?.ownerPlayerId)
            .toMatch(
                /^[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);
        expect(labeledPlayerEntity?.ownerPlayerName).toBe('snapshot-tester');
        expect(labeledPlayerEntity?.tooltipLabel).toContain('snapshot-tester');

        clearTimeout(timeout);
        socket.close();
        resolve();
      });

      socket.on('error', (error) => {
        clearTimeout(timeout);
        reject(error);
      });
    });

    await app.close();
  });

  test(
      'records transport/session/integrity heartbeats and persists anti-cheat snapshots',
      async () => {
        const app = await createApp();

        const start = await app.inject({
          method: 'POST',
          url: '/api/game/session/start',
          payload: {playerName: 'heartbeat-tester'},
        });
        expect(start.statusCode).toBe(201);
        const started = start.json() as {sessionId: string};

        const transportHb = await app.inject({
          method: 'POST',
          url: '/api/game/session/heartbeat/transport',
          payload: {sessionId: started.sessionId, heartbeatSequence: 1},
        });
        expect(transportHb.statusCode).toBe(202);

        const usermodeHb = await app.inject({
          method: 'POST',
          url: '/api/game/session/heartbeat/usermode',
          payload: {
            sessionId: started.sessionId,
            heartbeatSequence: 2,
            simulationTick: 10,
            debuggerPresent: false,
            codeIntegrityViolations: 0,
            memoryMapAnomalies: 0,
            timingAnomalies: 0,
          },
        });
        expect(usermodeHb.statusCode).toBe(202);

        const driverHb = await app.inject({
          method: 'POST',
          url: '/api/game/session/heartbeat/driver',
          payload: {
            sessionId: started.sessionId,
            heartbeatSequence: 3,
            suspiciousRing12DriverCount: 1,
            unsignedDriverCount: 0,
            hiddenModuleCount: 0,
          },
        });
        expect(driverHb.statusCode).toBe(202);

        const status = await app.inject({
          method: 'GET',
          url: '/api/game/session/status',
        });
        expect(status.statusCode).toBe(200);
        const payload = status.json() as {
          heartbeatLayers: {
            transport: {lastSequence: number}; session: {lastSequence: number};
            integrity: {lastSequence: number};
          };
          antiCheat: {score: number; flagged: boolean; integrityHash: number};
          antiCheatSnapshots: Array<{source: string}>;
          server: {
            targetTps: number; currentTps: number; averageTickMs: number;
            lagMs: number;
          };
          mode: string;
        };

        expect(payload.heartbeatLayers.transport.lastSequence).toBe(1);
        expect(payload.heartbeatLayers.session.lastSequence).toBe(2);
        expect(payload.heartbeatLayers.integrity.lastSequence).toBe(3);
        expect(payload.antiCheat.score).toBeGreaterThanOrEqual(0);
        expect(payload.antiCheat.integrityHash).toBeGreaterThan(0);
        expect(payload.antiCheatSnapshots.length).toBeGreaterThanOrEqual(3);
        expect(payload.antiCheatSnapshots.some((s) => s.source === 'usermode'))
            .toBe(true);
        expect(payload.antiCheatSnapshots.some((s) => s.source === 'driver'))
            .toBe(true);
        expect(payload.server.targetTps).toBeGreaterThan(0);
        expect(payload.server.currentTps).toBeGreaterThan(0);
        expect(payload.server.averageTickMs).toBeGreaterThanOrEqual(0);
        expect(payload.server.lagMs).toBeGreaterThanOrEqual(0);
        expect(payload.mode).toBe('overworld');

        await app.close();
      });
});

describe('game input command invariants', () => {
  test('accepts monotonic sequence and non-decreasing tick', () => {
    const result = validateInputCommandEnvelope(
        {
          sequence: 2,
          tick: 12,
          moveX: 1,
          moveZ: 0,
          timestamp: Date.now(),
        },
        {sequence: 1, tick: 12});

    expect(result.ok).toBe(true);
  });

  test('rejects non-monotonic sequence and invalid axis', () => {
    const badSequence = validateInputCommandEnvelope(
        {
          sequence: 3,
          tick: 12,
          moveX: 0,
          moveZ: 1,
          timestamp: Date.now(),
        },
        {sequence: 3, tick: 11});
    expect(badSequence.ok).toBe(false);
    expect(badSequence.reason).toBe('sequence_non_monotonic');

    const badAxis = validateInputCommandEnvelope({
      sequence: 4,
      tick: 12,
      moveX: 2 as - 1 | 0 | 1,
      moveZ: 0,
      timestamp: Date.now(),
    });
    expect(badAxis.ok).toBe(false);
    expect(badAxis.reason).toBe('invalid_axis');
  });
});
