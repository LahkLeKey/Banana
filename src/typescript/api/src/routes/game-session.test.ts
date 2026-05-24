import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';
import {createHash} from 'node:crypto';
import WebSocket, {type RawData} from 'ws';

import {InMemoryAntiCheatInteropAdapter} from '../lib/anti-cheat-interop.ts';
import {signToken} from '../middleware/auth.ts';
import {__resetAuthSessionStoreForTests, deriveDefaultSessionExpiry, getAuthSessionStore} from '../services/authSessionStore.ts';
import {InMemoryEngineService} from '../services/nativeEngine.ts';

import {__resetGameSessionStateForTests, registerGameSessionRoutes, validateInputCommandEnvelope,} from './game-session.ts';

async function createApp() {
  const app = Fastify({logger: false});
  __resetGameSessionStateForTests();
  __resetAuthSessionStoreForTests();
  await registerGameSessionRoutes(
      app, new InMemoryAntiCheatInteropAdapter(), new InMemoryEngineService());
  await app.ready();
  return app;
}

async function issueSteamAuth(steamId = '76561198000000000') {
  process.env.BANANA_JWT_SECRET =
      process.env.BANANA_JWT_SECRET ?? 'test-secret';
  const token = await signToken({sub: `steam:${steamId}`, role: 'viewer'});
  const store = await getAuthSessionStore();
  await store.upsertSteamUser(steamId);
  await store.createSession({
    steamId,
    tokenHash: createHash('sha256').update(token).digest('hex'),
    expiresAt: deriveDefaultSessionExpiry(),
  });

  return {
    steamId,
    token,
    headers: {authorization: `Bearer ${token}`},
  };
}

describe('game session routes', () => {
  test('starts and rejoins the static overworld session', async () => {
    const app = await createApp();
    const auth = await issueSteamAuth();

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      headers: auth.headers,
      payload: {playerName: 'tester'},
    });

    expect(start.statusCode).toBe(201);
    const started = start.json() as {
      sessionId: string;
      token: string;
      mode: string;
      steamId: string;
    };
    expect(started.sessionId).toBe('overworld');
    expect(started.token).toBe('gst_overworld');
    expect(started.mode).toBe('overworld');
    expect(started.steamId).toBe(auth.steamId);

    const rejoin = await app.inject({
      method: 'POST',
      url: '/api/game/session/rejoin',
      headers: auth.headers,
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
    const auth = await issueSteamAuth();

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      headers: auth.headers,
      payload: {playerName: 'ws-tester'},
    });
    expect(start.statusCode).toBe(201);

    const listenAddress = await app.listen({port: 0, host: '127.0.0.1'});
    const url = new URL(listenAddress);
    const wsUrl =
        `ws://${url.hostname}:${url.port}/game-session?playerName=ws-tester`;

    await new Promise<void>((resolve, reject) => {
      const socket = new WebSocket(wsUrl, {headers: auth.headers});
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
    const auth = await issueSteamAuth();

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      headers: auth.headers,
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
      const socket = new WebSocket(wsUrl, {headers: auth.headers});

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

      socket.on('message', (raw: RawData) => {
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

      socket.on('error', (error: Error) => {
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
        const auth = await issueSteamAuth();

        const start = await app.inject({
          method: 'POST',
          url: '/api/game/session/start',
          headers: auth.headers,
          payload: {playerName: 'heartbeat-tester'},
        });
        expect(start.statusCode).toBe(201);
        const started = start.json() as {sessionId: string};

        const transportHb = await app.inject({
          method: 'POST',
          url: '/api/game/session/heartbeat/transport',
          headers: auth.headers,
          payload: {sessionId: started.sessionId, heartbeatSequence: 1},
        });
        expect(transportHb.statusCode).toBe(202);

        const usermodeHb = await app.inject({
          method: 'POST',
          url: '/api/game/session/heartbeat/usermode',
          headers: auth.headers,
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
          headers: auth.headers,
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
          headers: auth.headers,
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

  test('exposes world stats for web tools', async () => {
    const app = await createApp();
    const auth = await issueSteamAuth();

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      headers: auth.headers,
      payload: {playerName: 'stats-tester'},
    });
    expect(start.statusCode).toBe(201);

    const stats = await app.inject({
      method: 'GET',
      url: '/api/game/tools/world/stats',
      headers: auth.headers,
    });
    expect(stats.statusCode).toBe(200);

    const payload = stats.json() as {
      sessionId: string;
      players: {connected: number; total: number};
      world: {entityCount: number};
      server: {targetTps: number; currentTps: number};
      workflows: {totalEvents: number; recent: Array<unknown>};
      mode: string;
    };

    expect(payload.sessionId).toBe('overworld');
    expect(payload.mode).toBe('overworld');
    expect(payload.players.total).toBeGreaterThanOrEqual(0);
    expect(payload.players.connected).toBeGreaterThanOrEqual(0);
    expect(payload.world.entityCount).toBeGreaterThanOrEqual(0);
    expect(payload.server.targetTps).toBeGreaterThan(0);
    expect(payload.server.currentTps).toBeGreaterThan(0);
    expect(payload.workflows.totalEvents).toBe(0);
    expect(payload.workflows.recent.length).toBe(0);

    await app.close();
  });

  test('dispatches server workflow events to websocket clients', async () => {
    const app = await createApp();
    const auth = await issueSteamAuth();

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      headers: auth.headers,
      payload: {playerName: 'workflow-tester'},
    });
    expect(start.statusCode).toBe(201);

    const listenAddress = await app.listen({port: 0, host: '127.0.0.1'});
    const httpUrl = new URL(listenAddress);
    const wsUrl = `ws://${httpUrl.hostname}:${
        httpUrl.port}/game-session?playerName=workflow-tester`;

    await new Promise<void>((resolve, reject) => {
      const timeout = setTimeout(() => {
        reject(new Error('timed out waiting for server workflow event'));
      }, 5000);

      const socket = new WebSocket(wsUrl, {headers: auth.headers});

      socket.on('message', async (raw: RawData) => {
        const payload = JSON.parse(raw.toString()) as | {type: 'connected'} | {
          type: 'server_workflow';
          sessionId: string;
          workflow: {workflowName: string; issuedBy: string};
        };

        if (payload.type === 'connected') {
          const dispatch = await app.inject({
            method: 'POST',
            url: '/api/game/tools/workflows/dispatch',
            headers: auth.headers,
            payload: {
              workflowName: 'double_xp_weekend',
              issuedBy: 'ops-console',
              patch: {xpMultiplier: 2},
            },
          });
          expect(dispatch.statusCode).toBe(202);
          return;
        }

        expect(payload.type).toBe('server_workflow');
        expect(payload.sessionId).toBe('overworld');
        expect(payload.workflow.workflowName).toBe('double_xp_weekend');
        expect(payload.workflow.issuedBy).toBe('ops-console');

        clearTimeout(timeout);
        socket.close();
        resolve();
      });

      socket.on('error', (error: Error) => {
        clearTimeout(timeout);
        reject(error);
      });
    });

    const stats = await app.inject({
      method: 'GET',
      url: '/api/game/tools/world/stats',
      headers: auth.headers,
    });
    expect(stats.statusCode).toBe(200);
    const statsPayload = stats.json() as {workflows: {totalEvents: number}};
    expect(statsPayload.workflows.totalEvents).toBeGreaterThanOrEqual(1);

    await app.close();
  });

  test(
      'supports build/class/gear/combo endpoints for a session player',
      async () => {
        const app = await createApp();
        const auth = await issueSteamAuth();

        const start = await app.inject({
          method: 'POST',
          url: '/api/game/session/start',
          headers: auth.headers,
          payload: {playerName: 'build-tester'},
        });
        expect(start.statusCode).toBe(201);
        const started = start.json() as {
          sessionId: string;
          playerGuid: string
        };

        const setClass = await app.inject({
          method: 'POST',
          url: '/api/game/session/player/build/class',
          headers: auth.headers,
          payload: {
            sessionId: started.sessionId,
            playerGuid: started.playerGuid,
            classType: 2,
          },
        });
        expect(setClass.statusCode).toBe(200);

        const setAllocations = await app.inject({
          method: 'POST',
          url: '/api/game/session/player/build/allocations',
          headers: auth.headers,
          payload: {
            sessionId: started.sessionId,
            playerGuid: started.playerGuid,
            offensePoints: 3,
            defensePoints: 1,
            utilityPoints: 2,
          },
        });
        expect(setAllocations.statusCode).toBe(200);

        const equip = await app.inject({
          method: 'POST',
          url: '/api/game/session/player/build/equip',
          headers: auth.headers,
          payload: {
            sessionId: started.sessionId,
            playerGuid: started.playerGuid,
            slot: 0,
            tier: 2,
            attackBonus: 4,
            defenseBonus: 0,
            utilityBonus: 0,
          },
        });
        expect(equip.statusCode).toBe(200);

        const stats = await app.inject({
          method: 'GET',
          url: `/api/game/session/player/build/stats?sessionId=${
              encodeURIComponent(started.sessionId)}&playerGuid=${
              encodeURIComponent(started.playerGuid)}`,
          headers: auth.headers,
        });
        expect(stats.statusCode).toBe(200);
        const statsPayload = stats.json() as {
          playerGuid: string;
          stats: {
            health: number; attack: number; defense: number; utility: number
          };
        };
        expect(statsPayload.playerGuid).toBe(started.playerGuid);
        expect(statsPayload.stats.attack).toBeGreaterThan(0);

        const combo = await app.inject({
          method: 'POST',
          url: '/api/game/session/player/combo/evaluate',
          headers: auth.headers,
          payload: {
            sessionId: started.sessionId,
            playerGuid: started.playerGuid,
            firstSkill: 'snare_shot',
            secondSkill: 'piercing_volley',
            elapsedMs: 1000,
            partySize: 3,
          },
        });
        expect(combo.statusCode).toBe(200);
        const comboPayload = combo.json() as {
          combo: {
            triggered: boolean; damageBonusPct: number;
            mitigationBonusPct: number;
            partySynergyBonusPct: number;
          };
        };
        expect(comboPayload.combo.triggered).toBe(true);
        expect(comboPayload.combo.damageBonusPct).toBeGreaterThan(0);
        expect(comboPayload.combo.partySynergyBonusPct)
            .toBeGreaterThanOrEqual(0);

        await app.close();
      });

  test('rejects invalid build endpoint payloads', async () => {
    const app = await createApp();
    const auth = await issueSteamAuth();

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      headers: auth.headers,
      payload: {playerName: 'invalid-build-tester'},
    });
    expect(start.statusCode).toBe(201);
    const started = start.json() as {sessionId: string};

    const badClass = await app.inject({
      method: 'POST',
      url: '/api/game/session/player/build/class',
      headers: auth.headers,
      payload: {
        sessionId: started.sessionId,
        playerGuid: 'not-a-guid',
        classType: 9,
      },
    });
    expect(badClass.statusCode).toBe(400);

    const badCombo = await app.inject({
      method: 'POST',
      url: '/api/game/session/player/combo/evaluate',
      headers: auth.headers,
      payload: {
        sessionId: started.sessionId,
        playerGuid: 'not-a-guid',
        firstSkill: '',
        secondSkill: '',
        elapsedMs: -1,
        partySize: 0,
      },
    });
    expect(badCombo.statusCode).toBe(400);

    const badAllocations = await app.inject({
      method: 'POST',
      url: '/api/game/session/player/build/allocations',
      headers: auth.headers,
      payload: {
        sessionId: started.sessionId,
        playerGuid: 'c9155c7f-a804-4f62-ae4f-c3db36410f50',
        offensePoints: 10,
        defensePoints: 10,
        utilityPoints: 1,
      },
    });
    expect(badAllocations.statusCode).toBe(400);

    const badEquip = await app.inject({
      method: 'POST',
      url: '/api/game/session/player/build/equip',
      headers: auth.headers,
      payload: {
        sessionId: started.sessionId,
        playerGuid: 'c9155c7f-a804-4f62-ae4f-c3db36410f50',
        slot: 0,
        tier: 1.5,
        attackBonus: 2,
        defenseBonus: 0,
        utilityBonus: 0,
      },
    });
    expect(badEquip.statusCode).toBe(400);

    const badComboFractions = await app.inject({
      method: 'POST',
      url: '/api/game/session/player/combo/evaluate',
      headers: auth.headers,
      payload: {
        sessionId: started.sessionId,
        playerGuid: 'c9155c7f-a804-4f62-ae4f-c3db36410f50',
        firstSkill: 'snare_shot',
        secondSkill: 'piercing_volley',
        elapsedMs: 1000.5,
        partySize: 2,
      },
    });
    expect(badComboFractions.statusCode).toBe(400);

    await app.close();
  });

  test('requires steam auth for gameplay session start', async () => {
    const app = await createApp();

    const response = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      payload: {playerName: 'unauthenticated'},
    });

    expect(response.statusCode).toBe(401);
    expect(response.json()).toEqual({error: 'steam_auth_required'});
    await app.close();
  });

  test('returns steam-account gameplay overview', async () => {
    const app = await createApp();
    const auth = await issueSteamAuth('76561198000012345');

    const start = await app.inject({
      method: 'POST',
      url: '/api/game/session/start',
      headers: auth.headers,
      payload: {playerName: 'account-tester'},
    });
    expect(start.statusCode).toBe(201);

    const overview = await app.inject({
      method: 'GET',
      url: '/api/game/account/overview',
      headers: auth.headers,
    });

    expect(overview.statusCode).toBe(200);
    const payload = overview.json() as {
      steamId: string;
      playerGuid: string;
      sessionId: string;
      buildStats:
          {health: number; attack: number; defense: number; utility: number};
      world: {entityCount: number};
      mode: string;
    };

    expect(payload.steamId).toBe('76561198000012345');
    expect(payload.sessionId).toBe('overworld');
    expect(payload.playerGuid)
        .toMatch(
            /^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);
    expect(payload.buildStats.health).toBeGreaterThan(0);
    expect(payload.world.entityCount).toBeGreaterThanOrEqual(0);
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
