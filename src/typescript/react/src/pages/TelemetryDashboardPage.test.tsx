import { afterEach, beforeEach, describe, expect, test } from "bun:test";
import { cleanup, render, screen } from "@testing-library/react";

import { TelemetryDashboardPage } from "./TelemetryDashboardPage";
import { recordTelemetryEvent, resetTelemetryStore } from "../lib/telemetryStore";

describe("TelemetryDashboardPage", () => {
    beforeEach(() => {
        resetTelemetryStore();
    });

    afterEach(() => {
        cleanup();
        resetTelemetryStore();
    });

    test("renders telemetry dashboard and native unavailable state with no events", () => {
        render(<TelemetryDashboardPage autoHydrate={false} />);

        expect(screen.getByTestId("telemetry-dashboard-title").textContent).toContain(
            "Telemetry Dashboard"
        );
        expect(screen.getByTestId("native-unavailable").textContent).toContain(
            "Native observability data unavailable"
        );
        expect(screen.getByTestId("telemetry-api-hydration-status").textContent).toContain(
            "Waiting for API telemetry probe"
        );
    });

    test("renders wasm and native metrics when telemetry exists", () => {
        recordTelemetryEvent({
            source: "wasm-worker",
            event: "wasm.worker.init.ready",
            timestamp: Date.now(),
            status: "ok",
            durationMs: 210,
            variant: "simd",
        });
        recordTelemetryEvent({
            source: "wasm-worker",
            event: "wasm.worker.call.completed",
            timestamp: Date.now(),
            status: "ok",
            durationMs: 75,
            variant: "simd",
        });
        recordTelemetryEvent({
            source: "native",
            event: "native.wrapper.call.completed",
            timestamp: Date.now(),
            status: "ok",
            durationMs: 13,
            layer: "wrapper",
        });
        recordTelemetryEvent({
            source: "native",
            event: "native.core.eval.completed",
            timestamp: Date.now(),
            status: "ok",
            durationMs: 8,
            layer: "core",
        });

        render(<TelemetryDashboardPage autoHydrate={false} />);

        expect(screen.getByTestId("wasm-summary-count").textContent).toContain("2 events");
        expect(screen.getByTestId("native-summary-count").textContent).toContain("2 events");
        expect(screen.getByTestId("native-wrapper-count").textContent).toContain("1");
        expect(screen.getByTestId("native-core-count").textContent).toContain("1");
        expect(screen.getByTestId("wasm-p95-duration").textContent).toContain("75 ms");
    });
});
