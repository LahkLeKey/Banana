// @ts-nocheck -- bun:test types not in app tsconfig
//
// Spec 048 -- ErrorBoundary + non-empty-root invariants.
// The blank-page bug surfaces as an empty document body when a render
// error short-circuits the React tree. These tests assert that the
// boundary always renders a visible shadcn fallback and that a healthy
// render always contains at least one shadcn Card.

import { afterEach, beforeEach, describe, expect, test } from "bun:test";
import { act, cleanup, fireEvent, render, screen } from "@testing-library/react";

import { App } from "./App";
import { ErrorBoundary } from "./components/ErrorBoundary";

let originalFetch: typeof fetch;
let originalConsoleError: typeof console.error;

beforeEach(() => {
  (globalThis as any).window.banana = { apiBaseUrl: "http://api.example", platform: "" };
  originalFetch = globalThis.fetch;
  // Suppress React's expected error logging for boundary tests.
  originalConsoleError = console.error;
  console.error = () => undefined;
});

afterEach(() => {
  cleanup();
  globalThis.fetch = originalFetch;
  console.error = originalConsoleError;
  delete (globalThis as any).window.banana;
});

function jsonResponse(body: unknown, status = 200): Response {
  return new Response(JSON.stringify(body), {
    status,
    headers: { "content-type": "application/json" },
  });
}

function makeFetchMock(handler: (input: RequestInfo | URL) => Response): typeof fetch {
  return (async (input: RequestInfo | URL) => handler(input)) as typeof fetch;
}

function defaultBootstrap(input: RequestInfo | URL): Response {
  const url = String(input);
  if (url.endsWith("/banana?purchases=3&multiplier=2")) {
    return jsonResponse({ banana: 0 });
  }
  if (url.endsWith("/chat/sessions")) {
    return jsonResponse(
      {
        session: {
          id: "chat_1",
          platform: "web",
          created_at: "x",
          updated_at: "x",
          message_count: 1,
        },
        welcome_message: {
          id: "m0",
          session_id: "chat_1",
          role: "assistant",
          content: "hi",
          created_at: "x",
          status: "complete",
        },
      },
      201
    );
  }
  return jsonResponse({ error: { message: "unexpected " + url } }, 500);
}

async function flush() {
  for (let i = 0; i < 5; i += 1) {
    await act(async () => {
      await Promise.resolve();
    });
  }
}

describe("App shadcn baseline (spec 048)", () => {
  test("root contains at least one shadcn Card after mount", async () => {
    globalThis.fetch = makeFetchMock(defaultBootstrap);
    await act(async () => {
      render(<App />);
    });
    await flush();

    const cards = document.querySelectorAll('[data-slot="card"]');
    expect(cards.length).toBeGreaterThanOrEqual(3);
  });

  test("ensemble Predict button is the shadcn Button (h-10 px-4)", async () => {
    globalThis.fetch = makeFetchMock(defaultBootstrap);
    await act(async () => {
      render(<App />);
    });
    await flush();

    const submit = screen.getByRole("button", { name: /Predict ensemble/ });
    // shadcn default size: h-10 px-4 py-2
    expect(submit.className).toContain("h-10");
    expect(submit.className).toContain("px-4");
  });

  test("body remains non-empty after a failed ensemble submit", async () => {
    globalThis.fetch = makeFetchMock((input) => {
      const url = String(input);
      if (url.endsWith("/ml/ensemble/embedding")) {
        return jsonResponse({ error: { message: "down" } }, 503);
      }
      return defaultBootstrap(input);
    });
    await act(async () => {
      render(<App />);
    });
    await flush();

    const textarea = screen.getByPlaceholderText(
      "Describe a possible banana"
    ) as HTMLTextAreaElement;
    await act(async () => {
      fireEvent.change(textarea, { target: { value: "boom sample" } });
    });
    await act(async () => {
      fireEvent.click(screen.getByRole("button", { name: /Predict ensemble/ }));
    });
    await flush();

    expect(document.body.textContent?.length ?? 0).toBeGreaterThan(0);
    expect(document.querySelectorAll('[data-slot="card"]').length).toBeGreaterThanOrEqual(3);
  });
});

describe("ErrorBoundary (spec 048)", () => {
  function Boom(): never {
    throw new Error("synthetic render failure");
  }

  test("renders fallback Card with error message and retry when child throws", () => {
    render(
      <ErrorBoundary>
        <Boom />
      </ErrorBoundary>
    );

    expect(screen.getByTestId("app-error-boundary")).not.toBeNull();
    expect(screen.getByTestId("app-error-boundary-message").textContent).toContain(
      "synthetic render failure"
    );
    expect(screen.getByTestId("app-error-boundary-retry")).not.toBeNull();
    // Document body must remain non-empty (no blank page).
    expect(document.body.textContent?.length ?? 0).toBeGreaterThan(0);
    expect(document.querySelectorAll('[data-slot="card"]').length).toBeGreaterThanOrEqual(1);
  });

  test("retry resets the boundary and re-renders children", () => {
    let shouldThrow = true;
    function Conditional() {
      if (shouldThrow) {
        throw new Error("conditional failure");
      }
      return <div data-testid="recovered">recovered</div>;
    }

    render(
      <ErrorBoundary>
        <Conditional />
      </ErrorBoundary>
    );

    expect(screen.getByTestId("app-error-boundary")).not.toBeNull();

    shouldThrow = false;
    act(() => {
      fireEvent.click(screen.getByTestId("app-error-boundary-retry"));
    });

    expect(screen.queryByTestId("app-error-boundary")).toBeNull();
    expect(screen.getByTestId("recovered")).not.toBeNull();
  });
});
