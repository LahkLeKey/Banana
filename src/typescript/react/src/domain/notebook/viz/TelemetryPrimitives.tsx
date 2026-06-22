import type { ReactNode } from 'react';

export function SectionLabel({
    children,
    color = '#94a3b8',
}: {
    readonly children: ReactNode;
    readonly color?: string;
}) {
    return (
        <div
            style={{
                fontSize: 10,
                color,
                textTransform: 'uppercase',
                letterSpacing: '0.06em',
                fontWeight: 700,
            }}
        >
            {children}
        </div>
    );
}

export function InfoLine({
    code,
    text,
}: {
    readonly code: string;
    readonly text: string;
}) {
    return (
        <div style={{ display: 'grid', gridTemplateColumns: '92px 1fr', gap: 6, alignItems: 'center' }}>
            <span style={{ fontSize: 10, color: '#7dd3fc', borderRadius: 999, border: '1px solid rgba(125, 211, 252, 0.26)', padding: '1px 6px', textAlign: 'center' }}>{code}</span>
            <span style={{ fontSize: 10, color: '#94a3b8' }}>{text}</span>
        </div>
    );
}

export function MetricPill({
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

export function MetricBar({
    label,
    value,
    color,
}: {
    readonly label: string;
    readonly value: number;
    readonly color: string;
}) {
    const clamped = Math.min(1, Math.max(0, value));
    return (
        <div style={{ display: 'grid', gap: 2 }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <span style={{ fontSize: 9, color: '#94a3b8' }}>{label}</span>
                <span style={{ fontSize: 9, color: '#cbd5e1' }}>{Math.round(clamped * 100)}%</span>
            </div>
            <div style={{ height: 6, borderRadius: 999, background: 'rgba(30, 41, 59, 0.8)', overflow: 'hidden' }}>
                <div style={{ height: '100%', width: `${Math.round(clamped * 100)}%`, background: color, borderRadius: 999 }} />
            </div>
        </div>
    );
}
