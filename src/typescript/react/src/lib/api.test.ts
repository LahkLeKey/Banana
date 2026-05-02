import { afterEach, describe, expect, test } from "bun:test";

import {
  createChatSession,
  fetchBananaSummary,
  fetchEnsembleVerdict,
  fetchEnsembleVerdictWithEmbedding,
  resolveApiBaseResolution,
  resolveApiBaseResolutionError,
  resolveApiBaseUrl,
  resolveChatApiBaseUrl,
  resolveChatBootstrapError,
  resolvePlatformLabel,
  sendChatMessage,
} from "./api";

const originalFetch = globalThis.fetch;

afterEach(() => {
  globalThis.fetch = originalFetch;
});

describe("react api client", () => {
  test("resolves API base URL with env first, then bridge", () => {
    expect(
      resolveApiBaseUrl(
        "http://vite.example",
        {
          apiBaseUrl: "http://bridge.example",
          platform: "linux",
        },
        "http://localhost:8080"
      )
    ).toBe("http://vite.example");

    expect(
      resolveApiBaseUrl(
        "",
        {
          apiBaseUrl: "http://bridge.example",
          platform: "linux",
        },
        "http://localhost:8080"
      )
    ).toBe("http://bridge.example");

    expect(resolveApiBaseUrl("", undefined, "http://localhost:8080")).toBe("http://localhost:8080");
    expect(resolveApiBaseUrl("", undefined, "")).toBe("");
  });

  test("returns detailed API base resolution source", () => {
    expect(resolveApiBaseResolution("http://vite.example", undefined, "")).toEqual({
      baseUrl: "http://vite.example",
      error: null,
      source: "vite",
    });

    expect(
      resolveApiBaseResolution(
        "",
        {
          apiBaseUrl: "http://bridge.example",
          platform: "linux",
        },
        ""
      )
    ).toEqual({
      baseUrl: "http://bridge.example",
      error: null,
      source: "electron",
    });

    expect(resolveApiBaseResolution("", undefined, "http://localhost:8080")).toEqual({
      baseUrl: "http://localhost:8080",
      error: null,
      source: "localhost-default",
    });

    expect(resolveApiBaseResolution("", undefined, "").source).toBe("unresolved");
  });

  test("resolves chat API base with localhost fastify fallback", () => {
    expect(resolveChatApiBaseUrl("http://localhost:8080", "", undefined)).toBe(
      "http://localhost:8081"
    );
    expect(resolveChatApiBaseUrl("http://127.0.0.1:8080", "", undefined)).toBe(
      "http://127.0.0.1:8081"
    );
    expect(resolveChatApiBaseUrl("http://localhost:8080", "http://localhost:9081", undefined)).toBe(
      "http://localhost:9081"
    );
    expect(resolveChatApiBaseUrl("http://api.example", "", undefined)).toBe("http://api.example");
  });

  test("reports explicit API base configuration error when unresolved", () => {
    expect(
      resolveApiBaseResolutionError(
        "",
        {
          apiBaseUrl: "http://bridge.example",
          platform: "linux",
        },
        ""
      )
    ).toBeNull();

    expect(resolveApiBaseResolutionError("http://vite.example", undefined, "")).toBeNull();

    expect(resolveApiBaseResolutionError("", undefined, "http://localhost:8080")).toBeNull();

    expect(resolveApiBaseResolutionError("", undefined, "")).toContain("Missing API base URL");
  });

  test("resolves platform label from bridge", () => {
    expect(
      resolvePlatformLabel({
        apiBaseUrl: "http://bridge.example",
        platform: "win32",
      })
    ).toBe("electron-win32");

    expect(resolvePlatformLabel(undefined)).toBe("web");
  });

  test("classifies transport bootstrap failures with remediation", () => {
    expect(resolveChatBootstrapError(new Error("Failed to fetch"))).toEqual({
      kind: "transport_unreachable",
      message: "Chat bootstrap transport failure: the chat runtime endpoint could not be reached.",
      remediation:
        "Ensure the runtime profile is healthy and the apps profile is up, then retry chat bootstrap or reload.",
      retryable: true,
    });
  });

  test("classifies request bootstrap failures with remediation", () => {
    expect(resolveChatBootstrapError(new Error("request failed (404)"))).toEqual({
      kind: "request_failed",
      message: "Chat bootstrap request failed: request failed (404)",
      remediation:
        "Verify chat session routes are available for the configured API base, then retry.",
      retryable: true,
    });
  });

  test("fetchBananaSummary returns typed payload", async () => {
    globalThis.fetch = (async () =>
      new Response(JSON.stringify({ banana: 42 }), {
        status: 200,
        headers: { "content-type": "application/json" },
      })) as typeof fetch;

    const payload = await fetchBananaSummary("http://api.example");
    expect(payload.banana).toBe(42);
  });

  test("createChatSession posts platform metadata", async () => {
    let lastUrl = "";
    let lastInit: RequestInit | undefined;

    globalThis.fetch = (async (input: RequestInfo | URL, init?: RequestInit) => {
      lastUrl = String(input);
      lastInit = init;
      return new Response(
        JSON.stringify({
          session: {
            id: "chat_1",
            platform: "web",
            created_at: "2026-04-25T00:00:00Z",
            updated_at: "2026-04-25T00:00:00Z",
            message_count: 1,
          },
          welcome_message: {
            id: "chat_1_m0001",
            session_id: "chat_1",
            role: "assistant",
            content: "banana assistant ready",
            created_at: "2026-04-25T00:00:00Z",
            status: "complete",
          },
        }),
        {
          status: 201,
          headers: { "content-type": "application/json" },
        }
      );
    }) as typeof fetch;

    const payload = await createChatSession("http://api.example", "web");

    expect(payload.session.id).toBe("chat_1");
    expect(lastUrl).toBe("http://api.example/chat/sessions");
    expect(lastInit?.method).toBe("POST");

    const body = JSON.parse(String(lastInit?.body)) as {
      platform: string;
      metadata: { source: string };
    };
    expect(body.platform).toBe("web");
    expect(body.metadata.source).toBe("react-portal");
  });

  test("sendChatMessage posts content and idempotency key", async () => {
    let lastUrl = "";
    let lastInit: RequestInit | undefined;

    globalThis.fetch = (async (input: RequestInfo | URL, init?: RequestInit) => {
      lastUrl = String(input);
      lastInit = init;
      return new Response(
        JSON.stringify({
          session_id: "chat_1",
          duplicate: false,
          user_message: {
            id: "chat_1_m0002",
            session_id: "chat_1",
            role: "user",
            content: "banana question",
            created_at: "2026-04-25T00:00:00Z",
            status: "accepted",
          },
          assistant_message: {
            id: "chat_1_m0003",
            session_id: "chat_1",
            role: "assistant",
            content: "banana reply",
            created_at: "2026-04-25T00:00:00Z",
            status: "complete",
          },
        }),
        {
          status: 201,
          headers: { "content-type": "application/json" },
        }
      );
    }) as typeof fetch;

    const payload = await sendChatMessage(
      "http://api.example",
      "chat_1",
      "banana question",
      "client-1"
    );

    expect(payload.session_id).toBe("chat_1");
    expect(lastUrl).toBe("http://api.example/chat/sessions/chat_1/messages");
    expect(lastInit?.method).toBe("POST");

    const body = JSON.parse(String(lastInit?.body)) as {
      content: string;
      client_message_id: string;
    };
    expect(body.content).toBe("banana question");
    expect(body.client_message_id).toBe("client-1");
  });

  test("propagates API errors with typed message", async () => {
    globalThis.fetch = (async () =>
      new Response(JSON.stringify({ error: { message: "banana backend down" } }), {
        status: 503,
        headers: { "content-type": "application/json" },
      })) as typeof fetch;

    await expect(fetchBananaSummary("http://api.example")).rejects.toThrow("banana backend down");
  });

  test("fetchEnsembleVerdict snapshots the slice 014 contract shape", async () => {
    let lastUrl = "";
    let lastInit: RequestInit | undefined;

    globalThis.fetch = (async (input: RequestInfo | URL, init?: RequestInit) => {
      lastUrl = String(input);
      lastInit = init;
      return new Response(
        JSON.stringify({
          label: "banana",
          score: 0.83,
          did_escalate: true,
          calibration_magnitude: 0.6,
          status: "ok",
        }),
        {
          status: 200,
          headers: { "content-type": "application/json" },
        }
      );
    }) as typeof fetch;

    const verdict = await fetchEnsembleVerdict(
      "http://api.example",
      "yellow fruit on the counter maybe"
    );

    // Field-by-field snapshot: any drift in slice 014 wire shape breaks
    // this test (R-R03 contract).
    expect(Object.keys(verdict).sort()).toEqual([
      "calibration_magnitude",
      "did_escalate",
      "label",
      "score",
      "status",
    ]);
    expect(verdict.label).toBe("banana");
    expect(verdict.score).toBe(0.83);
    expect(verdict.did_escalate).toBe(true);
    expect(verdict.calibration_magnitude).toBe(0.6);
    expect(verdict.status).toBe("ok");

    expect(lastUrl).toBe("http://api.example/ml/ensemble");
    expect(lastInit?.method).toBe("POST");
    const body = JSON.parse(String(lastInit?.body)) as { inputJson: string };
    const inner = JSON.parse(body.inputJson) as { text: string };
    expect(inner.text).toBe("yellow fruit on the counter maybe");
  });

  test("fetchEnsembleVerdictWithEmbedding returns verdict and 4-dim embedding", async () => {
    let lastUrl = "";
    let lastInit: RequestInit | undefined;

    globalThis.fetch = (async (input: RequestInfo | URL, init?: RequestInit) => {
      lastUrl = String(input);
      lastInit = init;
      return new Response(
        JSON.stringify({
          verdict: {
            label: "banana",
            score: 0.83,
            did_escalate: true,
            calibration_magnitude: 0.6,
            status: "ok",
          },
          embedding: [0.42, -0.11, 0.07, 0.83],
        }),
        {
          status: 200,
          headers: { "content-type": "application/json" },
        }
      );
    }) as typeof fetch;

    const result = await fetchEnsembleVerdictWithEmbedding(
      "http://api.example",
      "yellow fruit on the counter maybe"
    );

    expect(result.verdict.label).toBe("banana");
    expect(result.verdict.did_escalate).toBe(true);
    expect(result.embedding).toEqual([0.42, -0.11, 0.07, 0.83]);

    expect(lastUrl).toBe("http://api.example/ml/ensemble/embedding");
    expect(lastInit?.method).toBe("POST");
    const body = JSON.parse(String(lastInit?.body)) as { inputJson: string };
    const inner = JSON.parse(body.inputJson) as { text: string };
    expect(inner.text).toBe("yellow fruit on the counter maybe");
  });

  test("fetchEnsembleVerdictWithEmbedding accepts a null embedding (cheap-path verdict)", async () => {
    globalThis.fetch = (async () =>
      new Response(
        JSON.stringify({
          verdict: {
            label: "banana",
            score: 0.95,
            did_escalate: false,
            calibration_magnitude: 0.7,
            status: "ok",
          },
          embedding: null,
        }),
        {
          status: 200,
          headers: { "content-type": "application/json" },
        }
      )) as typeof fetch;

    const result = await fetchEnsembleVerdictWithEmbedding("http://api.example", "ripe banana");

    expect(result.embedding).toBeNull();
    expect(result.verdict.did_escalate).toBe(false);
  });

  test("fetchEnsembleVerdictWithEmbedding propagates 5xx errors", async () => {
    globalThis.fetch = (async () =>
      new Response(JSON.stringify({ error: { message: "ensemble unavailable" } }), {
        status: 503,
        headers: { "content-type": "application/json" },
      })) as typeof fetch;

    await expect(
      fetchEnsembleVerdictWithEmbedding("http://api.example", "ambiguous")
    ).rejects.toThrow("ensemble unavailable");
  });
});
