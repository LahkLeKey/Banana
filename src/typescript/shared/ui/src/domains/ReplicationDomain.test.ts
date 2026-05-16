import {beforeEach, describe, expect, it} from "bun:test";

import {ReplicationDomain, ReplicationDomainConfig} from "./ReplicationDomain";

describe("ReplicationDomain", () => {
    let replication: ReplicationDomain;
    beforeEach(() => {
        const config: ReplicationDomainConfig = {maxHistory : 3};
        replication = new ReplicationDomain(config);
    });

    it("records and retrieves state by tick", () => {
        replication.recordState(1, {
            tick : 1,
            timestamp : Date.now(),
            entities : {player : {x : 1}},
            isAuthoritative : true,
        });
        replication.recordState(2, {
            tick : 2,
            timestamp : Date.now(),
            entities : {player : {x : 2}},
            isAuthoritative : true,
        });
        expect(replication.getStateAtTick(1)?.state.entities.player).toEqual({x : 1});
        expect(replication.getStateAtTick(2)?.state.entities.player).toEqual({x : 2});
    });

    it("limits state history to maxHistory", () => {
        for (let tick = 1; tick <= 4; tick++)
        {
            replication.recordState(tick, {
                tick,
                timestamp : Date.now(),
                entities : {player : {x : tick}},
                isAuthoritative : true,
            });
        }
        expect(replication.getHistory().length).toBe(3);
        expect(replication.getStateAtTick(1)).toBeUndefined();
        expect(replication.getStateAtTick(2)?.state.entities.player).toEqual({x : 2});
    });

    it("gets latest state", () => {
        const next = replication.applyDelta({
            entityId : "player_1",
            tick : 1,
            changes : {x : 10, z : 20},
            timestamp : Date.now(),
        });
        expect(next.entities.player_1).toEqual({x : 10, z : 20});
        expect(replication.getLatestState()?.state.tick).toBe(1);
    });
});
