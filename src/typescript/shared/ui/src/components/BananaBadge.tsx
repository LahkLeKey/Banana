// @banana/ui annotation: cross-platform (no DOM-only deps).
import type { ReactNode } from "react";
import type { EnsembleVerdict } from "../types";

type CountVariantProps = {
    variant?: "count";
    count: number;
    children?: ReactNode;
};

type EnsembleVariantProps = {
    variant: "ensemble";
    verdict: EnsembleVerdict;
    /**
     * Slice 015 US3 -- when false (default), the rendered DOM is
     * byte-identical to the same badge with the flag absent. Only set true
     * when the consumer has opted into VITE_BANANA_SHOW_ATTENTION=1.
     */
    showAttention?: boolean;
    children?: ReactNode;
};

export type BananaBadgeProps = CountVariantProps | EnsembleVariantProps;

export function BananaBadge(props: BananaBadgeProps) {
    if (props.variant === "ensemble") {
        return <EnsembleBadge {...props} />;
    }
    const { count, children } = props;
    return (
        <span className="inline-flex items-center gap-1 rounded-md bg-yellow-200 px-2 py-1 text-xs font-medium text-yellow-900">
            🍌 {count}
            {children}
        </span>
    );
}

function EnsembleBadge({ verdict, showAttention, children }: EnsembleVariantProps) {
    const labelText = verdict.label === "banana"
        ? "banana"
        : verdict.label === "not_banana"
            ? "not banana"
            : "unknown";
    const tone = verdict.label === "banana"
        ? "bg-yellow-200 text-yellow-900"
        : verdict.label === "not_banana"
            ? "bg-slate-200 text-slate-900"
            : "bg-rose-100 text-rose-900";
    const magnitude = verdict.calibration_magnitude.toFixed(2);

    return (
        <span
            data-testid="banana-badge-ensemble"
            data-status={verdict.status}
            className={`inline-flex items-center gap-2 rounded-md px-2 py-1 text-xs font-medium ${tone}`}
        >
            <span>🍌 {labelText}</span>
            <span data-testid="banana-badge-ensemble-magnitude" className="opacity-75">
                m={magnitude}
            </span>
            {verdict.did_escalate ? (
                <span
                    data-testid="banana-badge-ensemble-escalated"
                    className="rounded-full bg-amber-100 px-1.5 py-0.5 text-[10px] font-semibold uppercase text-amber-900"
                >
                    escalated
                </span>
            ) : null}
            {showAttention ? (
                <span
                    data-testid="banana-badge-ensemble-attention"
                    className="rounded-full bg-indigo-100 px-1.5 py-0.5 text-[10px] font-semibold uppercase text-indigo-900"
                >
                    {verdict.did_escalate ? "fb" : "rb"}
                </span>
            ) : null}
            {children}
        </span>
    );
}
