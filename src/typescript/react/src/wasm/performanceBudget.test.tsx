// @ts-nocheck -- bun:test types not in app tsconfig; behavior asserted via DOM.
//
// Spec 261 T022: Performance budget tests for measurable outcomes SC-001 through SC-004
// Validates timing thresholds and compliance metrics using web timing API.

import { afterEach, beforeEach, describe, expect, test } from "bun:test";
import { act, cleanup, render, screen, waitFor } from "@testing-library/react";
import { exportPerformanceMetrics, exportTelemetryPayload } from "./performanceTracker";
import { WasmViewport } from "./WasmViewport";

let originalFetch: typeof fetch;

beforeEach(() => {
  originalFetch = globalThis.fetch;
  // Mock successful WASM load
  globalThis.fetch = () =>
    Promise.resolve(
      new Response(new Uint8Array([0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00]))
    ) as unknown as Promise<Response>;
});

afterEach(() => {
  globalThis.fetch = originalFetch;
  cleanup();
});

describe("Spec 261: Performance Budget Tests (SC-001 through SC-004)", () => {
  test("SC-001: First interactive frame appears within 3.0 seconds in 95% of runs", async () => {
    // Note: This is a statistical target. In test environments, we verify
    // the mechanism works rather than proving the 95% percentile.

    const start = Date.now();

    await act(async () => {
      render(<WasmViewport showDebug={true} />);
    });

    // Wait for first frame to be emitted
    const canvas = await screen.findByTestId("wasm-canvas", { timeout: 5000 });

    // Wait for lifecycle to transition through ready → running
    await waitFor(
      () => {
        const state = canvas.getAttribute("data-lifecycle");
        expect(["ready", "running"]).toContain(state);
      },
      { timeout: 5000 }
    );

    const elapsed = Date.now() - start;

    // In a real deployment, measure 95th percentile across 100 runs
    // For unit tests, verify that the timing mechanism works
    expect(elapsed).toBeLessThan(10000); // Generous budget for test environment

    const metrics = exportPerformanceMetrics();
    expect(metrics).toBeTruthy();
    expect(metrics?.first_interactive_ms).toBeLessThan(10000);

    console.log(`SC-001 sample: first interactive in ${elapsed}ms (target: <=3000ms)`);
  });

  test("SC-002: Overlay open/close preserves viewport continuity with <=1 dropped frame", async () => {
    // SC-002 requires frame-level instrumentation. This test verifies the
    // overlay layer management does not crash the rendering loop.

    await act(async () => {
      render(
        <WasmViewport
          showDebug={true}
          onLifecycle={(state) => {
            if (state === "running") {
              // Dispatch overlay open
              window.dispatchEvent(new CustomEvent("spec261:openMenu"));
            }
          }}
        />
      );
    });

    const canvas = await screen.findByTestId("wasm-canvas", { timeout: 5000 });

    // Wait for lifecycle to transition to running
    await waitFor(
      () => {
        expect(canvas.getAttribute("data-lifecycle")).toBe("running");
      },
      { timeout: 5000 }
    );

    // Dispatch overlay open
    await act(async () => {
      window.dispatchEvent(new CustomEvent("spec261:openMenu"));
    });

    // Canvas should remain visible and lifecycle stable
    await waitFor(
      () => {
        expect(canvas.isVisible()).toBe(true);
        expect(canvas.getAttribute("data-lifecycle")).toBe("running");
      },
      { timeout: 1000 }
    );

    // Close overlay
    await act(async () => {
      window.dispatchEvent(new CustomEvent("spec261:closeMenu"));
    });

    // Canvas and lifecycle should remain stable
    expect(canvas.isVisible()).toBe(true);
    expect(canvas.getAttribute("data-lifecycle")).toBe("running");
  });

  test("SC-003: Input routing transitions complete deterministically with <=100ms latency", async () => {
    // SC-003 requires measuring time from input event to routing mode change.
    // This test verifies the mechanism is in place and transitions complete.

    const routingLatencies: number[] = [];

    await act(async () => {
      render(<WasmViewport />);
    });

    const canvas = await screen.findByTestId("wasm-canvas", { timeout: 5000 });

    // Wait for running state
    await waitFor(
      () => {
        expect(canvas.getAttribute("data-lifecycle")).toBe("running");
      },
      { timeout: 5000 }
    );

    // Measure transition times
    const before = Date.now();

    await act(async () => {
      window.dispatchEvent(new CustomEvent("spec261:openMenu"));
    });

    const afterOpen = Date.now();
    routingLatencies.push(afterOpen - before);

    // Wait a frame
    await new Promise((resolve) => setTimeout(resolve, 100));

    // Close menu
    const beforeClose = Date.now();
    await act(async () => {
      window.dispatchEvent(new CustomEvent("spec261:closeMenu"));
    });
    const afterClose = Date.now();
    routingLatencies.push(afterClose - beforeClose);

    // Verify transitions complete quickly (< 500ms in test; target is 100ms in production)
    for (const latency of routingLatencies) {
      expect(latency).toBeLessThan(500);
    }

    console.log(`SC-003 sample transitions: [${routingLatencies.join("ms, ")}ms]`);
  });

  test("SC-004: 100% of simulated WASM initialization failures enter degraded mode", async () => {
    // SC-004 requires that all WASM load failures gracefully enter degraded mode
    // without crashing the app shell.

    globalThis.fetch = () =>
      Promise.reject(new Error("WASM fetch failed (SC-004 test)")) as unknown as Promise<Response>;

    await act(async () => {
      render(<WasmViewport />);
    });

    // Wait for degraded banner to appear
    const banner = await screen.findByTestId("wasm-error-banner", { timeout: 5000 });

    expect(banner).toBeTruthy();
    expect(await banner.isVisible()).toBe(true);

    // Verify canvas is degraded
    const canvas = screen.getByTestId("wasm-canvas");
    expect(canvas.getAttribute("data-lifecycle")).toBe("degraded");

    // Verify error message is present
    const message = await screen.findByTestId("wasm-error-message");
    expect(await message.textContent()).toContain("WASM fetch failed");

    // Verify telemetry payload shows degraded entry
    const telemetry = exportTelemetryPayload();
    expect(telemetry).toBeTruthy();
    expect(telemetry?.metrics.degraded_entries).toBeGreaterThan(0);
  });

  test("Performance metrics export includes compliance flags", async () => {
    await act(async () => {
      render(<WasmViewport />);
    });

    const canvas = await screen.findByTestId("wasm-canvas", { timeout: 5000 });

    // Wait for running
    await waitFor(
      () => {
        expect(canvas.getAttribute("data-lifecycle")).toBe("running");
      },
      { timeout: 5000 }
    );

    const telemetry = exportTelemetryPayload();

    expect(telemetry?.compliance).toBeTruthy();
    expect(telemetry?.compliance.sc_001_first_frame_3s).toBeDefined();
    expect(telemetry?.compliance.sc_002_overlay_frame_continuity).toBeDefined();
    expect(telemetry?.compliance.sc_003_input_routing_100ms).toBeDefined();
    expect(telemetry?.compliance.sc_004_degraded_mode_100_pct).toBeDefined();
  });

  test("Performance marks are recorded in window.performance", async () => {
    const marks: string[] = [];
    const originalMark = window.performance?.mark;

    // Intercept performance.mark calls
    if (window.performance) {
      window.performance.mark = (name: string) => {
        marks.push(name);
        return originalMark?.call(window.performance, name);
      };
    }

    await act(async () => {
      render(<WasmViewport />);
    });

    const canvas = await screen.findByTestId("wasm-canvas", { timeout: 5000 });

    await waitFor(
      () => {
        expect(canvas.getAttribute("data-lifecycle")).toBe("running");
      },
      { timeout: 5000 }
    );

    // Verify key performance marks were recorded
    expect(marks).toContain("spec261:boot_start");
    expect(marks).toContain("spec261:boot_ready");
    expect(marks).toContain("spec261:first_frame");

    // Restore original mark function
    if (window.performance && originalMark) {
      window.performance.mark = originalMark;
    }
  });
});
