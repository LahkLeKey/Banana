import { describe, expect, test } from "bun:test";
import Fastify from "fastify";

import { __resetCorpusRouteStateForTests, registerCorpusRoutes } from "./corpus.ts";

async function createApp() {
  process.env.BANANA_CORPUS_AUDIT_DISABLED = "1";
  const app = Fastify({ logger: false });
  __resetCorpusRouteStateForTests();
  await registerCorpusRoutes(app);
  await app.ready();
  return app;
}

describe("corpus routes", () => {
  test("accepts feedback and lists it", async () => {
    const app = await createApp();

    const post = await app.inject({
      method: "POST",
      url: "/corpus/feedback",
      payload: {
        text: "ripe banana crate from harvest lane",
        label: "banana",
      },
    });

    expect(post.statusCode).toBe(202);
    const postPayload = post.json() as {
      accepted: { id: string; label: string; source: string };
      persistence: { mode: string; audit_written: boolean };
    };
    expect(postPayload.accepted.id).toStartWith("feedback_");
    expect(postPayload.accepted.label).toBe("banana");
    expect(postPayload.accepted.source).toBe("user");
    expect(
      postPayload.persistence.mode === "memory" || postPayload.persistence.mode === "jsonl-audit"
    ).toBe(true);

    const list = await app.inject({
      method: "GET",
      url: "/corpus/feedback?limit=10",
    });

    expect(list.statusCode).toBe(200);
    const listPayload = list.json() as {
      count: number;
      rows: Array<{ id: string; label: string }>;
    };
    expect(listPayload.count).toBe(1);
    expect(listPayload.rows[0]?.id).toBe(postPayload.accepted.id);
    expect(listPayload.rows[0]?.label).toBe("banana");

    await app.close();
  });

  test("rejects invalid payload", async () => {
    const app = await createApp();

    const res = await app.inject({
      method: "POST",
      url: "/corpus/feedback",
      payload: {
        text: "",
        label: "banana",
      },
    });

    expect(res.statusCode).toBe(400);
    const payload = res.json() as { error: string };
    expect(payload.error).toBe("invalid_argument");

    await app.close();
  });
});
