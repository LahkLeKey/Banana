/**
 * OperatorPage.tsx — operator chat interface backed by the live chat API.
 */
import { useEffect, useRef, useState } from "react";
import { Card, CardContent, CardHeader, CardTitle } from "../components/ui/card";
import { Textarea } from "../components/ui/textarea";
import { Button } from "../components/ui/button";
import { createChatSession, sendChatMessage, resolveApiBaseUrl, resolveChatApiBaseUrl } from "../lib/api";

type Message = { role: "user" | "assistant"; content: string };

export function OperatorPage() {
  const [sessionId, setSessionId] = useState<string | null>(null);
  const [messages, setMessages] = useState<Message[]>([]);
  const [input, setInput] = useState("");
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const bottomRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    async function bootstrap() {
      try {
        const base = resolveApiBaseUrl();
        const chatBase = resolveChatApiBaseUrl(base);
        const res = await createChatSession(base, "web", chatBase);
        setSessionId(res.session.id);
        setMessages([{ role: "assistant", content: res.welcome_message.content }]);
      } catch (e) {
        setError(e instanceof Error ? e.message : String(e));
      }
    }
    bootstrap();
  }, []);

  useEffect(() => {
    bottomRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages]);

  async function send() {
    if (!input.trim() || !sessionId) return;
    const text = input.trim();
    setInput("");
    setMessages((m) => [...m, { role: "user", content: text }]);
    setLoading(true);
    try {
      const base = resolveApiBaseUrl();
      const chatBase = resolveChatApiBaseUrl(base);
      const res = await sendChatMessage(base, sessionId, text, crypto.randomUUID(), chatBase);
      setMessages((m) => [...m, { role: "assistant", content: res.assistant_message.content }]);
    } catch (e) {
      setMessages((m) => [...m, { role: "assistant", content: `Error: ${e instanceof Error ? e.message : String(e)}` }]);
    } finally {
      setLoading(false);
    }
  }

  function handleKey(e: React.KeyboardEvent) {
    if (e.key === "Enter" && !e.shiftKey) { e.preventDefault(); send(); }
  }

  return (
    <div className="flex flex-col h-full gap-4">
      <Card>
        <CardHeader>
          <CardTitle>Operator</CardTitle>
        </CardHeader>
      </Card>

      {error && (
        <Card>
          <CardContent className="py-3 text-sm text-destructive">{error}</CardContent>
        </Card>
      )}

      <Card className="flex-1 overflow-hidden">
        <CardContent className="flex flex-col h-full p-4 gap-3">
          <div className="flex-1 overflow-y-auto space-y-3 pr-1">
            {messages.map((msg, i) => (
              <div key={i} className={`flex ${msg.role === "user" ? "justify-end" : "justify-start"}`}>
                <div className={`max-w-[80%] rounded-lg px-3 py-2 text-sm ${msg.role === "user"
                    ? "bg-primary text-primary-foreground"
                    : "bg-muted text-foreground"
                  }`}>
                  {msg.content}
                </div>
              </div>
            ))}
            {loading && (
              <div className="flex justify-start">
                <div className="rounded-lg bg-muted px-3 py-2 text-sm text-muted-foreground">…</div>
              </div>
            )}
            <div ref={bottomRef} />
          </div>

          <div className="flex gap-2 pt-2 border-t">
            <Textarea
              value={input}
              onChange={(e) => setInput(e.target.value)}
              onKeyDown={handleKey}
              placeholder={sessionId ? "Ask the operator… (Enter to send)" : "Connecting…"}
              rows={2}
              disabled={!sessionId || loading}
              className="flex-1 resize-none"
            />
            <Button onClick={send} disabled={!sessionId || !input.trim() || loading}>
              Send
            </Button>
          </div>
        </CardContent>
      </Card>
    </div>
  );
}
