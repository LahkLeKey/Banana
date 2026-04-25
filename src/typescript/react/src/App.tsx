import { FormEvent, useCallback, useEffect, useMemo, useRef, useState } from "react";
import {
    BananaBadge,
    ChatMessageBubble,
    RipenessLabel,
    type ChatMessage,
    type ChatSession,
} from "@banana/ui";

type CreateSessionResponse = {
    session: ChatSession;
    welcome_message: ChatMessage;
};

type SendMessageResponse = {
    session_id: string;
    duplicate: boolean;
    user_message: ChatMessage;
    assistant_message: ChatMessage;
};

type ErrorPayload = {
    error?: {
        message?: string;
    };
};

const ELECTRON_BRIDGE = typeof window !== "undefined" ? window.banana : undefined;
const API_BASE_URL =
    import.meta.env.VITE_BANANA_API_BASE_URL || ELECTRON_BRIDGE?.apiBaseUrl || "";
const PLATFORM = ELECTRON_BRIDGE?.platform
    ? `electron-${ELECTRON_BRIDGE.platform}`
    : "web";

function mergeMessages(existing: ChatMessage[], incoming: ChatMessage[]): ChatMessage[] {
    const merged = [...existing];
    const ids = new Set(existing.map((message) => message.id));
    for (const message of incoming) {
        if (!ids.has(message.id)) {
            merged.push(message);
            ids.add(message.id);
        }
    }
    return merged;
}

async function parseApiError(response: Response): Promise<string> {
    try {
        const payload = (await response.json()) as ErrorPayload;
        return payload.error?.message ?? `request failed (${response.status})`;
    } catch {
        return `request failed (${response.status})`;
    }
}

export function App() {
    const [banana, setBanana] = useState<number | null>(null);
    const [session, setSession] = useState<ChatSession | null>(null);
    const [messages, setMessages] = useState<ChatMessage[]>([]);
    const [draft, setDraft] = useState("");
    const [chatError, setChatError] = useState<string | null>(null);
    const [isBootstrapping, setIsBootstrapping] = useState(false);
    const [isSending, setIsSending] = useState(false);
    const messageCounter = useRef(0);

    const chatUnavailable = API_BASE_URL.length === 0;
    const platformLabel = useMemo(() => PLATFORM, []);

    useEffect(() => {
        if (!API_BASE_URL) return;
        fetch(`${API_BASE_URL}/banana?purchases=3&multiplier=2`)
            .then((r) => r.json())
            .then((d: { banana: number }) => setBanana(d.banana))
            .catch(() => setBanana(null));
    }, []);

    useEffect(() => {
        if (chatUnavailable) {
            setChatError("set VITE_BANANA_API_BASE_URL (or BANANA_API_BASE_URL for Electron preload)");
            return;
        }

        let cancelled = false;

        async function bootstrapSession() {
            setIsBootstrapping(true);
            setChatError(null);
            try {
                const response = await fetch(`${API_BASE_URL}/chat/sessions`, {
                    method: "POST",
                    headers: { "content-type": "application/json" },
                    body: JSON.stringify({
                        platform: platformLabel,
                        metadata: { source: "react-portal" },
                    }),
                });

                if (!response.ok) {
                    throw new Error(await parseApiError(response));
                }

                const payload = (await response.json()) as CreateSessionResponse;
                if (!cancelled) {
                    setSession(payload.session);
                    setMessages([payload.welcome_message]);
                }
            } catch (error: unknown) {
                if (!cancelled) {
                    setChatError(
                        error instanceof Error
                            ? error.message
                            : "failed to create chat session"
                    );
                }
            } finally {
                if (!cancelled) {
                    setIsBootstrapping(false);
                }
            }
        }

        void bootstrapSession();
        return () => {
            cancelled = true;
        };
    }, [chatUnavailable, platformLabel]);

    const sendMessage = useCallback(async () => {
        if (!session || !API_BASE_URL) return;

        const content = draft.trim();
        if (!content) return;

        setDraft("");
        setIsSending(true);
        setChatError(null);

        try {
            messageCounter.current += 1;
            const response = await fetch(
                `${API_BASE_URL}/chat/sessions/${session.id}/messages`,
                {
                    method: "POST",
                    headers: { "content-type": "application/json" },
                    body: JSON.stringify({
                        content,
                        client_message_id: `${platformLabel}-${session.id}-${messageCounter.current}`,
                    }),
                }
            );

            if (!response.ok) {
                throw new Error(await parseApiError(response));
            }

            const payload = (await response.json()) as SendMessageResponse;
            setMessages((existing) =>
                mergeMessages(existing, [payload.user_message, payload.assistant_message])
            );
        } catch (error: unknown) {
            setChatError(
                error instanceof Error ? error.message : "failed to send chat message"
            );
        } finally {
            setIsSending(false);
        }
    }, [draft, platformLabel, session]);

    function onSubmit(event: FormEvent<HTMLFormElement>) {
        event.preventDefault();
        void sendMessage();
    }

    return (
        <main className="mx-auto max-w-3xl space-y-4 p-6">
            <h1 className="text-2xl font-semibold">Banana v2</h1>
            <p className="text-sm text-gray-600">API base: {API_BASE_URL || "<unset>"}</p>

            <div className="flex items-center gap-2">
                <BananaBadge count={banana ?? 0}>(today)</BananaBadge>
                <RipenessLabel value="yellow" />
            </div>

            <section className="space-y-2 rounded-xl border border-slate-200 bg-white p-4 shadow-sm">
                <div className="flex flex-wrap items-center justify-between gap-2 text-xs text-slate-500">
                    <span>platform: {platformLabel}</span>
                    <span>session: {session?.id ?? (isBootstrapping ? "initializing" : "missing")}</span>
                </div>

                <div className="flex max-h-80 flex-col gap-2 overflow-y-auto rounded-lg border border-slate-100 bg-slate-50 p-3">
                    {messages.map((message) => (
                        <ChatMessageBubble key={message.id} message={message} />
                    ))}
                    {messages.length === 0 && !isBootstrapping ? (
                        <p className="text-xs text-slate-500">No chat messages yet.</p>
                    ) : null}
                </div>

                <form className="flex flex-col gap-2 sm:flex-row" onSubmit={onSubmit}>
                    <input
                        className="flex-1 rounded-md border border-slate-300 px-3 py-2 text-sm"
                        disabled={chatUnavailable || isBootstrapping || isSending || !session}
                        onChange={(event) => setDraft(event.target.value)}
                        placeholder="Ask the banana assistant"
                        value={draft}
                    />
                    <button
                        className="rounded-md bg-lime-700 px-4 py-2 text-sm font-medium text-white disabled:cursor-not-allowed disabled:bg-slate-300"
                        disabled={
                            chatUnavailable ||
                            isBootstrapping ||
                            isSending ||
                            !session ||
                            draft.trim().length === 0
                        }
                        type="submit"
                    >
                        {isSending ? "Sending..." : "Send"}
                    </button>
                </form>

                {chatError ? <p className="text-xs text-red-700">{chatError}</p> : null}
            </section>
        </main>
    );
}
