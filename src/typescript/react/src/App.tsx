import { FormEvent, useEffect, useMemo, useState } from "react";

import { createBatch, fetchBanana, fetchBatchStatus, fetchHealth, predictRipeness } from "./lib/bananaApi";
import type {
    BananaBatchCreateRequest,
    BananaBatchResponse,
    BananaRequest,
    BananaResponse,
    BananaRipenessRequest,
    BananaRipenessResponse,
    HealthResponse
} from "./types/api";

type RequestState = "idle" | "loading" | "ready" | "error";

type PanelState<T> = {
    status: RequestState;
    data: T | null;
    error: string | null;
};

function App(): JSX.Element {
    const [form, setForm] = useState<BananaRequest>({ purchases: 10, multiplier: 5 });
    const [batchForm, setBatchForm] = useState<BananaBatchCreateRequest>({
        batchId: "batch-1",
        originFarm: "farm-1",
        storageTempC: 13.2,
        ethyleneExposure: 2.5,
        estimatedShelfLifeDays: 3
    });
    const [ripenessForm, setRipenessForm] = useState<BananaRipenessRequest>({
        batchId: "batch-1",
        temperatureHistory: [12.5, 13, 14.2],
        daysSinceHarvest: 5,
        ethyleneExposure: 2.5,
        mechanicalDamage: 0.1,
        storageTempC: 13.2
    });
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
    const [batch, setBatch] = useState<PanelState<BananaBatchResponse>>({
        status: "idle",
        data: null,
        error: null
    });
    const [ripeness, setRipeness] = useState<PanelState<BananaRipenessResponse>>({
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

    const batchSummary = useMemo(() => {
        if (batch.status !== "ready" || !batch.data) {
            return "Create or load a batch to see supply-chain status.";
        }

        return `${batch.data.batchId} from ${batch.data.originFarm} is ${batch.data.exportStatus}`;
    }, [batch]);

    const ripenessSummary = useMemo(() => {
        if (ripeness.status !== "ready" || !ripeness.data) {
            return "Predict ripeness after a batch exists in the native registry.";
        }

        return `${ripeness.data.batchId} => ${ripeness.data.predictedStage} with ${ripeness.data.shelfLifeHours}h shelf life`;
    }, [ripeness]);

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

    async function handleBatchCreate(event: FormEvent<HTMLFormElement>): Promise<void> {
        event.preventDefault();

        setBatch({ status: "loading", data: null, error: null });

        try {
            const payload = await createBatch(batchForm);
            setBatch({ status: "ready", data: payload, error: null });
            setRipenessForm((current) => ({
                ...current,
                batchId: payload.batchId,
                ethyleneExposure: payload.ethyleneExposure,
                storageTempC: payload.storageTempC
            }));
        } catch (error) {
            setBatch({
                status: "error",
                data: null,
                error: error instanceof Error ? error.message : "Unknown error"
            });
        }
    }

    async function handleBatchLookup(): Promise<void> {
        setBatch({ status: "loading", data: null, error: null });

        try {
            const payload = await fetchBatchStatus(batchForm.batchId);
            setBatch({ status: "ready", data: payload, error: null });
        } catch (error) {
            setBatch({
                status: "error",
                data: null,
                error: error instanceof Error ? error.message : "Unknown error"
            });
        }
    }

    async function handleRipenessPredict(event: FormEvent<HTMLFormElement>): Promise<void> {
        event.preventDefault();

        setRipeness({ status: "loading", data: null, error: null });

        try {
            const payload = await predictRipeness(ripenessForm);
            setRipeness({ status: "ready", data: payload, error: null });
        } catch (error) {
            setRipeness({
                status: "error",
                data: null,
                error: error instanceof Error ? error.message : "Unknown error"
            });
        }
    }

    return (
        <main className="mx-auto flex w-full max-w-6xl flex-col gap-8 px-5 py-10 sm:px-8 lg:py-14">
            <header className="space-y-3">
                <p className="inline-flex rounded-full border border-sky-300/40 bg-sky-400/10 px-3 py-1 text-xs font-semibold uppercase tracking-[0.16em] text-sky-200">
                    Banana Runtime Console
                </p>
                <h1 className="font-display text-4xl leading-tight text-white sm:text-5xl">React + Bun control surface for Banana API</h1>
                <p className="max-w-2xl text-sm text-slate-300 sm:text-base">
                    This control surface now spans basic banana calculation, native-backed batch tracking, and ripeness prediction for the Banana Management System.
                </p>
            </header>

            <section className="grid gap-6 xl:grid-cols-[1.15fr_0.85fr]">
                <div className="grid gap-6">
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

                    <article className="rounded-3xl border border-slate-700/60 bg-slate-900/70 p-5 shadow-panel backdrop-blur-sm sm:p-7">
                        <h2 className="font-display text-2xl text-emerald-300">Track Batch</h2>
                        <p className="mt-1 text-sm text-slate-300">Create or inspect a native-backed batch in the BMS registry.</p>

                        <form className="mt-6 grid gap-4 md:grid-cols-2" onSubmit={handleBatchCreate}>
                            <label className="block md:col-span-1">
                                <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Batch ID</span>
                                <input
                                    className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-emerald-300"
                                    name="batchId"
                                    onChange={(event) => {
                                        const value = event.target.value;
                                        setBatchForm((current) => ({ ...current, batchId: value }));
                                        setRipenessForm((current) => ({ ...current, batchId: value }));
                                    }}
                                    type="text"
                                    value={batchForm.batchId}
                                />
                            </label>

                            <label className="block md:col-span-1">
                                <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Origin Farm</span>
                                <input
                                    className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-emerald-300"
                                    name="originFarm"
                                    onChange={(event) => setBatchForm((current) => ({ ...current, originFarm: event.target.value }))}
                                    type="text"
                                    value={batchForm.originFarm}
                                />
                            </label>

                            <label className="block">
                                <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Storage Temp C</span>
                                <input
                                    className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-emerald-300"
                                    min={0}
                                    onChange={(event) => setBatchForm((current) => ({ ...current, storageTempC: Number(event.target.value) }))}
                                    type="number"
                                    value={batchForm.storageTempC}
                                />
                            </label>

                            <label className="block">
                                <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Ethylene Exposure</span>
                                <input
                                    className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-emerald-300"
                                    min={0}
                                    onChange={(event) => setBatchForm((current) => ({ ...current, ethyleneExposure: Number(event.target.value) }))}
                                    type="number"
                                    value={batchForm.ethyleneExposure}
                                />
                            </label>

                            <label className="block md:col-span-2">
                                <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Estimated Shelf Life Days</span>
                                <input
                                    className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-emerald-300"
                                    min={0}
                                    onChange={(event) => setBatchForm((current) => ({ ...current, estimatedShelfLifeDays: Number(event.target.value) }))}
                                    type="number"
                                    value={batchForm.estimatedShelfLifeDays}
                                />
                            </label>

                            <button
                                className="rounded-xl bg-gradient-to-r from-emerald-300 to-lime-300 px-4 py-3 font-semibold text-slate-950 transition hover:brightness-105 disabled:cursor-not-allowed disabled:opacity-60"
                                disabled={batch.status === "loading"}
                                type="submit"
                            >
                                {batch.status === "loading" ? "Saving..." : "Create Batch"}
                            </button>

                            <button
                                className="rounded-xl border border-emerald-300/40 bg-emerald-300/10 px-4 py-3 font-semibold text-emerald-100 transition hover:bg-emerald-300/20"
                                onClick={() => {
                                    void handleBatchLookup();
                                }}
                                type="button"
                            >
                                Fetch Batch Status
                            </button>
                        </form>
                    </article>

                    <article className="rounded-3xl border border-slate-700/60 bg-slate-900/70 p-5 shadow-panel backdrop-blur-sm sm:p-7">
                        <h2 className="font-display text-2xl text-rose-200">Predict Ripeness</h2>
                        <p className="mt-1 text-sm text-slate-300">Run batch-aware ripeness prediction against the native BMS registry.</p>

                        <form className="mt-6 grid gap-4 md:grid-cols-2" onSubmit={handleRipenessPredict}>
                            <label className="block md:col-span-2">
                                <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Batch ID</span>
                                <input
                                    className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-rose-200"
                                    onChange={(event) => setRipenessForm((current) => ({ ...current, batchId: event.target.value }))}
                                    type="text"
                                    value={ripenessForm.batchId}
                                />
                            </label>

                            <label className="block md:col-span-2">
                                <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Temperature History</span>
                                <input
                                    className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-rose-200"
                                    onChange={(event) => setRipenessForm((current) => ({
                                        ...current,
                                        temperatureHistory: event.target.value
                                            .split(",")
                                            .map((value) => Number(value.trim()))
                                            .filter((value) => !Number.isNaN(value))
                                    }))}
                                    type="text"
                                    value={ripenessForm.temperatureHistory.join(", ")}
                                />
                            </label>

                            <label className="block">
                                <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Days Since Harvest</span>
                                <input
                                    className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-rose-200"
                                    min={0}
                                    onChange={(event) => setRipenessForm((current) => ({ ...current, daysSinceHarvest: Number(event.target.value) }))}
                                    type="number"
                                    value={ripenessForm.daysSinceHarvest}
                                />
                            </label>

                            <label className="block">
                                <span className="mb-2 block text-xs font-semibold uppercase tracking-wide text-slate-300">Mechanical Damage</span>
                                <input
                                    className="w-full rounded-xl border border-slate-600 bg-slate-950/70 px-4 py-3 text-white outline-none transition focus:border-rose-200"
                                    min={0}
                                    step="0.01"
                                    onChange={(event) => setRipenessForm((current) => ({ ...current, mechanicalDamage: Number(event.target.value) }))}
                                    type="number"
                                    value={ripenessForm.mechanicalDamage}
                                />
                            </label>

                            <button
                                className="md:col-span-2 rounded-xl bg-gradient-to-r from-rose-200 to-orange-300 px-4 py-3 font-semibold text-slate-950 transition hover:brightness-105 disabled:cursor-not-allowed disabled:opacity-60"
                                disabled={ripeness.status === "loading"}
                                type="submit"
                            >
                                {ripeness.status === "loading" ? "Predicting..." : "Predict Ripeness"}
                            </button>
                        </form>
                    </article>
                </div>

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
                        <p className="text-xs uppercase tracking-wide text-slate-400">Batch Summary</p>
                        <p className="mt-2 text-sm text-slate-200">{batchSummary}</p>
                        {batch.status === "error" && <p className="mt-2 text-sm text-rose-300">{batch.error}</p>}
                    </div>

                    <div className="rounded-2xl border border-slate-700 bg-slate-950/70 p-4">
                        <p className="text-xs uppercase tracking-wide text-slate-400">Ripeness Summary</p>
                        <p className="mt-2 text-sm text-slate-200">{ripenessSummary}</p>
                        {ripeness.status === "error" && <p className="mt-2 text-sm text-rose-300">{ripeness.error}</p>}
                    </div>

                    <div className="rounded-2xl border border-slate-700 bg-slate-950/70 p-4">
                        <p className="text-xs uppercase tracking-wide text-slate-400">Banana Payload</p>
                        {banana.status === "ready" && banana.data ? (
                            <pre className="mt-2 overflow-x-auto text-xs text-slate-100">{JSON.stringify(banana.data, null, 2)}</pre>
                        ) : (
                            <p className="mt-2 text-sm text-slate-400">No result yet.</p>
                        )}
                    </div>

                    <div className="rounded-2xl border border-slate-700 bg-slate-950/70 p-4">
                        <p className="text-xs uppercase tracking-wide text-slate-400">Batch Payload</p>
                        {batch.status === "ready" && batch.data ? (
                            <pre className="mt-2 overflow-x-auto text-xs text-slate-100">{JSON.stringify(batch.data, null, 2)}</pre>
                        ) : (
                            <p className="mt-2 text-sm text-slate-400">No batch payload yet.</p>
                        )}
                    </div>

                    <div className="rounded-2xl border border-slate-700 bg-slate-950/70 p-4">
                        <p className="text-xs uppercase tracking-wide text-slate-400">Ripeness Payload</p>
                        {ripeness.status === "ready" && ripeness.data ? (
                            <pre className="mt-2 overflow-x-auto text-xs text-slate-100">{JSON.stringify(ripeness.data, null, 2)}</pre>
                        ) : (
                            <p className="mt-2 text-sm text-slate-400">No ripeness payload yet.</p>
                        )}
                    </div>
                </article>
            </section>
        </main>
    );
}

export default App;
