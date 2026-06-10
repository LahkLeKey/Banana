import type { CSSProperties, ReactNode } from 'react';

type SurfaceTone = 'teal' | 'amber';

const toneStyles: Record<SurfaceTone, { border: string; background: string; }> = {
    teal: {
        border: '1px solid rgba(45, 212, 191, 0.26)',
        background: 'linear-gradient(170deg, rgba(8, 21, 33, 0.92), rgba(4, 11, 22, 0.96))',
    },
    amber: {
        border: '1px solid rgba(250, 204, 21, 0.28)',
        background: 'linear-gradient(170deg, rgba(28, 18, 4, 0.92), rgba(15, 10, 2, 0.96))',
    },
};

type SurfaceCardProps = {
    title: string;
    description?: string;
    tone?: SurfaceTone;
    children: ReactNode;
    aside?: ReactNode;
};

export function SurfaceCard(
    { title, description, tone = 'teal', children, aside }: SurfaceCardProps) {
    const toneStyle = toneStyles[tone];
    return (
        <article style={{
            borderRadius: 18,
            ...toneStyle,
            padding: 18,
            minWidth: 0,
            fontFamily: '"IBM Plex Sans", "Segoe UI", sans-serif',
            boxShadow: 'inset 0 1px 0 rgba(148, 163, 184, 0.08)',
        }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', gap: 12 }}>
                <div style={{ minWidth: 0 }}>
                    <h2 style={{
                        margin: '0 0 8px',
                        fontSize: 18,
                        letterSpacing: '0.04em',
                        textTransform: 'uppercase',
                    }}>
                        {title}
                    </h2>
                    {description ?
                        <p style={{ margin: 0, color: '#cbd5e1', lineHeight: 1.55 }}>{description}</p> :
                        null}
                </div>
                {aside ? <div>{aside}</div> : null}
            </div>
            <div style={{ marginTop: 12 }}>{children}</div>
        </article>
    );
}

export function StatTile(
    { label, value, accent = '#93c5fd' }: { label: string; value: ReactNode; accent?: string; }) {
    return (
        <article style={{
            borderRadius: 12,
            border: '1px solid rgba(148, 163, 184, 0.25)',
            background: 'rgba(8, 13, 28, 0.7)',
            padding: 14,
        }}>
            <div style={{
                fontSize: 12,
                color: accent,
                textTransform: 'uppercase',
                letterSpacing: '0.08em',
            }}>
                {label}
            </div>
            <div style={{ marginTop: 6, fontSize: 24, fontWeight: 700 }}>{value}</div>
        </article>
    );
}

export function Pill(
    { children, color, borderColor }: { children: ReactNode; color: string; borderColor: string; }) {
    return (
        <span style={{
            fontSize: 12,
            color,
            border: `1px solid ${borderColor}`,
            padding: '4px 8px',
            borderRadius: 999,
            whiteSpace: 'nowrap',
        }}>
            {children}
        </span>
    );
}

export function ActionLink(
    { href, children, emphasis = 'secondary' }:
        { href: string; children: ReactNode; emphasis?: 'primary' | 'secondary'; }) {
    const baseStyle: CSSProperties = {
        display: 'inline-flex',
        alignItems: 'center',
        justifyContent: 'center',
        padding: '12px 14px',
        borderRadius: 10,
        textDecoration: 'none',
        fontWeight: 700,
        fontSize: 13,
    };

    const style = emphasis === 'primary' ? {
        ...baseStyle,
        background: 'linear-gradient(135deg, #0e7490, #0f766e)',
        color: '#f8fafc',
    } : {
        ...baseStyle,
        border: '1px solid rgba(148, 163, 184, 0.38)',
        color: '#e2e8f0',
        background: 'rgba(8, 13, 28, 0.45)',
    };

    return <a href={href} style={style}>{children}</a>;
}
