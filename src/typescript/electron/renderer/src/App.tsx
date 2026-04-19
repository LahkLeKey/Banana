import { useEffect, useMemo, useState } from "react";
import { Activity, ArrowRightLeft, Banana, FlaskConical, Server } from "lucide-react";
import { Badge, Button, Card, CardContent, CardDescription, CardHeader, CardTitle, Input, Label } from "@banana/ui";

type StatusVariant = "outline" | "warning" | "success" | "destructive";

type StatusChip = {
    label: string;
    variant: StatusVariant;
};

type FormState = {
    purchases: number;
    multiplier: number;
    apiUrl: string;
    includeDatabase: boolean;
};

type RuntimeConfig = {
    apiUrl: string;
    nativePath: string | null;
};

const idleChip: StatusChip = { label: "Idle", variant: "outline" };

const nativeIdle = "Run a native scenario to inspect calculation, breakdown, and message output.";
const apiIdle = "Run API scenario to inspect HTTP status and JSON payload.";
const compareIdle = "Run both paths to compare native and API outcomes.";

function pretty(value: unknown): string {
    return JSON.stringify(value, null, 2);
}

function errorPayload(error: unknown): { message: string; status?: number; statusCode?: number; payload?: unknown } {
    if (error instanceof Error) {
        const anyError = error as Error & { status?: number; statusCode?: number; payload?: unknown };
        return {
            message: anyError.message,
            status: anyError.status,
            statusCode: anyError.statusCode,
            payload: anyError.payload
        };
    }

    return { message: "Unknown error" };
}

export function App(): JSX.Element {
    const [form, setForm] = useState<FormState>({
        purchases: 10,
        multiplier: 2,
        apiUrl: "http://127.0.0.1:8080",
        includeDatabase: false
    });

    const [runtime, setRuntime] = useState<RuntimeConfig>({
        apiUrl: "http://127.0.0.1:8080",
        nativePath: null
    });

    const [nativeChip, setNativeChip] = useState<StatusChip>(idleChip);
    const [apiChip, setApiChip] = useState<StatusChip>(idleChip);
    const [compareChip, setCompareChip] = useState<StatusChip>({ label: "Waiting", variant: "outline" });

    const [nativeOutput, setNativeOutput] = useState<string>(nativeIdle);
    const [apiOutput, setApiOutput] = useState<string>(apiIdle);
    const [compareOutput, setCompareOutput] = useState<string>(compareIdle);
    const [bootstrapError, setBootstrapError] = useState<string | null>(null);

    useEffect(() => {
        let active = true;

        if (!window.bananaQA) {
            setBootstrapError("Electron preload bridge is unavailable. Ensure preload.js exposes window.bananaQA.");
            return;
        }

        window.bananaQA
            .getConfig()
            .then((config) => {
                if (!active) {
                    return;
                }

                setRuntime(config);
                setForm((current) => ({
                    ...current,
                    apiUrl: config.apiUrl || current.apiUrl
                }));
            })
            .catch((error) => {
                if (!active) {
                    return;
                }

                const details = errorPayload(error);
                setBootstrapError(details.message);
            });

        return () => {
            active = false;
        };
    }, []);

    const request = useMemo(
        () => ({
            purchases: form.purchases,
            multiplier: form.multiplier,
            apiUrl: form.apiUrl,
            includeDatabase: form.includeDatabase
        }),
        [form]
    );

    async function runHealth(): Promise<void> {
        setApiChip({ label: "Checking", variant: "warning" });

        try {
            const result = await window.bananaQA.checkHealth({ apiUrl: form.apiUrl });
            setApiChip({ label: "Healthy", variant: "success" });
            setApiOutput(pretty(result));
        } catch (error) {
            setApiChip({ label: "Unavailable", variant: "destructive" });
            setApiOutput(pretty(errorPayload(error)));
        }
    }

    async function runNative(): Promise<void> {
        setNativeChip({ label: "Running", variant: "warning" });
        setNativeOutput("Running native scenario...");

        try {
            const result = await window.bananaQA.runNative(request);
            setNativeChip({ label: "Ready", variant: "success" });
            setNativeOutput(pretty(result));
        } catch (error) {
            setNativeChip({ label: "Failed", variant: "destructive" });
            setNativeOutput(pretty(errorPayload(error)));
        }
    }

    async function runApi(): Promise<void> {
        setApiChip({ label: "Calling", variant: "warning" });
        setApiOutput("Calling API...");

        try {
            const result = await window.bananaQA.runApi(request);
            setApiChip({ label: "Ready", variant: "success" });
            setApiOutput(pretty(result));
        } catch (error) {
            setApiChip({ label: "Failed", variant: "destructive" });
            setApiOutput(pretty(errorPayload(error)));
        }
    }

    async function runCompare(): Promise<void> {
        setCompareChip({ label: "Comparing", variant: "warning" });
        setCompareOutput("Comparing native and API paths...");

        try {
            const result = (await window.bananaQA.runCompare(request)) as { matches?: boolean };
            const matches = Boolean(result.matches);
            setCompareChip({ label: matches ? "Match" : "Mismatch", variant: matches ? "success" : "destructive" });
            setCompareOutput(pretty(result));
        } catch (error) {
            setCompareChip({ label: "Failed", variant: "destructive" });
            setCompareOutput(pretty(errorPayload(error)));
        }
    }

    return (
        <div className="noise-overlay min-h-screen">
            <main className="mx-auto flex w-full max-w-7xl flex-col gap-6 px-4 py-8 sm:px-8">
                <header className="space-y-3">
                    <Badge variant="outline" className="w-fit border-amber-500/50 bg-amber-100/60 text-amber-800">
                        Banana Desktop
                    </Badge>
                    <h1 className="font-[Fraunces] text-3xl leading-tight text-foreground sm:text-5xl">
                        Bun + React + shadcn + Tailwind for Banana Desktop
                    </h1>
                    <p className="max-w-3xl text-sm text-muted-foreground sm:text-base">
                        This Electron renderer now runs on a Bun-managed React stack while preserving the existing preload bridge for native, API,
                        and compare workflows.
                    </p>
                    <div className="flex flex-wrap items-center gap-2 text-xs">
                        <Badge variant="secondary" className="gap-1">
                            <Server className="h-3.5 w-3.5" />
                            API {runtime.apiUrl}
                        </Badge>
                        <Badge variant="outline" className="gap-1">
                            <FlaskConical className="h-3.5 w-3.5" />
                            Native path {runtime.nativePath ?? "not set"}
                        </Badge>
                        {bootstrapError && (
                            <Badge variant="destructive" className="gap-1">
                                <Activity className="h-3.5 w-3.5" />
                                {bootstrapError}
                            </Badge>
                        )}
                    </div>
                </header>

                <Card>
                    <CardHeader>
                        <CardTitle>Scenario Controls</CardTitle>
                        <CardDescription>Set request inputs and run native, API, or side-by-side comparison paths.</CardDescription>
                    </CardHeader>
                    <CardContent className="space-y-5">
                        <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-4">
                            <div className="space-y-2">
                                <Label htmlFor="purchases">Purchases</Label>
                                <Input
                                    id="purchases"
                                    min={0}
                                    type="number"
                                    value={form.purchases}
                                    onChange={(event) => setForm((current) => ({ ...current, purchases: Number(event.target.value) }))}
                                />
                            </div>
                            <div className="space-y-2">
                                <Label htmlFor="multiplier">Multiplier</Label>
                                <Input
                                    id="multiplier"
                                    min={0}
                                    type="number"
                                    value={form.multiplier}
                                    onChange={(event) => setForm((current) => ({ ...current, multiplier: Number(event.target.value) }))}
                                />
                            </div>
                            <div className="space-y-2 lg:col-span-2">
                                <Label htmlFor="api-url">API URL</Label>
                                <Input
                                    id="api-url"
                                    type="url"
                                    value={form.apiUrl}
                                    onChange={(event) => setForm((current) => ({ ...current, apiUrl: event.target.value }))}
                                />
                            </div>
                        </div>

                        <label className="flex items-center gap-2 text-sm text-muted-foreground">
                            <input
                                checked={form.includeDatabase}
                                className="h-4 w-4 rounded border-input"
                                type="checkbox"
                                onChange={(event) => setForm((current) => ({ ...current, includeDatabase: event.target.checked }))}
                            />
                            Include native DB query in scenario run
                        </label>

                        <div className="flex flex-wrap gap-2">
                            <Button className="gap-2" onClick={() => void runNative()}>
                                <Banana className="h-4 w-4" />
                                Run Native
                            </Button>
                            <Button variant="secondary" className="gap-2" onClick={() => void runApi()}>
                                <Server className="h-4 w-4" />
                                Run API
                            </Button>
                            <Button variant="outline" className="gap-2" onClick={() => void runCompare()}>
                                <ArrowRightLeft className="h-4 w-4" />
                                Run Both
                            </Button>
                            <Button variant="ghost" className="gap-2" onClick={() => void runHealth()}>
                                <Activity className="h-4 w-4" />
                                Check API
                            </Button>
                        </div>
                    </CardContent>
                </Card>

                <section className="grid gap-5 lg:grid-cols-3">
                    <Card className="min-h-[20rem]">
                        <CardHeader className="flex-row items-center justify-between space-y-0">
                            <CardTitle className="text-xl">Native</CardTitle>
                            <Badge variant={nativeChip.variant}>{nativeChip.label}</Badge>
                        </CardHeader>
                        <CardContent>
                            <pre className="max-h-[22rem] overflow-auto rounded-xl bg-amber-50 p-4 text-xs text-amber-950">{nativeOutput}</pre>
                        </CardContent>
                    </Card>

                    <Card className="min-h-[20rem]">
                        <CardHeader className="flex-row items-center justify-between space-y-0">
                            <CardTitle className="text-xl">API</CardTitle>
                            <Badge variant={apiChip.variant}>{apiChip.label}</Badge>
                        </CardHeader>
                        <CardContent>
                            <pre className="max-h-[22rem] overflow-auto rounded-xl bg-lime-50 p-4 text-xs text-lime-950">{apiOutput}</pre>
                        </CardContent>
                    </Card>

                    <Card className="min-h-[20rem]">
                        <CardHeader className="flex-row items-center justify-between space-y-0">
                            <CardTitle className="text-xl">Comparison</CardTitle>
                            <Badge variant={compareChip.variant}>{compareChip.label}</Badge>
                        </CardHeader>
                        <CardContent>
                            <pre className="max-h-[22rem] overflow-auto rounded-xl bg-orange-50 p-4 text-xs text-orange-950">{compareOutput}</pre>
                        </CardContent>
                    </Card>
                </section>
            </main>
        </div>
    );
}
