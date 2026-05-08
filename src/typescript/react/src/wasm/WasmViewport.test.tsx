// @ts-nocheck -- bun:test types not in app tsconfig; behavior asserted via DOM.
//
// Spec 261 T010: Lifecycle transition contract tests.
// Validates booting → degraded on fetch failure and banner/retry rendering.

import { afterEach, beforeEach, describe, expect, test } from "bun:test";
import { act, cleanup, render, screen, waitFor } from "@testing-library/react";

import { WasmViewport } from "./WasmViewport";

// ── Minimal fetch mocks ───────────────────────────────────────────────────────

function fetchReject(message: string): typeof fetch {
  return () => Promise.reject(new Error(message)) as unknown as Promise<Response>;
}

function fetchStatus(status: number): typeof fetch {
  return () => Promise.resolve(new Response(null, { status })) as unknown as Promise<Response>;
}

// ── Test setup ───────────────────────────────────────────────────────────────

let originalFetch: typeof fetch;

beforeEach(() => {
  originalFetch = globalThis.fetch;
  // Silence console errors from WASM fetch failures in test output
  globalThis.console.error = () => {};
});

afterEach(() => {
  globalThis.fetch = originalFetch;
  cleanup();
});

// ── T010 Lifecycle contract tests ─────────────────────────────────────────────

describe("WasmViewport lifecycle transitions", () => {
  test("renders canvas with data-lifecycle=booting on mount", async () => {
    // Fetch never resolves → stays booting
    globalThis.fetch = () => new Promise(() => {}) as unknown as Promise<Response>;

    await act(async () => {
      render(<WasmViewport />);
    });

    const canvas = screen.getByTestId("wasm-canvas");
    expect(canvas.getAttribute("data-lifecycle")).toBe("booting");
  });

  test("transitions to degraded and shows error banner on fetch failure", async () => {
    globalThis.fetch = fetchReject("engine.wasm not found");

    const lifecycleStates: string[] = [];

    await act(async () => {
      render(
        <WasmViewport
          onLifecycle={(s) => {
            lifecycleStates.push(s);
          }}
        />
      );
    });

    // Wait for degraded banner to appear
    await waitFor(
      () => {
        expect(screen.queryByTestId("wasm-error-banner")).toBeTruthy();
      },
      { timeout: 3000 }
    );

    expect(lifecycleStates).toContain("degraded");
    expect(screen.getByTestId("wasm-error-message").textContent).toContain("engine.wasm not found");
  });

  test("shows retry button when maxRetries not reached", async () => {
    globalThis.fetch = fetchReject("net::ERR_CONNECTION_REFUSED");

    await act(async () => {
      render(<WasmViewport />);
    });

    await waitFor(() => expect(screen.queryByTestId("wasm-retry-btn")).toBeTruthy(), {
      timeout: 3000,
    });
  });

  test("shows API fallback button when onApiFallback prop is provided", async () => {
    globalThis.fetch = fetchReject("net::ERR_CONNECTION_REFUSED");
    let fallbackCalled = false;

    await act(async () => {
      render(
        <WasmViewport
          onApiFallback={() => {
            fallbackCalled = true;
          }}
        />
      );
    });

    await waitFor(() => expect(screen.queryByTestId("wasm-fallback-btn")).toBeTruthy(), {
      timeout: 3000,
    });

    // Click fallback
    await act(async () => {
      screen.getByTestId("wasm-fallback-btn").click();
    });

    expect(fallbackCalled).toBe(true);
  });

  test("canvas opacity decreases when degraded", async () => {
    globalThis.fetch = fetchReject("offline");

    await act(async () => {
      render(<WasmViewport />);
    });

    await waitFor(() => expect(screen.queryByTestId("wasm-error-banner")).toBeTruthy(), {
      timeout: 3000,
    });

    const canvas = screen.getByTestId("wasm-canvas");
    expect(canvas.style.opacity).toBe("0.3");
  });

  test("transitions to degraded on HTTP error response", async () => {
    globalThis.fetch = fetchStatus(404);

    await act(async () => {
      render(<WasmViewport />);
    });

    await waitFor(() => expect(screen.queryByTestId("wasm-error-banner")).toBeTruthy(), {
      timeout: 3000,
    });

    expect(screen.getByTestId("wasm-error-message").textContent).toContain("404");
  });
});
