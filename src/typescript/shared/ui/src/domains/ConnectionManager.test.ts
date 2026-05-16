import {describe, expect, it} from "bun:test";

import {ConnectionManager, type TransportClient} from "./ConnectionManager";

function createTransportMock(): TransportClient
{
    let open = false;
    return {
        async connect() {
            open = true;
        },
        disconnect() {
            open = false;
        },
        send() {
            // no-op
        },
        isOpen() {
            return open;
        },
    };
}

describe("ConnectionManager", () => {
    it("transitions to connected on start", async () => {
        const manager = new ConnectionManager(createTransportMock(), {
            heartbeatIntervalMs : 1000,
            reconnectBackoffMs : 100,
            maxReconnectAttempts : 2,
        });

        await manager.start();
        expect(manager.getState()).toBe("connected");

        manager.stop();
    });

    it("reconnects after disconnect while under max attempts", async () => {
        const manager = new ConnectionManager(createTransportMock(), {
            heartbeatIntervalMs : 1000,
            reconnectBackoffMs : 100,
            maxReconnectAttempts : 2,
        });

        await manager.start();
        await manager.handleDisconnect();

        expect(manager.getState()).toBe("connected");
        expect(manager.getReconnectAttempts()).toBe(1);

        manager.stop();
    });
});
