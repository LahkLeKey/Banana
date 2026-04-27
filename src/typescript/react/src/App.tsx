import { FormEvent, KeyboardEvent, MouseEvent, useCallback, useEffect, useMemo, useRef, useState } from "react";
import {
    BananaBadge,
    ChatMessageBubble,
    ErrorText,
    EscalationPanel,
    RetryButton,
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
    resolveApiBaseResolution,
    resolveChatApiBaseUrl,
    resolveChatBootstrapError,
    predictRipeness,
    type RipenessResponse,
    resolvePlatformLabel,
    sendChatMessage,
} from "./lib/api";
import {
    VERDICT_EMPTY_COPY,
    errorWording,
    verdictCopy,
} from "./lib/copy";
import {
    ENSEMBLE_RETRY_POLICY,
    getEnsembleQueue,
    getVerdictHistory,
    type EnsembleQueuePayload,
    type RecentVerdict,
} from "./lib/resilience-bootstrap";
import {
    classifyPotentialBlankPage,
    snapshotLocation,
} from "./lib/blankPageClassification";
import {
    canSubmitEnsemble,
    normalizeEnsembleSample,
    preventNativeSubmitNavigation,
} from "./lib/ensembleSubmitGuardrails";
import { Alert, AlertDescription, AlertTitle } from "./components/ui/alert";
import { Button } from "./components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "./components/ui/card";
import { Input } from "./components/ui/input";
import { Textarea } from "./components/ui/textarea";

// Slice 030 -- narrow Electron bridge surface for history publish +
// drain-success signal. No-op outside Electron (web tab) since the
// bridge is undefined.
type ElectronBananaBridge = {
    history?: { publish?: (list: unknown[]) => void };
    notifyDrainSuccess?: (payload: unknown) => void;
};

function getElectronBridge(): ElectronBananaBridge | undefined {
    if (typeof window === "undefined") return undefined;
    return (window as unknown as { banana?: ElectronBananaBridge }).banana;
}

function publishHistoryToBridge(list: RecentVerdict[]): void {
    const bridge = getElectronBridge();
    try {
        bridge?.history?.publish?.(list);
    } catch {
        /* bridge is best-effort */
    }
}

function notifyDrainSuccessToBridge(verdict: EnsembleVerdict): void {
    const bridge = getElectronBridge();
    try {
        bridge?.notifyDrainSuccess?.({ verdict });
    } catch {
        /* bridge is best-effort */
    }
}

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

type ChatBootstrapState = "initializing" | "ready" | "failed";

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
    const [chatBootstrapState, setChatBootstrapState] = useState<ChatBootstrapState>("initializing");
    const [chatBootstrapRemediation, setChatBootstrapRemediation] = useState<string | null>(null);
    const [bootstrapAttempt, setBootstrapAttempt] = useState(0);
    const [isSending, setIsSending] = useState(false);
    // Slice 029 -- last-N verdict history (rendered as "Recent verdicts").
    const [recentVerdicts, setRecentVerdicts] = useState<RecentVerdict[]>([]);
    const messageCounter = useRef(0);

    const apiBaseResolution = useMemo(() => resolveApiBaseResolution(), []);
    const apiBaseUrl = apiBaseResolution.baseUrl;
    const chatApiBaseUrl = useMemo(() => resolveChatApiBaseUrl(apiBaseUrl), [apiBaseUrl]);
    const apiBaseResolutionError = apiBaseResolution.error;
    const platformLabel = useMemo(() => resolvePlatformLabel(), []);
    const chatUnavailable = Boolean(apiBaseResolutionError);
    const chatReady = chatBootstrapState === "ready" && Boolean(session);
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
            // Config errors are rendered in the dedicated banner.
            setChatError(null);
            setChatBootstrapRemediation(null);
            setChatBootstrapState("failed");
            return;
        }

        let cancelled = false;

        async function bootstrapSession() {
            setIsBootstrapping(true);
            setChatBootstrapState("initializing");
            setChatError(null);
            setChatBootstrapRemediation(null);
            try {
                const payload = await createChatSession(apiBaseUrl, platformLabel, chatApiBaseUrl);
                if (!cancelled) {
                    setSession(payload.session);
                    setMessages([payload.welcome_message]);
                    setChatBootstrapState("ready");
                }
            } catch (error: unknown) {
                if (!cancelled) {
                    const resolution = resolveChatBootstrapError(error);
                    setSession(null);
                    setMessages([]);
                    setChatError(resolution.message);
                    setChatBootstrapRemediation(resolution.remediation);
                    setChatBootstrapState("failed");
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
    }, [apiBaseResolutionError, apiBaseUrl, bootstrapAttempt, chatApiBaseUrl, chatUnavailable, platformLabel]);

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
                chatApiBaseUrl,
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
    }, [apiBaseUrl, chatApiBaseUrl, draft, platformLabel, session]);

    function onSubmit(event: FormEvent<HTMLFormElement>) {
        event.preventDefault();
        void sendMessage();
    }

    function onRetryBootstrap() {
        setBootstrapAttempt((value) => value + 1);
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

    const recordVerdict = useCallback(
        async (sample: string, verdict: EnsembleVerdict) => {
            const history = getVerdictHistory();
            const entry: RecentVerdict = {
                id: `v_${Date.now()}_${Math.random().toString(36).slice(2, 8)}`,
                capturedAt: Date.now(),
                input: sample,
                verdict,
                didEscalate: Boolean(verdict.did_escalate),
            };
            try {
                await history.record(entry);
                const list = await history.list(10);
                setRecentVerdicts(list);
                // Slice 030 -- publish the snapshot to the Electron main
                // process (no-op outside Electron) so the tray menu can
                // surface the latest verdict.
                publishHistoryToBridge(list);
            } catch {
                // History is best-effort; never block the UI on it.
            }
        },
        [],
    );

    const runEnsembleVerdict = useCallback(
        async (sample: string) => {
            if (!apiBaseUrl || sample.length === 0) {
                return;
            }
            const beforeSubmitLocation = snapshotLocation();
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
                void recordVerdict(sample, result.verdict);
            } catch (error: unknown) {
                setEnsembleError(errorWording(error));
                setEnsembleVerdict(null);
                setEnsembleEmbedding(null);
                // Slice 029 -- when offline, queue the sample so a later
                // online drain can resolve it; preserve the draft.
                const isOffline = typeof navigator !== "undefined" && navigator.onLine === false;
                if (isOffline) {
                    try {
                        await getEnsembleQueue().enqueue({
                            key: `ensemble:${sample}`,
                            payload: { sample, submittedAt: Date.now() } satisfies EnsembleQueuePayload,
                            retry: ENSEMBLE_RETRY_POLICY,
                            enqueuedAt: Date.now(),
                        });
                    } catch {
                        // Queue is best-effort; existing inline error already surfaces.
                    }
                }
            } finally {
                const submitOutcome = classifyPotentialBlankPage(
                    beforeSubmitLocation,
                    snapshotLocation(),
                );
                if (submitOutcome === "navigation") {
                    setEnsembleError(
                        "Unexpected navigation was detected while predicting. Please retry and report this regression.",
                    );
                }
                setIsPredictingEnsemble(false);
            }
        },
        [apiBaseUrl, recordVerdict],
    );

    const submitEnsemble = useCallback(
        async (rawInput: string) => {
            const sample = normalizeEnsembleSample(rawInput);
            if (!canSubmitEnsemble(apiBaseUrl, sample, isPredictingEnsemble)) {
                return;
            }
            await runEnsembleVerdict(sample);
        },
        [apiBaseUrl, isPredictingEnsemble, runEnsembleVerdict],
    );

    async function onEnsembleSubmit(event: FormEvent<HTMLFormElement>) {
        preventNativeSubmitNavigation(event);
        await submitEnsemble(ensembleInput);
    }

    async function onEnsembleClick(event: MouseEvent<HTMLButtonElement>) {
        preventNativeSubmitNavigation(event);
        await submitEnsemble(ensembleInput);
    }

    async function onEnsembleInputKeyDown(event: KeyboardEvent<HTMLTextAreaElement>) {
        if (event.key === "Enter" && (event.ctrlKey || event.metaKey)) {
            preventNativeSubmitNavigation(event);
            await submitEnsemble(ensembleInput);
        }
    }

    // Slice 029 -- on mount, hydrate the recent-verdicts surface from
    // persistent storage (e.g., a prior browser-tab session).
    useEffect(() => {
        let cancelled = false;
        getVerdictHistory()
            .list(10)
            .then((list) => {
                if (!cancelled) {
                    setRecentVerdicts(list);
                    publishHistoryToBridge(list);
                }
            })
            .catch(() => {
                /* best-effort */
            });
        return () => {
            cancelled = true;
        };
    }, []);

    // Slice 029 -- drain queued submissions when the browser comes back
    // online; render whichever verdict resolves last.
    useEffect(() => {
        if (typeof window === "undefined" || !apiBaseUrl) return;
        const onOnline = () => {
            void getEnsembleQueue()
                .drain(async (payload) => {
                    const result = await fetchEnsembleVerdictWithEmbedding(
                        apiBaseUrl,
                        payload.sample,
                    );
                    setEnsembleVerdict(result.verdict);
                    setEnsembleEmbedding(result.embedding);
                    setEnsembleError(null);
                    void recordVerdict(payload.sample, result.verdict);
                    // Slice 030 -- signal Electron main so it raises a
                    // drain-success native notification.
                    notifyDrainSuccessToBridge(result.verdict);
                })
                .catch(() => {
                    /* drain errors leave the job queued for next online tick */
                });
        };
        window.addEventListener("online", onOnline);
        return () => window.removeEventListener("online", onOnline);
    }, [apiBaseUrl, recordVerdict]);

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
        void submitEnsemble(lastSubmittedSample);
    }

    return (
        <main className="mx-auto max-w-3xl space-y-4 p-6">
            <h1 className="text-2xl font-semibold">Banana v2</h1>
            <p className="text-sm text-muted-foreground">API base: {apiBaseUrl || "<unset>"}</p>
            {apiBaseResolutionError ? (
                <Alert variant="destructive" data-testid="api-config-error">
                    <AlertTitle>Frontend runtime configuration error</AlertTitle>
                    <AlertDescription>
                        <p>{apiBaseResolutionError}</p>
                        <p className="mt-1 text-xs">
                            Remediation: run <code>Compose: frontend react down</code>, then <code>Compose: frontend react up + ready</code>.
                        </p>
                    </AlertDescription>
                </Alert>
            ) : null}

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

            <Card>
                <CardHeader>
                    <CardTitle className="text-sm">Ensemble verdict</CardTitle>
                </CardHeader>
                <CardContent className="space-y-3">
                    <form className="space-y-2" onSubmit={onEnsembleSubmit} data-testid="ensemble-form">
                        <Textarea
                            className="min-h-20"
                            disabled={chatUnavailable || isPredictingEnsemble}
                            onChange={(event) => onEnsembleInputChange(event.target.value)}
                            onKeyDown={(event) => {
                                void onEnsembleInputKeyDown(event);
                            }}
                            placeholder="Describe a possible banana"
                            value={ensembleInput}
                        />
                        <Button
                            disabled={chatUnavailable || isPredictingEnsemble || ensembleInput.trim().length === 0}
                            onClick={(event) => {
                                void onEnsembleClick(event);
                            }}
                            type="button"
                        >
                            {isPredictingEnsemble ? "Predicting..." : "Predict ensemble"}
                        </Button>
                    </form>
                    <div data-testid="ensemble-verdict-surface" className="space-y-2 text-sm">
                        {ensembleError ? (
                            <div className="flex flex-wrap items-center gap-2">
                                <ErrorText data-testid="ensemble-error">
                                    {ensembleError}
                                </ErrorText>
                                {lastSubmittedSample !== null ? (
                                    <RetryButton
                                        data-testid="ensemble-retry"
                                        onClick={onRetryEnsemble}
                                        disabled={isPredictingEnsemble}
                                    />
                                ) : null}
                            </div>
                        ) : ensembleVerdict ? (
                            <p data-testid="ensemble-verdict-copy" className="font-medium text-foreground">
                                {verdictCopy(ensembleVerdict)}
                                {ensembleVerdict.did_escalate ? (
                                    <EscalationPanel loadSummary={loadEscalationSummary} />
                                ) : null}
                            </p>
                        ) : (
                            <p data-testid="ensemble-verdict-empty" className="text-muted-foreground">
                                {VERDICT_EMPTY_COPY}
                            </p>
                        )}
                    </div>
                    {recentVerdicts.length > 0 ? (
                        <div data-testid="recent-verdicts" className="space-y-1 border-t border-border pt-2 text-xs">
                            <p className="font-semibold text-foreground">Recent verdicts</p>
                            <ul className="space-y-1 text-muted-foreground">
                                {recentVerdicts.map((entry) => (
                                    <li key={entry.id} data-testid="recent-verdict-item">
                                        <span className="font-medium text-foreground">{verdictCopy(entry.verdict as EnsembleVerdict)}</span>
                                        <span className="ml-2 text-muted-foreground">{entry.input}</span>
                                    </li>
                                ))}
                            </ul>
                        </div>
                    ) : null}
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-sm">Ripeness prediction</CardTitle>
                </CardHeader>
                <CardContent className="space-y-3">
                    <form className="space-y-2" onSubmit={onRipenessSubmit}>
                        <Textarea
                            className="min-h-24"
                            disabled={chatUnavailable || isPredictingRipeness}
                            onChange={(event) => setRipenessInput(event.target.value)}
                            placeholder="Describe the banana sample"
                            value={ripenessInput}
                        />
                        <Button
                            disabled={chatUnavailable || isPredictingRipeness || ripenessInput.trim().length === 0}
                            type="submit"
                            variant="secondary"
                        >
                            {isPredictingRipeness ? "Predicting..." : "Predict ripeness"}
                        </Button>
                    </form>

                    {ripenessResult ? (
                        <div className="flex items-center gap-2 text-sm text-muted-foreground">
                            <span>Result:</span>
                            <RipenessLabel value={ripenessResult.label} />
                            <span>confidence {typeof ripenessResult.confidence === "number" && Number.isFinite(ripenessResult.confidence) ? ripenessResult.confidence.toFixed(4) : "--"}</span>
                        </div>
                    ) : null}
                    {ripenessError ? <ErrorText>{ripenessError}</ErrorText> : null}
                </CardContent>
            </Card>

            <Card>
                <CardContent className="space-y-2 pt-6">
                    <div className="flex flex-wrap items-center justify-between gap-2 text-xs text-muted-foreground">
                        <span>platform: {platformLabel}</span>
                        <span>session: {session?.id ?? chatBootstrapState}</span>
                    </div>

                    <div className="flex max-h-80 flex-col gap-2 overflow-y-auto rounded-md border border-border bg-muted/30 p-3">
                        {messages.map((message) => (
                            <ChatMessageBubble key={message.id} message={message} />
                        ))}
                        {messages.length === 0 && !isBootstrapping ? (
                            <p className="text-xs text-muted-foreground">No chat messages yet.</p>
                        ) : null}
                    </div>

                    <form className="flex flex-col gap-2 sm:flex-row" onSubmit={onSubmit}>
                        <Input
                            className="flex-1"
                            disabled={!chatReady || chatUnavailable || isBootstrapping || isSending}
                            onChange={(event) => setDraft(event.target.value)}
                            placeholder="Ask the banana assistant"
                            value={draft}
                        />
                        <Button
                            disabled={
                                !chatReady ||
                                chatUnavailable ||
                                isBootstrapping ||
                                isSending ||
                                draft.trim().length === 0
                            }
                            type="submit"
                            variant="secondary"
                        >
                            {isSending ? "Sending..." : "Send"}
                        </Button>
                    </form>

                    {chatError ? <ErrorText>{chatError}</ErrorText> : null}
                    {chatBootstrapRemediation ? (
                        <p className="text-xs text-muted-foreground" data-testid="chat-bootstrap-remediation">
                            {chatBootstrapRemediation}
                        </p>
                    ) : null}
                    {!chatUnavailable && !chatReady && !isBootstrapping ? (
                        <RetryButton
                            data-testid="chat-bootstrap-retry"
                            onClick={onRetryBootstrap}
                            disabled={isBootstrapping}
                        />
                    ) : null}
                </CardContent>
            </Card>
        </main>
    );
}
