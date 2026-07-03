import type { CSSProperties, ReactNode } from 'react';

type RouteShellProps = {
    readonly children: ReactNode;
    readonly background: string;
};

export function RouteShell({ children, background }: RouteShellProps) {
    return (
        <main style={{
            minHeight: '100dvh',
            background,
            color: '#f8fafc',
            display: 'grid',
            placeItems: 'center',
            padding: '36px 20px',
        }}>
            {children}
        </main>
    );
}

type RoutePanelProps = {
    readonly children: ReactNode;
    readonly width?: string;
};

export function RoutePanel({ children, width = 'min(100%, 1020px)' }: RoutePanelProps) {
    return (
        <section style={{
            width,
            border: '1px solid rgba(148, 163, 184, 0.2)',
            borderRadius: 24,
            padding: 32,
            background: 'rgba(8, 15, 31, 0.72)',
            boxShadow: '0 24px 90px rgba(2, 6, 23, 0.5)',
        }}>
            {children}
        </section>
    );
}

type RouteEyebrowProps = {
    readonly children: ReactNode;
    readonly color?: string;
};

export function RouteEyebrow({ children, color = '#a7f3d0' }: RouteEyebrowProps) {
    return (
        <p style={{
            margin: 0,
            textTransform: 'uppercase',
            letterSpacing: '0.14em',
            color,
            fontWeight: 700,
            fontSize: 12,
        }}>
            {children}
        </p>
    );
}

type RouteTitleProps = {
    readonly children: ReactNode;
};

export function RouteTitle({ children }: RouteTitleProps) {
    return (
        <h1 style={{ margin: '14px 0 10px', fontSize: 'clamp(2.2rem, 5vw, 4rem)', lineHeight: 0.95 }}>
            {children}
        </h1>
    );
}

type RouteBodyProps = {
    readonly children: ReactNode;
    readonly maxWidth?: number;
};

export function RouteBody({ children, maxWidth = 760 }: RouteBodyProps) {
    return (
        <p style={{ margin: 0, color: '#cbd5e1', lineHeight: 1.7, maxWidth }}>
            {children}
        </p>
    );
}

type RouteActionsRowProps = {
    readonly children: ReactNode;
    readonly marginTop?: number;
};

export function RouteActionsRow({ children, marginTop = 24 }: RouteActionsRowProps) {
    return (
        <div style={{ display: 'flex', gap: 14, flexWrap: 'wrap', marginTop }}>
            {children}
        </div>
    );
}

type RouteActionLinkProps = {
    readonly href: string;
    readonly children: ReactNode;
    readonly target?: string;
    readonly rel?: string;
    readonly tone?: 'primary' | 'neutral' | 'emerald';
};

const actionToneStyle: Record<NonNullable<RouteActionLinkProps['tone']>, CSSProperties> = {
    primary: {
        background: 'linear-gradient(135deg, #22c55e, #0f766e)',
        color: '#fff',
    },
    neutral: {
        border: '1px solid rgba(148, 163, 184, 0.3)',
        color: '#e2e8f0',
    },
    emerald: {
        border: '1px solid rgba(16, 185, 129, 0.4)',
        color: '#bbf7d0',
        background: 'rgba(6, 78, 59, 0.35)',
    },
};

export function RouteActionLink({ href, children, target, rel, tone = 'neutral' }: RouteActionLinkProps) {
    return (
        <a
            href={href}
            target={target}
            rel={rel}
            style={{
                display: 'inline-flex',
                alignItems: 'center',
                justifyContent: 'center',
                padding: '14px 18px',
                borderRadius: 14,
                textDecoration: 'none',
                fontWeight: 700,
                ...actionToneStyle[tone],
            }}
        >
            {children}
        </a>
    );
}

type RouteActionButtonProps = {
    readonly children: ReactNode;
    readonly onClick: () => void;
    readonly disabled?: boolean;
    readonly tone?: 'primary' | 'neutral' | 'danger';
};

const buttonToneStyle: Record<NonNullable<RouteActionButtonProps['tone']>, CSSProperties> = {
    primary: {
        border: '1px solid rgba(20, 184, 166, 0.7)',
        background: 'linear-gradient(135deg, #0f766e, #0e7490)',
        color: '#f8fafc',
    },
    neutral: {
        border: '1px solid rgba(56, 189, 248, 0.65)',
        background: 'rgba(8, 47, 73, 0.6)',
        color: '#e0f2fe',
    },
    danger: {
        border: '1px solid rgba(248, 113, 113, 0.5)',
        background: 'rgba(69, 10, 10, 0.55)',
        color: '#fecaca',
    },
};

export function RouteActionButton({ children, onClick, disabled = false, tone = 'neutral' }: RouteActionButtonProps) {
    return (
        <button
            type="button"
            onClick={onClick}
            disabled={disabled}
            style={{
                borderRadius: 12,
                fontWeight: 700,
                padding: '12px 18px',
                cursor: disabled ? 'not-allowed' : 'pointer',
                opacity: disabled ? 0.8 : 1,
                ...buttonToneStyle[tone],
            }}
        >
            {children}
        </button>
    );
}

type RouteFieldLabelProps = {
    readonly children: ReactNode;
};

export function RouteFieldLabel({ children }: RouteFieldLabelProps) {
    return (
        <label style={{ display: 'block', marginTop: 18, color: '#cbd5e1', fontWeight: 700 }}>
            {children}
        </label>
    );
}

type RouteTextInputProps = {
    readonly value: string;
    readonly onChange: (value: string) => void;
    readonly maxLength?: number;
    readonly width?: string;
    readonly placeholder?: string;
};

export function RouteTextInput({ value, onChange, maxLength, width = 'min(420px, 100%)', placeholder }: RouteTextInputProps) {
    return (
        <input
            type="text"
            value={value}
            onChange={(event) => onChange(event.target.value)}
            maxLength={maxLength}
            placeholder={placeholder}
            style={{
                marginTop: 8,
                width,
                borderRadius: 10,
                border: '1px solid rgba(45, 212, 191, 0.35)',
                background: 'rgba(6, 15, 26, 0.9)',
                color: '#e2e8f0',
                padding: '11px 12px',
            }}
        />
    );
}

type RouteMetaTextProps = {
    readonly children: ReactNode;
};

export function RouteMetaText({ children }: RouteMetaTextProps) {
    return (
        <p style={{ margin: '10px 0 0', color: '#93c5fd' }}>{children}</p>
    );
}

type RouteInfoGridProps = {
    readonly children: ReactNode;
};

export function RouteInfoGrid({ children }: RouteInfoGridProps) {
    return (
        <div style={{
            marginTop: 22,
            display: 'grid',
            gap: 14,
            gridTemplateColumns: 'repeat(auto-fit, minmax(230px, 1fr))',
        }}>
            {children}
        </div>
    );
}

type RouteInfoCardProps = {
    readonly title: string;
    readonly children: ReactNode;
};

export function RouteInfoCard({ title, children }: RouteInfoCardProps) {
    return (
        <article style={{
            borderRadius: 16,
            border: '1px solid rgba(148, 163, 184, 0.2)',
            background: 'rgba(15, 23, 42, 0.52)',
            padding: 16,
        }}>
            <h2 style={{ margin: '0 0 10px', fontSize: 18 }}>{title}</h2>
            {children}
        </article>
    );
}
