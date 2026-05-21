// @banana/ui annotation: cross-platform.
// Slice 027 -- token-driven. Public props preserved.
import { tokens } from "../tokens";
import type { ChatMessage } from "../types";

type ChatRole = ChatMessage["role"];

function bubbleTone(role: ChatRole): {
  bg: string;
  fg: string;
  align: "flex-start" | "flex-end" | "center";
} {
  if (role === "user") {
    return { bg: tokens.color.banana.bg, fg: tokens.color.banana.fg, align: "flex-end" };
  }
  if (role === "assistant") {
    return { bg: tokens.color.surface.muted, fg: tokens.color.text.default, align: "flex-start" };
  }
  return { bg: tokens.color.surface.muted, fg: tokens.color.text.muted, align: "center" };
}

export function ChatMessageBubble({ message }: { message: ChatMessage }) {
  const tone = bubbleTone(message.role);
  return (
    <article
      style={{
        alignSelf: tone.align,
        maxWidth: 480,
        marginBlockEnd: tokens.space[2],
        borderRadius: tokens.radius.md,
        backgroundColor: tone.bg,
        color: tone.fg,
        paddingInline: tokens.space[3],
        paddingBlock: tokens.space[2],
        fontSize: tokens.font.size.sm,
      }}
    >
      <p style={{ margin: 0, whiteSpace: "pre-wrap", lineHeight: 1.4 }}>{message.content}</p>
      <p
        style={{
          margin: 0,
          marginBlockStart: tokens.space[1],
          fontSize: 10,
          textTransform: "uppercase",
          letterSpacing: 0.4,
          opacity: 0.7,
        }}
      >
        {message.role} - {message.status}
      </p>
    </article>
  );
}
