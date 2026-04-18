import { FormEvent, useEffect, useMemo, useState } from "react";

import { fetchBanana, fetchHealth } from "./lib/bananaApi";
import type { BananaRequest, BananaResponse, HealthResponse } from "./types/api";

type RequestState = "idle" | "loading" | "ready" | "error";

type PanelState<T> = {
    status: RequestState;
    data: T | null;
    error: string | null;
};

function App(): JSX.Element {
    const [form, setForm] = useState<BananaRequest>({ purchases: 10, multiplier: 5 });
    const [health, setHealth] = useState<PanelState<HealthResponse>>({
        status: "idle",
        data: null,
        error: null
    });
    const [banana, setBanana] = useState<PanelState<BananaResponse>>({
        status: "idle",
        data: null,
        error: null
    });

    useEffect(() => {
        const controller = new AbortController();
        setHealth({ status: "loading", data: null, error: null });

        fetchHealth(controller.signal)
            .then((payload) => {
                if (!payload?.status) {
                    setHealth({ status: "ready", data: null, error: null });
                    return;
                }

                setHealth({ status: "ready", data: payload, error: null });
            })
            .catch((error) => {
                setHealth({ status: "error", data: null, error: error instanceof Error ? error.message : "Unknown error" });
            });

        return () => controller.abort();
    }, []);

    const summary = useMemo(() => {
        if (banana.status !== "ready" || !banana.data) {
            return "Run a banana request to see computed output.";
        }

        return `${banana.data.purchases} x ${banana.data.multiplier} => ${banana.data.banana}`;
    }, [banana]);

    async function handleSubmit(event: FormEvent<HTMLFormElement>): Promise<void> {
        event.preventDefault();

        setBanana({ status: "loading", data: null, error: null });

        try {
            const payload = await fetchBanana(form);
            setBanana({ status: "ready", data: payload, error: null });
        } catch (error) {
            setBanana({
                status: "error",
                data: null,
                error: error instanceof Error ? error.message : "Unknown error"
            });
        }
    }

    return (
        <main className="mx-auto flex w-full max-w-5xl flex-col gap-8 px-5 py-10 sm:px-8 lg:py-14">
            <header className="space-y-3">
                <p className="inline-flex rounded-full border border-sky-300/40 bg-sky-400/10 px-3 py-1 text-xs font-semibold uppercase tracking-[0.16em] text-sky-200">
                    Banana Runtime Console
                </p>
                <h1 className="font-display text-4xl leading-tight text-white sm:text-5xl">React + Bun control surface for Banana API</h1>
                <p className="max-w-2xl text-sm text-slate-300 sm:text-base">
                    This starter app is wired to the backend health and banana endpoints with typed models, explicit error handling, and responsive UI states.
                </p>
            </header>

            <section className="grid gap-6 lg:grid-cols-[1.15fr_0.85fr]">
                <article className="rounded-3xl border border-slate-700/60 bg-slate-900/70 p-5 shadow-panel backdrop-blur-sm sm:p-7">
                    <h2 className="font-display text-2xl text-amber-300">Request Banana</h2>
                    <p className="mt-1 text-sm text-slate-300">Send query params to /banana and inspect the payload.</p>

                    <form className="mt-6 space-y-4" onSubmit={handleSubmit}>
                        <label className="block">
                            <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Purchases</span>
                            <input
                                className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-amber-300"
                                min={0}
                                name="purchases"
                                onChange={(event) => setForm((current) => ({ ...current, purchases: Number(event.target.value) }))}
                                type="number"
                                value={form.purchases}
                            />
                        </label>

                        <label className="block">
                            <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Multiplier</span>
                            <input
                                className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-amber-300"
                                min={0}
                                name="multiplier"
                                onChange={(event) => setForm((current) => ({ ...current, multiplier: Number(event.target.value) }))}
                                type="number"
                                value={form.multiplier}
                            />
                        </label>

                        <button
                            className="w-full rounded-xl bg-gradient-to-r from-amber-300 to-orange-400 px-4 py-3 font-semibold text-slate-950 transition hover:brightness-105 disabled:cursor-not-allowed disabled:opacity-60"
                            disabled={banana.status === "loading"}
                            type="submit"
                        >
                            {banana.status === "loading" ? "Calculating..." : "Calculate Banana"}
                        </button>
                    </form>
                </article>

                <article className="flex flex-col gap-4 rounded-3xl border border-slate-700/60 bg-slate-900/70 p-5 shadow-panel backdrop-blur-sm sm:p-7">
                    <h2 className="font-display text-2xl text-sky-200">Service Status</h2>

                    <div className="rounded-2xl border border-slate-700 bg-slate-950/70 p-4">
                        <p className="text-xs uppercase tracking-wide text-slate-400">Health Endpoint</p>
                        {health.status === "loading" && <p className="mt-2 text-sm text-slate-200">Checking /health ...</p>}
                        {health.status === "error" && <p className="mt-2 text-sm text-rose-300">{health.error}</p>}
                        {health.status === "ready" && !health.data && <p className="mt-2 text-sm text-slate-200">Connected, but no payload returned.</p>}
                        {health.status === "ready" && health.data && (
                            <p className="mt-2 text-sm text-emerald-300">Status: {health.data.status}</p>
                        )}
                        {health.status === "idle" && <p className="mt-2 text-sm text-slate-300">No health check performed yet.</p>}
                    </div>

                    <div className="rounded-2xl border border-slate-700 bg-slate-950/70 p-4">
                        <p className="text-xs uppercase tracking-wide text-slate-400">Banana Summary</p>
                        <p className="mt-2 text-sm text-slate-200">{summary}</p>
                        {banana.status === "error" && <p className="mt-2 text-sm text-rose-300">{banana.error}</p>}
                    </div>

                    <div className="rounded-2xl border border-slate-700 bg-slate-950/70 p-4">
                        <p className="text-xs uppercase tracking-wide text-slate-400">Payload</p>
                        {banana.status === "ready" && banana.data ? (
                            <pre className="mt-2 overflow-x-auto text-xs text-slate-100">{JSON.stringify(banana.data, null, 2)}</pre>
                        ) : (
                            <p className="mt-2 text-sm text-slate-400">No result yet.</p>
                        )}
                    </div>
                </article>
            </section>
        </main>
    );
}

export default App;
