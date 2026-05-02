/**
 * streaming-chat.ts — SSE chat streaming endpoint (spec #054).
 *
 * POST /api/v1/streaming/chat
 * Body: { session_id: string, content: string }
 *
 * Emits token-by-token:
 *   data: {"type":"token","token":"..."}
 *   data: {"type":"done"}
 */
import type { FastifyInstance } from "fastify";

function sseEvent(data: unknown): string {
  return `data: ${JSON.stringify(data)}\n\n`;
}

// Naive token splitter: split reply into word-size chunks to simulate streaming
function* tokenize(text: string): Generator<string> {
  const words = text.split(/(\s+)/);
  for (const word of words) {
    if (word.length > 0) yield word;
  }
}

export async function registerStreamingChatRoutes(app: FastifyInstance): Promise<void> {
  app.post("/api/v1/streaming/chat", async (request, reply) => {
    const body = request.body as { session_id?: string; content?: string };
    const content = (body?.content ?? "").trim();
    if (!content) {
      return reply.status(400).send({ error: "content is required" });
    }

    reply.raw.setHeader("Content-Type", "text/event-stream");
    reply.raw.setHeader("Cache-Control", "no-cache");
    reply.raw.setHeader("Connection", "keep-alive");
    reply.raw.setHeader("X-Accel-Buffering", "no");
    reply.raw.flushHeaders();

    // Delegate to the existing chat domain for the full reply then stream tokens
    let replyText = "";
    try {
      const chatRes = await (request.server as FastifyInstance).inject({
        method: "POST",
        url: "/chat/message",
        payload: { session_id: body.session_id, content },
        headers: {
          "content-type": "application/json",
          authorization: request.headers.authorization ?? "",
        },
      });
      if (chatRes.statusCode === 200) {
        const parsed = chatRes.json<{ reply?: string }>();
        replyText = parsed?.reply ?? "";
      }
    } catch {
      replyText = "";
    }

    for (const token of tokenize(replyText || "…")) {
      reply.raw.write(sseEvent({ type: "token", token }));
    }
    reply.raw.write(sseEvent({ type: "done" }));
    reply.raw.end();
    return reply;
  });
}
