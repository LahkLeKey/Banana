import { afterEach, beforeEach, describe, expect, mock, test } from "bun:test";
import Fastify from "fastify";

import { registerRipenessRoutes } from "./ripeness.ts";

type FetchType = typeof globalThis.fetch;

let originalFetch: FetchType;

async function createApp() {
  const app = Fastify({ logger: false });
  await registerRipenessRoutes(app);
  await app.ready();
  return app;
}

describe("ripeness routes", () => {
  beforeEach(() => {
    originalFetch = globalThis.fetch;
  });

  afterEach(() => {
    globalThis.fetch = originalFetch;
    delete process.env.BANANA_ASPNET_API_BASE_URL;
  });

  test("returns typed ripeness payload for valid request", async () => {
    process.env.BANANA_ASPNET_API_BASE_URL = "http://aspnet.local";
    globalThis.fetch = mock(async (input, init) => {
      expect(String(input)).toBe("http://aspnet.local/ripeness/predict");
      expect(init?.method).toBe("POST");
      expect(init?.headers).toEqual({ "content-type": "application/json" });

      return new Response(
        JSON.stringify({
          label: "ripe",
          confidence: 0.8125,
          model: "regression",
        }),
        {
          status: 200,
          headers: { "content-type": "application/json" },
        }
      );
    }) as unknown as FetchType;

    const app = await createApp();

    const response = await app.inject({
      method: "POST",
      url: "/ripeness/predict",
      payload: { inputJson: "ripe yellow banana" },
    });

    expect(response.statusCode).toBe(200);
    const payload = response.json() as {
      label: string;
      confidence: number;
      model: string;
    };
    expect(payload.label).toBe("ripe");
    expect(payload.confidence).toBe(0.8125);
    expect(payload.model).toBe("regression");

    await app.close();
  });

  test("rejects invalid payload with 400 and does not call upstream", async () => {
    const fetchSpy = mock(async () => {
      throw new Error("upstream should not be called");
    });
    globalThis.fetch = fetchSpy as unknown as FetchType;

    const app = await createApp();

    const response = await app.inject({
      method: "POST",
      url: "/ripeness/predict",
      payload: { inputJson: "" },
    });

    expect(response.statusCode).toBe(400);
    const payload = response.json() as { error: string };
    expect(payload.error).toBe("invalid_argument");
    expect(fetchSpy).toHaveBeenCalledTimes(0);

    await app.close();
  });

  test("passes through upstream 5xx payload", async () => {
    globalThis.fetch = mock(
      async () =>
        new Response(
          JSON.stringify({
            error: "native_unavailable",
            remediation: "Set BANANA_NATIVE_PATH to a valid native library path.",
          }),
          {
            status: 503,
            headers: { "content-type": "application/json" },
          }
        )
    ) as unknown as FetchType;

    const app = await createApp();

    const response = await app.inject({
      method: "POST",
      url: "/ripeness/predict",
      payload: { inputJson: "green banana" },
    });

    expect(response.statusCode).toBe(503);
    const payload = response.json() as {
      error: string;
      remediation: string;
    };
    expect(payload.error).toBe("native_unavailable");
    expect(payload.remediation).toContain("BANANA_NATIVE_PATH");

    await app.close();
  });
});
