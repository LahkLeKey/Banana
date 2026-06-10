import type { CSSProperties, ReactNode } from 'react';

type RouteDeckTransitionProps = {
    readonly reducedMotion: boolean;
    readonly animating: boolean;
    readonly delayMs: number;
    readonly inactiveOpacity: number;
    readonly inactiveTransform: string;
    readonly inactiveFilter: string;
    readonly children: ReactNode;
};

const transitionStyle: CSSProperties = {
    transition: 'opacity 220ms ease, transform 220ms ease, filter 220ms ease',
};

export function RouteDeckTransition({
    reducedMotion,
    animating,
    delayMs,
    inactiveOpacity,
    inactiveTransform,
    inactiveFilter,
    children,
}: RouteDeckTransitionProps) {
    if (reducedMotion) {
        return (
            <div style={{ transition: 'none', opacity: 1, transform: 'none', filter: 'none' }}>
                {children}
            </div>
        );
    }

    return (
        <div style={{
            ...transitionStyle,
            transitionDelay: animating ? `${delayMs}ms` : '0ms',
            opacity: animating ? inactiveOpacity : 1,
            transform: animating ? inactiveTransform : 'translateY(0) scale(1)',
            filter: animating ? inactiveFilter : 'none',
        }}>
            {children}
        </div>
    );
}
