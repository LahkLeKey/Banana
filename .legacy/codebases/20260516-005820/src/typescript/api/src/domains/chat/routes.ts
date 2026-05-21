import { randomUUID } from "node:crypto";
import type { FastifyInstance, FastifyReply } from "fastify";
import { z } from "zod";

import { loadTrainedModel, scoreText } from "../not-banana/model";

type ChatRole = "user" | "assistant" | "system";
type ChatStatus = "accepted" | "complete";

type ChatMessage = {
  id: string;
  session_id: string;
  role: ChatRole;
  content: string;
  created_at: string;
  status: ChatStatus;
};

type SessionEcho = {
  id: string;
  platform: string;
  created_at: string;
  updated_at: string;
  message_count: number;
};

type SessionState = {
  id: string;
  platform: string;
  created_at: string;
  updated_at: string;
  message_sequence: number;
  messages: ChatMessage[];
  idempotency: Map<
    string,
    {
      user_message: ChatMessage;
      assistant_message: ChatMessage;
    }
  >;
};

const CreateSessionBody = z.object({
  platform: z.string().trim().min(1).max(64).default("web"),
  metadata: z.record(z.string(), z.string()).default({}),
});

const SessionParams = z.object({
  sessionId: z.string().trim().min(1).max(128),
});

const PostMessageBody = z.object({
  content: z.string().trim().min(1).max(2000),
  client_message_id: z.string().trim().min(1).max(128).optional(),
});

const sessions = new Map<string, SessionState>();

function clamp01(value: number): number {
  if (value < 0) return 0;
  if (value > 1) return 1;
  return value;
}

function bananaSignalScore(content: string): {
  score: number;
  matched: string[];
  modelSource: string;
  threshold: number;
} {
  const model = loadTrainedModel();
  const scored = scoreText(content, model);
  if (scored.token_count === 0) {
    return {
      score: 0.5,
      matched: [],
      modelSource: model.source,
      threshold: clamp01(model.recommended_threshold),
    };
  }
  return {
    score: clamp01(scored.banana_score),
    matched: scored.matched_banana_tokens,
    modelSource: model.source,
    threshold: clamp01(model.recommended_threshold),
  };
}

function buildAssistantReply(content: string): string {
  const signal = bananaSignalScore(content);
  const positiveThreshold = signal.threshold;
  const negativeThreshold = clamp01(1 - positiveThreshold);

  if (signal.score >= positiveThreshold) {
    const matched = signal.matched.length > 0 ? signal.matched.join(",") : "banana";
    return `banana-forward signal detected (${matched}) with confidence ${signal.score.toFixed(
      2
    )} (threshold=${positiveThreshold.toFixed(2)}) [model=${signal.modelSource}]`;
  }

  if (signal.score <= negativeThreshold) {
    return `not-banana signal is stronger right now (score=${signal.score.toFixed(
      2
    )}, threshold=${positiveThreshold.toFixed(
      2
    )}); include banana context for a fruit-focused answer [model=${signal.modelSource}]`;
  }

  return `mixed signal detected (banana_score=${signal.score.toFixed(
    2
  )}, threshold=${positiveThreshold.toFixed(
    2
  )}); ask a follow-up for clearer banana context [model=${signal.modelSource}]`;
}

function toSessionEcho(session: SessionState): SessionEcho {
  return {
    id: session.id,
    platform: session.platform,
    created_at: session.created_at,
    updated_at: session.updated_at,
    message_count: session.messages.length,
  };
}

function nowIso(): string {
  return new Date().toISOString();
}

function createMessage(
  session: SessionState,
  role: ChatRole,
  content: string,
  status: ChatStatus
): ChatMessage {
  session.message_sequence += 1;
  return {
    id: `${session.id}_m${String(session.message_sequence).padStart(4, "0")}`,
    session_id: session.id,
    role,
    content,
    created_at: nowIso(),
    status,
  };
}

function typedError(
  reply: FastifyReply,
  statusCode: number,
  code: string,
  message: string,
  retryable = false,
  issues?: unknown
) {
  return reply.status(statusCode).send({
    error: {
      code,
      message,
      retryable,
      retry_hint: retryable ? "retry_with_backoff" : "fix_request_then_retry",
    },
    ...(issues ? { issues } : {}),
  });
}

export function __resetChatRouteStateForTests() {
  sessions.clear();
}

export async function registerChatRoutes(app: FastifyInstance) {
  app.post("/chat/sessions", async (req, reply) => {
    const parsed = CreateSessionBody.safeParse(req.body ?? {});
    if (!parsed.success) {
      return typedError(
        reply,
        400,
        "invalid_argument",
        "invalid chat session payload",
        false,
        parsed.error.flatten()
      );
    }

    const createdAt = nowIso();
    const sessionId = `chat_${randomUUID()}`;
    const session: SessionState = {
      id: sessionId,
      platform: parsed.data.platform,
      created_at: createdAt,
      updated_at: createdAt,
      message_sequence: 0,
      messages: [],
      idempotency: new Map(),
    };

    const welcome = createMessage(
      session,
      "assistant",
      "banana assistant ready; send a message to classify banana context deterministically",
      "complete"
    );
    session.messages.push(welcome);
    session.updated_at = welcome.created_at;

    sessions.set(session.id, session);

    return reply.status(201).send({
      session: toSessionEcho(session),
      metadata: parsed.data.metadata,
      welcome_message: welcome,
    });
  });

  app.get("/chat/sessions/:sessionId", async (req, reply) => {
    const parsedParams = SessionParams.safeParse(req.params);
    if (!parsedParams.success) {
      return typedError(
        reply,
        400,
        "invalid_argument",
        "invalid session identifier",
        false,
        parsedParams.error.flatten()
      );
    }

    const session = sessions.get(parsedParams.data.sessionId);
    if (!session) {
      return typedError(reply, 404, "session_not_found", "chat session was not found", false);
    }

    return reply.status(200).send({
      session: toSessionEcho(session),
      messages: session.messages,
    });
  });

  app.post("/chat/sessions/:sessionId/messages", async (req, reply) => {
    const parsedParams = SessionParams.safeParse(req.params);
    if (!parsedParams.success) {
      return typedError(
        reply,
        400,
        "invalid_argument",
        "invalid session identifier",
        false,
        parsedParams.error.flatten()
      );
    }

    const parsedBody = PostMessageBody.safeParse(req.body ?? {});
    if (!parsedBody.success) {
      return typedError(
        reply,
        400,
        "invalid_argument",
        "invalid chat message payload",
        false,
        parsedBody.error.flatten()
      );
    }

    const session = sessions.get(parsedParams.data.sessionId);
    if (!session) {
      return typedError(reply, 404, "session_not_found", "chat session was not found", false);
    }

    const clientMessageId = parsedBody.data.client_message_id;
    if (clientMessageId && session.idempotency.has(clientMessageId)) {
      const existing = session.idempotency.get(clientMessageId)!;
      return reply.status(200).send({
        session_id: session.id,
        duplicate: true,
        user_message: existing.user_message,
        assistant_message: existing.assistant_message,
      });
    }

    const userMessage = createMessage(session, "user", parsedBody.data.content, "accepted");
    const assistantMessage = createMessage(
      session,
      "assistant",
      buildAssistantReply(parsedBody.data.content),
      "complete"
    );

    session.messages.push(userMessage, assistantMessage);
    session.updated_at = assistantMessage.created_at;

    if (clientMessageId) {
      session.idempotency.set(clientMessageId, {
        user_message: userMessage,
        assistant_message: assistantMessage,
      });
    }

    return reply.status(201).send({
      session_id: session.id,
      duplicate: false,
      user_message: userMessage,
      assistant_message: assistantMessage,
    });
  });
}
