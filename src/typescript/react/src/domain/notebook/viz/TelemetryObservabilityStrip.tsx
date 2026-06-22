import type { ReactNode } from 'react';

export function TelemetryObservabilityStrip({
    interactionCount,
    epochCount,
    impact,
    momentum,
    yield_,
    alignment,
    variance,
    flow,
    lastInteraction,
}: {
    readonly interactionCount: number;
    readonly epochCount: number;
    readonly impact: number;
    readonly momentum: number;
    readonly yield_: number;
    readonly alignment: number;
    readonly variance: number;
    readonly flow: number;
    readonly lastInteraction: string | null;
}) {
    return (
        <div
            style={{
                position: 'absolute',
                left: '50%',
                transform: 'translateX(-50%)',
                top: 64,
                width: 'calc(100vw - 12px)',
                maxWidth: 'min(76vw, 980px)',
                borderRadius: 12,
                border: '1px solid rgba(45, 212, 191, 0.24)',
                background: 'linear-gradient(90deg, rgba(2, 10, 20, 0.82), rgba(9, 18, 36, 0.72))',
                backdropFilter: 'blur(5px)',
                padding: '8px 10px',
                display: 'grid',
                gap: 6,
                pointerEvents: 'none',
            }}
        >
            <div style={{ display: 'flex', gap: 7, alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap' }}>
                <span style={{ fontSize: 10, color: '#67e8f9', letterSpacing: '0.07em', textTransform: 'uppercase', fontWeight: 700 }}>
                    Global observability
                </span>
                <span style={{ fontSize: 10, color: '#94a3b8' }}>
                    interactions {interactionCount} • epochs {epochCount}
                </span>
            </div>

            <div style={{ display: 'flex', gap: 6, alignItems: 'center', flexWrap: 'wrap' }}>
                <MetricPillSimple label="impact" value={formatSigned(impact)} tone={impact >= 0 ? 'good' : 'bad'} />
                <MetricPillSimple label="momentum" value={formatSigned(momentum)} tone={momentum >= 0 ? 'good' : 'bad'} />
                <MetricPillSimple label="yield" value={formatSigned(yield_)} tone={yield_ >= 0 ? 'good' : 'bad'} />
                <MetricPillSimple label="alignment" value={`${alignment}%`} />
                <MetricPillSimple label="variance" value={variance.toFixed(3)} />
                <MetricPillSimple label="flow" value={`${Math.round(flow * 100)}%`} />
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: '2.2fr 1fr', gap: 8, alignItems: 'center' }}>
                <div style={{ borderRadius: 999, border: '1px solid rgba(148, 163, 184, 0.22)', background: 'rgba(15, 23, 42, 0.45)', padding: '2px 8px' }}>
                    {/* sparkline will be rendered by parent */}
                </div>
                <div style={{ fontSize: 10, color: '#94a3b8', textAlign: 'right' }}>
                    {lastInteraction ? `last: ${lastInteraction}` : 'last: waiting for interaction'}
                </div>
            </div>
            <div style={{ fontSize: 9, color: '#64748b' }}>
                event ticks mark recent interaction logs (info/success/warn/error)
            </div>
        </div>
    );
}

function formatSigned(value: number): string {
    const sign = value >= 0 ? '+' : '';
    return `${sign}${value.toFixed(3)}`;
}

function MetricPillSimple({
    label,
    value,
    tone = 'default',
}: {
    readonly label: string;
    readonly value: string;
    readonly tone?: 'default' | 'good' | 'bad';
}) {
    const color = tone === 'good' ? '#86efac' : tone === 'bad' ? '#fda4af' : '#cbd5e1';
    const border = tone === 'good'
        ? '1px solid rgba(134, 239, 172, 0.28)'
        : tone === 'bad'
            ? '1px solid rgba(253, 164, 175, 0.28)'
            : '1px solid rgba(148, 163, 184, 0.24)';

    return (
        <span style={{ fontSize: 10, color, borderRadius: 999, border, padding: '2px 7px' }}>
            <span style={{ color: '#64748b' }}>{label}</span> {value}
        </span>
    );
}
