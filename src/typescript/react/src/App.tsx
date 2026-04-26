import { FormEvent, useCallback, useEffect, useMemo, useRef, useState } from "react";
import {
    BananaBadge,
    ChatMessageBubble,
    EscalationPanel,
    RipenessLabel,
    type ChatMessage,
    type ChatSession,
    type EmbeddingSummary,
    type EnsembleVerdict,
} from "@banana/ui";
import {
    createChatSession,
    fetchBananaSummary,
    fetchEnsembleVerdictWithEmbedding,
    predictRipeness,
    type RipenessResponse,
    resolveApiBaseUrl,
    resolvePlatformLabel,
    sendChatMessage,
} from "./lib/api";
import {
    RETRY_BUTTON_COPY,
    VERDICT_EMPTY_COPY,
    errorWording,
    verdictCopy,
} from "./lib/copy";

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

export function App() {
    const [banana, setBanana] = useState<number | null>(null);
    const [session, setSession] = useState<ChatSession | null>(null);
    const [messages, setMessages] = useState<ChatMessage[]>([]);
    const [draft, setDraft] = useState("");
    const [chatError, setChatError] = useState<string | null>(null);
    const [ripenessInput, setRipenessInput] = useState("");
    const [ripenessResult, setRipenessResult] = useState<RipenessResponse | null>(null);
    const [ripenessError, setRipenessError] = useState<string | null>(null);
    const [isPredictingRipeness, setIsPredictingRipeness] = useState(false);
    const [ensembleVerdict, setEnsembleVerdict] = useState<EnsembleVerdict | null>(null);
    const [ensembleEmbedding, setEnsembleEmbedding] = useState<number[] | null>(null);
    const [ensembleError, setEnsembleError] = useState<string | null>(null);
    const [isPredictingEnsemble, setIsPredictingEnsemble] = useState(false);
    const [ensembleInput, setEnsembleInput] = useState("");
    const [lastSubmittedSample, setLastSubmittedSample] = useState<string | null>(null);
    const [isBootstrapping, setIsBootstrapping] = useState(false);
    const [isSending, setIsSending] = useState(false);
    const messageCounter = useRef(0);

    const apiBaseUrl = useMemo(() => resolveApiBaseUrl(), []);
    const platformLabel = useMemo(() => resolvePlatformLabel(), []);
    const chatUnavailable = apiBaseUrl.length === 0;
    const showAttention = useMemo(() => {
        const importMeta = import.meta as { env?: { VITE_BANANA_SHOW_ATTENTION?: string } };
        return importMeta.env?.VITE_BANANA_SHOW_ATTENTION === "1";
    }, []);

    useEffect(() => {
        if (!apiBaseUrl) return;
        fetchBananaSummary(apiBaseUrl)
            .then((payload) => setBanana(payload.banana))
            .catch(() => setBanana(null));
    }, [apiBaseUrl]);

    // Slice 024 -- when running inside Electron, subscribe to verdicts
    // pushed by the main process (tray menu / global shortcut /
    // paste-classify) so the UI mirrors the desktop entry points.
    useEffect(() => {
        const bridge = (typeof window !== "undefined"
            ? (window as unknown as { banana?: { onVerdict?: (h: (payload: unknown) => void) => (() => void) } }).banana
            : undefined);
        if (!bridge?.onVerdict) return;
        const off = bridge.onVerdict((payload: unknown) => {
            const envelope = payload as { verdict?: EnsembleVerdict; embedding?: number[] | null } | null;
            if (envelope && envelope.verdict) {
                setEnsembleVerdict(envelope.verdict);
                setEnsembleEmbedding(envelope.embedding ?? null);
                setEnsembleError(null);
            }
        });
        return () => { try { off?.(); } catch { /* noop */ } };
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
                const payload = await createChatSession(apiBaseUrl, platformLabel);
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
    }, [apiBaseUrl, chatUnavailable, platformLabel]);

    const sendMessage = useCallback(async () => {
        if (!session || !apiBaseUrl) return;

        const content = draft.trim();
        if (!content) return;

        setDraft("");
        setIsSending(true);
        setChatError(null);

        try {
            messageCounter.current += 1;
            const payload = await sendChatMessage(
                apiBaseUrl,
                session.id,
                content,
                `${platformLabel}-${session.id}-${messageCounter.current}`,
            );
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
    }, [apiBaseUrl, draft, platformLabel, session]);

    function onSubmit(event: FormEvent<HTMLFormElement>) {
        event.preventDefault();
        void sendMessage();
    }

    async function onRipenessSubmit(event: FormEvent<HTMLFormElement>) {
        event.preventDefault();
        const value = ripenessInput.trim();
        if (!apiBaseUrl || value.length === 0) {
            return;
        }

        setIsPredictingRipeness(true);
        setRipenessError(null);
        try {
            const result = await predictRipeness(apiBaseUrl, value);
            setRipenessResult(result);
        } catch (error: unknown) {
            setRipenessError(
                error instanceof Error ? error.message : "failed to predict ripeness"
            );
            setRipenessResult(null);
        } finally {
            setIsPredictingRipeness(false);
        }
    }

    async function onEnsembleSubmit(event: FormEvent<HTMLFormElement>) {
        event.preventDefault();
        const value = ensembleInput.trim();
        if (!apiBaseUrl || value.length === 0) {
            return;
        }
        await runEnsembleVerdict(value);
    }

    const runEnsembleVerdict = useCallback(
        async (sample: string) => {
            if (!apiBaseUrl || sample.length === 0) {
                return;
            }
            setIsPredictingEnsemble(true);
            setEnsembleError(null);
            setLastSubmittedSample(sample);
            try {
                const result = await fetchEnsembleVerdictWithEmbedding(
                    apiBaseUrl,
                    sample,
                );
                setEnsembleVerdict(result.verdict);
                setEnsembleEmbedding(result.embedding);
            } catch (error: unknown) {
                setEnsembleError(errorWording(error));
                setEnsembleVerdict(null);
                setEnsembleEmbedding(null);
            } finally {
                setIsPredictingEnsemble(false);
            }
        },
        [apiBaseUrl],
    );

    const loadEscalationSummary = useCallback(
        async (): Promise<EmbeddingSummary> => ({ embedding: ensembleEmbedding }),
        [ensembleEmbedding],
    );

    function onEnsembleInputChange(value: string) {
        setEnsembleInput(value);
        // Typing a new sample invalidates the previous error + retry
        // affordance per baseline (retry preserves last submitted draft;
        // editing breaks that contract).
        if (lastSubmittedSample !== null) {
            setLastSubmittedSample(null);
            setEnsembleError(null);
        }
    }

    function onRetryEnsemble() {
        if (lastSubmittedSample === null) {
            return;
        }
        void runEnsembleVerdict(lastSubmittedSample);
    }

    return (
        <main className="mx-auto max-w-3xl space-y-4 p-6">
            <h1 className="text-2xl font-semibold">Banana v2</h1>
            <p className="text-sm text-gray-600">API base: {apiBaseUrl || "<unset>"}</p>

            <div className="flex items-center gap-2">
                <BananaBadge count={banana ?? 0}>(today)</BananaBadge>
                {ensembleVerdict ? (
                    <BananaBadge
                        variant="ensemble"
                        verdict={ensembleVerdict}
                        showAttention={showAttention}
                    />
                ) : null}
            </div>

            <section className="space-y-3 rounded-xl border border-slate-200 bg-white p-4 shadow-sm">
                <h2 className="text-sm font-semibold text-slate-900">Ensemble verdict</h2>
                <form className="space-y-2" onSubmit={onEnsembleSubmit}>
                    <textarea
                        className="min-h-20 w-full rounded-md border border-slate-300 px-3 py-2 text-sm"
                        disabled={chatUnavailable || isPredictingEnsemble}
                        onChange={(event) => onEnsembleInputChange(event.target.value)}
                        placeholder="Describe a possible banana"
                        value={ensembleInput}
                    />
                    <button
                        className="rounded-md bg-yellow-700 px-4 py-2 text-sm font-medium text-white disabled:cursor-not-allowed disabled:bg-slate-300"
                        disabled={chatUnavailable || isPredictingEnsemble || ensembleInput.trim().length === 0}
                        type="submit"
                    >
                        {isPredictingEnsemble ? "Predicting..." : "Predict ensemble"}
                    </button>
                </form>
                <div data-testid="ensemble-verdict-surface" className="space-y-2 text-sm">
                    {ensembleError ? (
                        <div className="flex flex-wrap items-center gap-2">
                            <p data-testid="ensemble-error" className="text-xs text-red-700">
                                {ensembleError}
                            </p>
                            {lastSubmittedSample !== null ? (
                                <button
                                    type="button"
                                    data-testid="ensemble-retry"
                                    onClick={onRetryEnsemble}
                                    disabled={isPredictingEnsemble}
                                    className="rounded-md border border-amber-700 px-2 py-1 text-xs font-medium text-amber-700 hover:bg-amber-50 disabled:cursor-not-allowed disabled:opacity-50"
                                >
                                    {RETRY_BUTTON_COPY}
                                </button>
                            ) : null}
                        </div>
                    ) : ensembleVerdict ? (
                        <p data-testid="ensemble-verdict-copy" className="font-medium text-slate-900">
                            {verdictCopy(ensembleVerdict)}
                            {ensembleVerdict.did_escalate ? (
                                <EscalationPanel loadSummary={loadEscalationSummary} />
                            ) : null}
                        </p>
                    ) : (
                        <p data-testid="ensemble-verdict-empty" className="text-slate-500">
                            {VERDICT_EMPTY_COPY}
                        </p>
                    )}
                </div>
            </section>

            <section className="space-y-3 rounded-xl border border-slate-200 bg-white p-4 shadow-sm">
                <h2 className="text-sm font-semibold text-slate-900">Ripeness prediction</h2>
                <form className="space-y-2" onSubmit={onRipenessSubmit}>
                    <textarea
                        className="min-h-24 w-full rounded-md border border-slate-300 px-3 py-2 text-sm"
                        disabled={chatUnavailable || isPredictingRipeness}
                        onChange={(event) => setRipenessInput(event.target.value)}
                        placeholder="Describe the banana sample"
                        value={ripenessInput}
                    />
                    <button
                        className="rounded-md bg-amber-700 px-4 py-2 text-sm font-medium text-white disabled:cursor-not-allowed disabled:bg-slate-300"
                        disabled={chatUnavailable || isPredictingRipeness || ripenessInput.trim().length === 0}
                        type="submit"
                    >
                        {isPredictingRipeness ? "Predicting..." : "Predict ripeness"}
                    </button>
                </form>

                {ripenessResult ? (
                    <div className="flex items-center gap-2 text-sm text-slate-700">
                        <span>Result:</span>
                        <RipenessLabel value={ripenessResult.label} />
                        <span>confidence {ripenessResult.confidence.toFixed(4)}</span>
                    </div>
                ) : null}
                {ripenessError ? <p className="text-xs text-red-700">{ripenessError}</p> : null}
            </section>

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
