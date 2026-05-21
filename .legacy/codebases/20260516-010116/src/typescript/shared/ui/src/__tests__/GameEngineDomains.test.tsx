// @ts-nocheck -- bun:test types are not part of app tsconfig.

import { describe, expect, test } from "bun:test";
import {
    createOutcome,
    GameEngineUIService,
    InputAggregator,
    InteractionManager,
    ViewportSizer,
} from "@banana/ui";

describe("InputAggregator — DDD Input Domain", () => {
    test("aggregates keyboard input with priority over radial", () => {
        const agg = new InputAggregator();

        // Set up both keyboard and radial
        agg.updateKeyState("w", true);
        agg.updateRadialDirection("down");

        // Keyboard should win
        const movement = agg.computeMovement();
        expect(movement.moveX).toBe(0);
        expect(movement.moveZ).toBe(1);
        expect(movement.source).toBe("keyboard");
    });

    test("computes neutral movement when no input active", () => {
        const agg = new InputAggregator();
        const movement = agg.computeMovement();

        expect(movement.moveX).toBe(0);
        expect(movement.moveZ).toBe(0);
        expect(movement.source).toBe("none");
    });

    test("clears all inputs on demand", () => {
        const agg = new InputAggregator();
        agg.updateKeyState("w", true);
        agg.updateRadialDirection("up");

        agg.clearAllInputs();
        const movement = agg.computeMovement();

        expect(movement.source).toBe("none");
    });

    test("handles opposite directions canceling out (WASD simultaneous press)", () => {
        const agg = new InputAggregator();
        agg.updateKeyState("w", true);
        agg.updateKeyState("s", true);
        agg.updateKeyState("a", true);
        agg.updateKeyState("d", true);

        const movement = agg.computeMovement();
        expect(movement.moveX).toBe(0);
        expect(movement.moveZ).toBe(0);
    });
});

describe("InteractionManager — DDD Interaction Domain", () => {
    test("executes registered action handler", () => {
        const manager = new InteractionManager([{ label: "Test", actionId: "test-action" }]);
        let handlerCalled = false;

        manager.registerActionHandler("test-action", (actionId, x, y) => {
            handlerCalled = true;
            return createOutcome(actionId, x, y, true, "Action executed");
        });

        manager.openMenu(100, 100, 1280, 720);
        const outcome = manager.executeAction("test-action");

        expect(handlerCalled).toBe(true);
        expect(outcome.success).toBe(true);
    });

    test("closes menu after action execution", () => {
        const manager = new InteractionManager([]);
        manager.registerActionHandler("test", (id, x, y) => createOutcome(id, x, y, true, "OK"));

        manager.openMenu(100, 100, 1280, 720);
        expect(manager.getMenuState().visible).toBe(true);

        manager.executeAction("test");
        expect(manager.getMenuState().visible).toBe(false);
    });

    test("handles missing action handler gracefully", () => {
        const manager = new InteractionManager([]);
        manager.openMenu(100, 100, 1280, 720);

        const outcome = manager.executeAction("missing-action");
        expect(outcome.success).toBe(false);
        expect(outcome.message).toContain("not registered");
    });

    test("clamps menu position within viewport", () => {
        const manager = new InteractionManager([]);

        // Try to open menu at far right-bottom (should be clamped)
        manager.openMenu(1200, 900, 1280, 720);
        const state = manager.getMenuState();

        // Should be clamped within bounds
        expect(state.x).toBeLessThanOrEqual(1280 - 220 - 8);
        expect(state.y).toBeLessThanOrEqual(720 - 140 - 8);
    });
});

describe("ViewportSizer — DDD Viewport Domain", () => {
    test("provides interface for viewport size and coordinate normalization", () => {
        const sizer = new ViewportSizer();

        // Verify methods exist and are callable (DOM-dependent tests run in React)
        expect(typeof sizer.attachTo).toBe("function");
        expect(typeof sizer.computeSize).toBe("function");
        expect(typeof sizer.hasSizeChanged).toBe("function");
        expect(typeof sizer.getLastSize).toBe("function");
        expect(typeof sizer.normalizeCoordinate).toBe("function");
    });
});

describe("GameEngineUIService — DDD Application Service", () => {
    test("provides access to all domain entities", () => {
        // Create a minimal mock element (DOM tests run in React integration tests)
        const mockElement = {
            getBoundingClientRect: () => ({ width: 1280, height: 720, left: 0, top: 0 }),
        } as any;

        const service = new GameEngineUIService(mockElement, [{ label: "Test", actionId: "test" }]);

        expect(service.getInputAggregator()).toBeDefined();
        expect(service.getInteractionManager()).toBeDefined();
        expect(service.getViewportSizer()).toBeDefined();
    });

    test("exposes service interface for UI orchestration", () => {
        const mockElement = {} as any;
        const service = new GameEngineUIService(mockElement);

        // Verify all methods exist
        expect(typeof service.onStateChange).toBe("function");
        expect(typeof service.getState).toBe("function");
        expect(typeof service.setEngineStatus).toBe("function");
        expect(typeof service.executeMenuAction).toBe("function");
        expect(typeof service.openContextMenu).toBe("function");
        expect(typeof service.closeContextMenu).toBe("function");
        expect(typeof service.clearInput).toBe("function");
        expect(typeof service.checkViewportChange).toBe("function");
    });

    test("broadcasts viewport changes when the viewport size changes", () => {
        let width = 1280;
        let height = 720;
        const mockElement = {
            getBoundingClientRect: () => ({ width, height, left: 0, top: 0 }),
        } as any;

        const previousWindow = globalThis.window;
        const previousDocument = globalThis.document;

        (globalThis as any).window = {
            devicePixelRatio: 1,
            visualViewport: undefined,
        };
        (globalThis as any).document = {
            documentElement: {
                clientWidth: 1,
                clientHeight: 1,
            },
        };

        try {
            const service = new GameEngineUIService(mockElement);
            const states: GameEngineUIState[] = [];

            service.onStateChange((state) => {
                states.push(state);
            });

            expect(service.checkViewportChange()).toBe(true);
            expect(states).toHaveLength(1);
            expect(states[0].viewportSize.cssWidth).toBe(1280);
            expect(states[0].viewportSize.cssHeight).toBe(720);

            width = 1024;
            height = 768;

            expect(service.checkViewportChange()).toBe(true);
            expect(states).toHaveLength(2);
            expect(states[1].viewportSize.cssWidth).toBe(1024);
            expect(states[1].viewportSize.cssHeight).toBe(768);
        } finally {
            (globalThis as any).window = previousWindow;
            (globalThis as any).document = previousDocument;
        }
    });
});
