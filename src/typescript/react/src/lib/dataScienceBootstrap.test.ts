import {describe, expect, it} from "bun:test";

import {
    buildEndpointBootstrapCells,
    buildExecutedSpikeMap,
    extractEndpointOperations,
} from "./dataScienceBootstrap";

describe("dataScienceBootstrap", () => {
    it("extracts and sorts OpenAPI operations by group/path/method", () => {
        const document = {
            paths : {
                "/health" : {
                    get : {summary : "Health check", tags : [ "System" ]},
                },
                "/banana" : {
                    post : {operationId : "createBanana", tags : [ "Fruit" ]},
                    get : {summary : "List bananas", tags : [ "Fruit" ]},
                },
                "/ignored" : {
                    trace : {summary : "Ignore trace", tags : [ "System" ]},
                },
            },
        };

        const operations = extractEndpointOperations(document);

        expect(operations).toHaveLength(3);
        expect(operations.map((op) => `${op.group}:${op.path}:${op.method}`)).toEqual([
            "Fruit:/banana:GET",
            "Fruit:/banana:POST",
            "System:/health:GET",
        ]);
        expect(operations[1]?.summary).toBe("createBanana");
    });

    it("builds notebook bootstrap cells with endpoint catalog and helper code", () => {
        const operations = [
            {method : "GET", path : "/health", summary : "Health", group : "System"},
            {method : "POST", path : "/banana", summary : "Create", group : "Fruit"},
        ];

        const cells = buildEndpointBootstrapCells(operations, "https://api.example.test/");

        expect(cells.length).toBeGreaterThanOrEqual(6);
        expect(cells[0]?.kind).toBe("markdown");
        expect(cells[0]?.source).toContain("Banana API Bootstrap Workspace");
        expect(cells[0]?.source).toContain("GET /health (System) - Health");
        expect(cells[1]?.kind).toBe("python");
        expect(cells[1]?.source).toContain('API_BASE = "https://api.example.test"');
        expect(cells[1]?.source).toContain("endpoint_catalog");
        expect(cells[2]?.source)
            .toContain("def call_endpoint(method, path, payload=None, headers=None):");
    });

    it("marks all follow-up spikes as executed", () => {
        const status = buildExecutedSpikeMap();

        expect(Object.keys(status).length).toBeGreaterThan(0);
        expect(status["DS-201"]).toBe("executed");
        expect(status["DS-226"]).toBe("executed");
    });
});
