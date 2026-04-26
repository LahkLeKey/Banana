// @banana/ui annotation: cross-platform (no DOM-only deps).
// Slice 023 -- inline collapsible "Why?" panel that surfaces the
// ensemble embedding fingerprint returned by `/ml/ensemble/embedding`
// (slice 017). Renders the canonical escalation cue + an inline
// summary of the 4-dim fingerprint when expanded. Defaults closed.
//
// Copy + cue conventions are pinned in
// .specify/specs/020-frontend-classifier-experience-spike/analysis/copy-and-cue-baseline.md
// -- drift = bug.

import { useState, type ReactNode } from "react";

export type EmbeddingSummary = {
    /**
     * 4-dim fingerprint per slice 017 contract:
     * [banana_context, not_banana_context, attention_delta, banana_probability].
     */
    embedding: number[] | null;
};

export type EscalationPanelProps = {
    /**
     * Async loader for the embedding summary. Called once on first
     * expand. Must return the slice 017 payload shape.
     */
    loadSummary: () => Promise<EmbeddingSummary>;
    /**
     * Optional override for the "Why?" trigger label (default
     * "Why?"). Customers should rarely change this; the baseline copy
     * is the contract.
     */
    triggerLabel?: string;
    /**
     * Optional render override for the loaded summary (defaults to
     * the built-in 4-component table).
     */
    children?: (summary: EmbeddingSummary) => ReactNode;
};

const COMPONENT_LABELS = [
    "banana_context",
    "not_banana_context",
    "attention_delta",
    "banana_probability",
];

export function EscalationPanel({
    loadSummary,
    triggerLabel = "Why?",
    children,
}: EscalationPanelProps) {
    const [open, setOpen] = useState(false);
    const [summary, setSummary] = useState<EmbeddingSummary | null>(null);
    const [error, setError] = useState<string | null>(null);
    const [loading, setLoading] = useState(false);

    async function toggle() {
        if (open) {
            setOpen(false);
            return;
        }
        setOpen(true);
        if (summary || loading) {
            return;
        }
        setLoading(true);
        setError(null);
        try {
            const result = await loadSummary();
            setSummary(result);
        } catch (err: unknown) {
            setError(
                err instanceof Error ? err.message : "Couldn't load the details. Try again.",
            );
        } finally {
            setLoading(false);
        }
    }

    return (
        <span data-testid="escalation-panel" className="inline-block">
            <button
                type="button"
                data-testid="escalation-panel-trigger"
                onClick={() => void toggle()}
                aria-expanded={open}
                className="ml-2 text-xs font-medium text-amber-700 underline underline-offset-2 hover:text-amber-800"
            >
                {triggerLabel}
            </button>
            {open ? (
                <div
                    data-testid="escalation-panel-body"
                    className="mt-2 rounded-md border border-amber-200 bg-amber-50 p-3 text-xs text-amber-900"
                >
                    {loading ? (
                        <span data-testid="escalation-panel-loading">Loading...</span>
                    ) : error ? (
                        <span data-testid="escalation-panel-error" className="text-red-700">
                            {error}
                        </span>
                    ) : summary ? (
                        children ? (
                            children(summary)
                        ) : (
                            <DefaultSummary summary={summary} />
                        )
                    ) : null}
                </div>
            ) : null}
        </span>
    );
}

function DefaultSummary({ summary }: { summary: EmbeddingSummary }) {
    if (!summary.embedding) {
        return (
            <span data-testid="escalation-panel-empty">
                No fingerprint captured for this verdict.
            </span>
        );
    }
    return (
        <table
            data-testid="escalation-panel-fingerprint"
            className="w-full table-fixed text-left"
        >
            <tbody>
                {summary.embedding.map((value, index) => (
                    <tr key={index}>
                        <th className="font-medium text-amber-900">
                            {COMPONENT_LABELS[index] ?? `dim_${index}`}
                        </th>
                        <td className="text-right tabular-nums">{value.toFixed(4)}</td>
                    </tr>
                ))}
            </tbody>
        </table>
    );
}
