/**
 * OperatorPage.tsx — operator chat interface backed by the live chat API.
 */
import { useEffect, useRef, useState } from "react";
import { Card, CardContent, CardHeader, CardTitle } from "../components/ui/card";
import { Textarea } from "../components/ui/textarea";
import { Button } from "../components/ui/button";
import { createChatSession, sendChatMessage, resolveApiBaseUrl, resolveChatApiBaseUrl } from "../lib/api";

type Message = { role: "user" | "assistant"; content: string };
type FailedTurn = { prompt: string; error: string };

const PINNED_SESSIONS_KEY = "banana.operator.pinned-sessions.v1";
const QUICK_COMMANDS = [
  "summarize current incident status",
  "show runbook steps for API outage",
  "diagnose recent model drift signals",
  "list next safe rollback actions",
];

export function OperatorPage() {
  const [telemetryConfig, setTelemetryConfig] = useState<{ sample_rate: number; unit: string } | null>(null);
  const [telemetryError, setTelemetryError] = useState<string | null>(null);
  const [sessionId, setSessionId] = useState<string | null>(null);
  const [recentSessions, setRecentSessions] = useState<string[]>([]);
  const [pinnedSessions, setPinnedSessions] = useState<string[]>([]);
  const [messages, setMessages] = useState<Message[]>([]);
  const [failedTurns, setFailedTurns] = useState<FailedTurn[]>([]);
  const [searchQuery, setSearchQuery] = useState("");
  const [roleFilter, setRoleFilter] = useState<"all" | "user" | "assistant">("all");
  const [input, setInput] = useState("");
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const bottomRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const raw = localStorage.getItem(PINNED_SESSIONS_KEY);
    if (!raw) return;
    try {
      const parsed = JSON.parse(raw) as string[];
      if (Array.isArray(parsed)) {
        setPinnedSessions(parsed);
      }
    } catch {
      setPinnedSessions([]);
    }
  }, []);

  useEffect(() => {
    localStorage.setItem(PINNED_SESSIONS_KEY, JSON.stringify(pinnedSessions));
  }, [pinnedSessions]);

  useEffect(() => {
    async function bootstrap() {
      try {
        const base = resolveApiBaseUrl();
        const chatBase = resolveChatApiBaseUrl(base);
        const res = await createChatSession(base, "web", chatBase);
        setSessionId(res.session.id);
        setRecentSessions((prev) => [res.session.id, ...prev.filter((s) => s !== res.session.id)].slice(0, 8));
        setMessages([{ role: "assistant", content: res.welcome_message.content }]);
      } catch (e) {
        setError(e instanceof Error ? e.message : String(e));
      }
    }
    bootstrap();
  }, []);

  useEffect(() => {
    async function loadTelemetryConfig() {
      try {
        const base = resolveApiBaseUrl();
        const response = await fetch(`${base}/operator/telemetry/config`);
        if (!response.ok) {
          throw new Error(`telemetry config returned ${response.status}`);
        }
        const data = (await response.json()) as { sample_rate: number; unit: string };
        setTelemetryConfig(data);
        setTelemetryError(null);
      } catch (e) {
        setTelemetryConfig(null);
        setTelemetryError(e instanceof Error ? e.message : String(e));
      }
    }

    loadTelemetryConfig();
  }, []);

  useEffect(() => {
    bottomRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages]);

  const tokenEstimate = Math.ceil(messages.reduce((sum, msg) => sum + msg.content.length, 0) / 4);

  const filteredMessages = messages.filter((msg) => {
    const matchesRole = roleFilter === "all" || msg.role === roleFilter;
    const matchesSearch = searchQuery.trim().length === 0
      || msg.content.toLowerCase().includes(searchQuery.trim().toLowerCase());
    return matchesRole && matchesSearch;
  });

  function pinCurrentSession() {
    if (!sessionId) return;
    setPinnedSessions((prev) => [sessionId, ...prev.filter((id) => id !== sessionId)].slice(0, 12));
  }

  function unpinSession(id: string) {
    setPinnedSessions((prev) => prev.filter((entry) => entry !== id));
  }

  async function retryFailedTurn(prompt: string) {
    setInput(prompt);
    if (!sessionId) return;
    const text = prompt.trim();
    setMessages((m) => [...m, { role: "user", content: text }]);
    setLoading(true);
    try {
      const base = resolveApiBaseUrl();
      const chatBase = resolveChatApiBaseUrl(base);
      const res = await sendChatMessage(base, sessionId, text, crypto.randomUUID(), chatBase);
      setMessages((m) => [...m, { role: "assistant", content: res.assistant_message.content }]);
    } catch (e) {
      const errText = e instanceof Error ? e.message : String(e);
      setMessages((m) => [...m, { role: "assistant", content: `Error: ${errText}` }]);
      setFailedTurns((prev) => [{ prompt: text, error: errText }, ...prev].slice(0, 10));
    } finally {
      setLoading(false);
    }
  }

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
      const errText = e instanceof Error ? e.message : String(e);
      setMessages((m) => [...m, { role: "assistant", content: `Error: ${errText}` }]);
      setFailedTurns((prev) => [{ prompt: text, error: errText }, ...prev].slice(0, 10));
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

      <Card>
        <CardHeader>
          <CardTitle className="text-base">Telemetry Sampling</CardTitle>
        </CardHeader>
        <CardContent className="text-sm">
          {telemetryConfig ? (
            <p>
              {telemetryConfig.sample_rate}
              {telemetryConfig.unit === "percent" ? "%" : ""}
            </p>
          ) : telemetryError ? (
            <p className="text-muted-foreground">Unable to load telemetry config: {telemetryError}</p>
          ) : (
            <p className="text-muted-foreground">Loading telemetry config…</p>
          )}
        </CardContent>
      </Card>

      <Card>
        <CardHeader>
          <CardTitle className="text-base">Operator Command Center</CardTitle>
        </CardHeader>
        <CardContent className="space-y-3 text-sm">
          <div className="flex flex-wrap items-center gap-2">
            <span className="rounded-full border px-2 py-1 text-xs">session {sessionId ? sessionId.slice(-8) : "n/a"}</span>
            <span className="rounded-full border px-2 py-1 text-xs">tokens ~{tokenEstimate}</span>
            <Button size="sm" variant="outline" onClick={pinCurrentSession} disabled={!sessionId}>Pin Current Session</Button>
          </div>

          <div className="grid gap-2 md:grid-cols-2">
            <div className="space-y-1">
              <p className="text-xs uppercase tracking-widest text-muted-foreground">Pinned Sessions</p>
              {pinnedSessions.length === 0 ? (
                <p className="text-xs text-muted-foreground">No pinned sessions yet.</p>
              ) : pinnedSessions.map((id) => (
                <div key={id} className="flex items-center justify-between rounded border px-2 py-1 text-xs">
                  <span className="font-mono">{id}</span>
                  <Button size="sm" variant="ghost" onClick={() => unpinSession(id)}>Unpin</Button>
                </div>
              ))}
            </div>
            <div className="space-y-1">
              <p className="text-xs uppercase tracking-widest text-muted-foreground">Recent Sessions</p>
              {recentSessions.length === 0 ? (
                <p className="text-xs text-muted-foreground">No recent sessions yet.</p>
              ) : recentSessions.map((id) => (
                <div key={id} className="rounded border px-2 py-1 text-xs font-mono">{id}</div>
              ))}
            </div>
          </div>

          <div className="grid gap-2 md:grid-cols-[1fr_180px]">
            <Textarea
              rows={2}
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
              placeholder="Search messages"
            />
            <select
              className="rounded-md border bg-background px-2 py-2 text-sm"
              value={roleFilter}
              onChange={(e) => setRoleFilter(e.target.value as "all" | "user" | "assistant")}
            >
              <option value="all">All roles</option>
              <option value="user">User only</option>
              <option value="assistant">Assistant only</option>
            </select>
          </div>

          <div className="flex flex-wrap gap-2">
            {QUICK_COMMANDS.map((cmd) => (
              <Button key={cmd} size="sm" variant="outline" onClick={() => setInput(cmd)}>{cmd}</Button>
            ))}
          </div>

          {failedTurns.length > 0 && (
            <div className="space-y-2">
              <p className="text-xs uppercase tracking-widest text-muted-foreground">Failed Turns</p>
              {failedTurns.map((turn, idx) => (
                <div key={`${turn.prompt}-${idx}`} className="flex items-start justify-between gap-2 rounded-md border p-2 text-xs">
                  <div>
                    <p className="font-medium">{turn.prompt}</p>
                    <p className="text-destructive">{turn.error}</p>
                  </div>
                  <Button size="sm" onClick={() => retryFailedTurn(turn.prompt)}>Retry</Button>
                </div>
              ))}
            </div>
          )}
        </CardContent>
      </Card>

      {error && (
        <Card>
          <CardContent className="py-3 text-sm text-destructive">{error}</CardContent>
        </Card>
      )}

      <Card className="flex-1 overflow-hidden">
        <CardContent className="flex flex-col h-full p-4 gap-3">
          <div className="flex-1 overflow-y-auto space-y-3 pr-1">
            {filteredMessages.map((msg, i) => (
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
