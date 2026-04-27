// @banana/ui annotation: cross-platform (no DOM-only deps).
// Slice 027 -- token-driven. Public props preserved.
import type { ReactNode } from 'react';
import { tokens } from '../tokens';
import type { EnsembleVerdict } from '../types';

type CountVariantProps = {
    variant?: 'count';
    count: number;
    children?: ReactNode;
};

type EnsembleVariantProps = {
    variant: 'ensemble';
    verdict: EnsembleVerdict;
    showAttention?: boolean;
    children?: ReactNode;
};

export type BananaBadgeProps = CountVariantProps | EnsembleVariantProps;

export function BananaBadge(props: BananaBadgeProps) {
    if (props.variant === 'ensemble') {
        return <EnsembleBadge {...props} />;
    }
    const { count, children } = props;
    return (
        <span
            style={{
                display: 'inline-flex',
                alignItems: 'center',
                gap: tokens.space[1],
                borderRadius: tokens.radius.md,
                backgroundColor: tokens.color.banana.bg,
                color: tokens.color.banana.fg,
                paddingInline: tokens.space[2],
                paddingBlock: tokens.space[1],
                fontSize: tokens.font.size.xs,
                fontWeight: tokens.font.weight.medium,
            }}>
            🍌 {count}
            {children}
        </span>
    );
}

function EnsembleBadge({ verdict, showAttention, children }: EnsembleVariantProps) {
    const labelText = verdict.label === 'banana'
        ? 'banana'
        : verdict.label === 'not_banana'
            ? 'not banana'
            : 'unknown';
    const tone = verdict.label === 'banana'
        ? { bg: tokens.color.banana.bg, fg: tokens.color.banana.fg }
        : verdict.label === 'not_banana'
            ? { bg: tokens.color.notbanana.bg, fg: tokens.color.notbanana.fg }
            : { bg: tokens.color.escalation.bg, fg: tokens.color.text.error };
    const rawMagnitude = typeof verdict.calibration_magnitude === 'number'
        ? verdict.calibration_magnitude
        : Number.NaN;
    const magnitude = Number.isFinite(rawMagnitude) ? rawMagnitude.toFixed(2) : '--';

    return (
        <span
            data-testid="banana-badge-ensemble"
            data-status={verdict.status}
            style={{
                display: 'inline-flex',
                alignItems: 'center',
                gap: tokens.space[2],
                borderRadius: tokens.radius.md,
                backgroundColor: tone.bg,
                color: tone.fg,
                paddingInline: tokens.space[2],
                paddingBlock: tokens.space[1],
                fontSize: tokens.font.size.xs,
                fontWeight: tokens.font.weight.medium,
            }}>
            <span>🍌 {labelText}</span>
            <span data-testid="banana-badge-ensemble-magnitude" style={{ opacity: 0.75 }}>
                m={magnitude}
            </span>
            {verdict.did_escalate ? (
                <span
                    data-testid="banana-badge-ensemble-escalated"
                    style={{
                        backgroundColor: tokens.color.escalation.bg,
                        color: tokens.color.escalation.fg,
                        borderRadius: tokens.radius.pill,
                        paddingInline: tokens.space[1],
                        fontSize: 10,
                        fontWeight: tokens.font.weight.semibold,
                        textTransform: 'uppercase',
                    }}>
                    escalated
                </span>
            ) : null}
            {showAttention ? (
                <span
                    data-testid="banana-badge-ensemble-attention"
                    style={{
                        backgroundColor: tokens.color.surface.muted,
                        color: tokens.color.text.muted,
                        borderRadius: tokens.radius.pill,
                        paddingInline: tokens.space[1],
                        fontSize: 10,
                        fontWeight: tokens.font.weight.semibold,
                        textTransform: 'uppercase',
                    }}>
                    {verdict.did_escalate ? 'fb' : 'rb'}
                </span>
            ) : null}
            {children}
        </span>
    );
}
