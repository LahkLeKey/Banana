import { afterEach, describe, expect, test } from "bun:test";
import { mkdtempSync, rmSync, writeFileSync } from "node:fs";
import { tmpdir } from "node:os";
import { join } from "node:path";
import Fastify from "fastify";

import { __resetTrainedModelCacheForTests } from "../not-banana/model.ts";

import { __resetChatRouteStateForTests, registerChatRoutes } from "./routes.ts";

function createTrainedArtifact(
  entries: Array<{ token: string; weight: number }>,
  minSignalScore: number
): { dir: string; filePath: string } {
  const dir = mkdtempSync(join(tmpdir(), "banana-chat-model-"));
  const filePath = join(dir, "vocabulary.json");
  writeFileSync(
    filePath,
    JSON.stringify({
      schema_version: 1,
      generated_at_utc: "2026-04-25T00:00:00Z",
      vocab_size: entries.length,
      vocabulary: entries.map((entry) => ({
        token: entry.token,
        banana_count: entry.weight > 0 ? 1 : 0,
        not_banana_count: entry.weight < 0 ? 1 : 0,
        support: 1,
        weight: entry.weight,
        signal: 1,
      })),
    }),
    "utf8"
  );

  writeFileSync(
    join(dir, "metrics.json"),
    JSON.stringify({
      schema_version: 1,
      generated_at_utc: "2026-04-25T00:00:00Z",
      metrics: {
        min_signal_score: minSignalScore,
        training_profile: "ci",
        session_mode: "single",
      },
    }),
    "utf8"
  );

  return { dir, filePath };
}

afterEach(() => {
  delete process.env.BANANA_NOT_BANANA_MODEL_PATH;
  __resetTrainedModelCacheForTests();
});

async function createApp() {
  const app = Fastify({ logger: false });
  __resetChatRouteStateForTests();
  __resetTrainedModelCacheForTests();
  await registerChatRoutes(app);
  await app.ready();
  return app;
}

describe("chat routes", () => {
  test("creates a session, posts a message, and replays idempotent duplicate", async () => {
    const app = await createApp();
    const sessionRes = await app.inject({
      method: "POST",
      url: "/chat/sessions",
      payload: { platform: "web" },
    });

    expect(sessionRes.statusCode).toBe(201);
    const sessionPayload = sessionRes.json() as {
      session: { id: string };
      welcome_message: { role: string };
    };
    expect(sessionPayload.session.id).toStartWith("chat_");
    expect(sessionPayload.welcome_message.role).toBe("assistant");

    const messageRes = await app.inject({
      method: "POST",
      url: `/chat/sessions/${sessionPayload.session.id}/messages`,
      payload: {
        content: "ripe banana bunch from harvest crate",
        client_message_id: "m1",
      },
    });

    expect(messageRes.statusCode).toBe(201);
    const messagePayload = messageRes.json() as {
      duplicate: boolean;
      user_message: { id: string; role: string };
      assistant_message: { id: string; role: string; content: string };
    };
    expect(messagePayload.duplicate).toBe(false);
    expect(messagePayload.user_message.role).toBe("user");
    expect(messagePayload.assistant_message.role).toBe("assistant");
    expect(messagePayload.assistant_message.content).toContain("banana");
    expect(messagePayload.assistant_message.content).toContain("[model=");

    const duplicateRes = await app.inject({
      method: "POST",
      url: `/chat/sessions/${sessionPayload.session.id}/messages`,
      payload: {
        content: "ripe banana bunch from harvest crate",
        client_message_id: "m1",
      },
    });

    expect(duplicateRes.statusCode).toBe(200);
    const duplicatePayload = duplicateRes.json() as {
      duplicate: boolean;
      user_message: { id: string };
      assistant_message: { id: string };
    };
    expect(duplicatePayload.duplicate).toBe(true);
    expect(duplicatePayload.user_message.id).toBe(messagePayload.user_message.id);
    expect(duplicatePayload.assistant_message.id).toBe(messagePayload.assistant_message.id);

    await app.close();
  });

  test("uses model-recommended threshold in assistant response", async () => {
    const artifact = createTrainedArtifact(
      [
        { token: "banana", weight: 1 },
        { token: "ripe", weight: 1 },
        { token: "plastic", weight: -1 },
      ],
      0.9
    );
    process.env.BANANA_NOT_BANANA_MODEL_PATH = artifact.filePath;

    const app = await createApp();

    try {
      const sessionRes = await app.inject({
        method: "POST",
        url: "/chat/sessions",
        payload: { platform: "web" },
      });
      const sessionPayload = sessionRes.json() as { session: { id: string } };

      const messageRes = await app.inject({
        method: "POST",
        url: `/chat/sessions/${sessionPayload.session.id}/messages`,
        payload: {
          content: "ripe banana plastic",
          client_message_id: "threshold-case",
        },
      });

      expect(messageRes.statusCode).toBe(201);
      const messagePayload = messageRes.json() as {
        assistant_message: { content: string };
      };
      expect(messagePayload.assistant_message.content).toContain("mixed signal detected");
      expect(messagePayload.assistant_message.content).toContain("threshold=0.90");
      expect(messagePayload.assistant_message.content).toContain("[model=trained-artifact]");
    } finally {
      await app.close();
      rmSync(artifact.dir, { recursive: true, force: true });
    }
  });

  test("returns typed error for missing session", async () => {
    const app = await createApp();
    const res = await app.inject({
      method: "GET",
      url: "/chat/sessions/unknown-session",
    });

    expect(res.statusCode).toBe(404);
    const payload = res.json() as { error: { code: string; retryable: boolean } };
    expect(payload.error.code).toBe("session_not_found");
    expect(payload.error.retryable).toBe(false);

    await app.close();
  });
});
