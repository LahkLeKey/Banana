import {describe, expect, test} from "bun:test";
import Fastify from "fastify";

import {
    __resetGameSessionStateForTests,
    registerGameSessionRoutes,
    validateInputCommandEnvelope,
} from "./game-session.ts";

async function createApp()
{
    const app = Fastify({logger : false});
    __resetGameSessionStateForTests();
    await registerGameSessionRoutes(app);
    await app.ready();
    return app;
}

describe("game session routes", () => {
    test("starts, rejoins, and ends a session", async () => {
        const app = await createApp();

        const start = await app.inject({
            method : "POST",
            url : "/api/game/session/start",
            payload : {playerName : "tester"},
        });

        expect(start.statusCode).toBe(201);
        const started = start.json() as
        {
            sessionId: string;
            token: string
        };
        expect(started.sessionId).toStartWith("session_");
        expect(started.token).toStartWith("gst_");

        const rejoin = await app.inject({
            method : "POST",
            url : "/api/game/session/rejoin",
            payload : {sessionId : started.sessionId},
        });

        expect(rejoin.statusCode).toBe(200);
        const rejoined = rejoin.json() as {sessionId : string};
        expect(rejoined.sessionId).toBe(started.sessionId);

        const end = await app.inject({
            method : "POST",
            url : "/api/game/session/end",
            payload : {sessionId : started.sessionId},
        });

        expect(end.statusCode).toBe(200);
        const ended = end.json() as {ended : boolean};
        expect(ended.ended).toBe(true);

        await app.close();
    });

    test("enforces realtime upgrade contract", async () => {
        const app = await createApp();

        const noUpgrade = await app.inject({method : "GET", url : "/api/game/realtime"});
        expect(noUpgrade.statusCode).toBe(426);

        const withUpgrade = await app.inject({
            method : "GET",
            url : "/api/game/realtime",
            headers : {upgrade : "websocket"},
        });
        expect(withUpgrade.statusCode).toBe(501);

        await app.close();
    });

    test("records transport/session/integrity heartbeats and persists anti-cheat snapshots",
         async () => {
             const app = await createApp();

             const start = await app.inject({
                 method : "POST",
                 url : "/api/game/session/start",
                 payload : {playerName : "heartbeat-tester"},
             });
             expect(start.statusCode).toBe(201);
             const started = start.json() as {sessionId : string};

             const transportHb = await app.inject({
                 method : "POST",
                 url : "/api/game/session/heartbeat/transport",
                 payload : {sessionId : started.sessionId, heartbeatSequence : 1},
             });
             expect(transportHb.statusCode).toBe(202);

             const usermodeHb = await app.inject({
                 method : "POST",
                 url : "/api/game/session/heartbeat/usermode",
                 payload : {
                     sessionId : started.sessionId,
                     heartbeatSequence : 2,
                     simulationTick : 10,
                     debuggerPresent : false,
                     codeIntegrityViolations : 0,
                     memoryMapAnomalies : 0,
                     timingAnomalies : 0,
                 },
             });
             expect(usermodeHb.statusCode).toBe(202);

             const driverHb = await app.inject({
                 method : "POST",
                 url : "/api/game/session/heartbeat/driver",
                 payload : {
                     sessionId : started.sessionId,
                     heartbeatSequence : 3,
                     suspiciousRing12DriverCount : 1,
                     unsignedDriverCount : 0,
                     hiddenModuleCount : 0,
                 },
             });
             expect(driverHb.statusCode).toBe(202);

             const status = await app.inject({
                 method : "GET",
                 url : `/api/game/session/status?sessionId=${started.sessionId}`,
             });
             expect(status.statusCode).toBe(200);
             const payload = status.json() as
             {
                 heartbeatLayers: {
                     transport: {lastSequence: number}; session : {lastSequence : number};
                     integrity : {lastSequence : number};
                 };
                 antiCheat: {score: number; flagged : boolean; integrityHash : number};
                 antiCheatSnapshots: Array<{source : string}>;
             };

             expect(payload.heartbeatLayers.transport.lastSequence).toBe(1);
             expect(payload.heartbeatLayers.session.lastSequence).toBe(2);
             expect(payload.heartbeatLayers.integrity.lastSequence).toBe(3);
             expect(payload.antiCheat.score).toBeGreaterThanOrEqual(0);
             expect(payload.antiCheat.integrityHash).toBeGreaterThan(0);
             expect(payload.antiCheatSnapshots.length).toBeGreaterThanOrEqual(3);
             expect(payload.antiCheatSnapshots.some((s) => s.source === "usermode")).toBe(true);
             expect(payload.antiCheatSnapshots.some((s) => s.source === "driver")).toBe(true);

             await app.close();
         });
});

describe("game input command invariants", () => {
    test("accepts monotonic sequence and non-decreasing tick", () => {
        const result = validateInputCommandEnvelope({
            sequence : 2,
            tick : 12,
            moveX : 1,
            moveZ : 0,
            timestamp : Date.now(),
        },
                                                    {sequence : 1, tick : 12});

        expect(result.ok).toBe(true);
    });

    test("rejects non-monotonic sequence and invalid axis", () => {
        const badSequence = validateInputCommandEnvelope({
            sequence : 3,
            tick : 12,
            moveX : 0,
            moveZ : 1,
            timestamp : Date.now(),
        },
                                                         {sequence : 3, tick : 11});
        expect(badSequence.ok).toBe(false);
        expect(badSequence.reason).toBe("sequence_non_monotonic");

        const badAxis = validateInputCommandEnvelope({
            sequence : 4,
            tick : 12,
            moveX : 2 as - 1 | 0 | 1,
            moveZ : 0,
            timestamp : Date.now(),
        });
        expect(badAxis.ok).toBe(false);
        expect(badAxis.reason).toBe("invalid_axis");
    });
});
