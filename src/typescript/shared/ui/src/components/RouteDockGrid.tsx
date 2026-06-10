import type { CSSProperties, ReactNode } from 'react';

export type RouteDockCorner = 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right';

export type RouteDockEntry = {
    readonly id: string;
    readonly corner: RouteDockCorner;
    readonly visible: boolean;
    readonly content: ReactNode;
};

type RouteDockGridProps = {
    readonly entries: RouteDockEntry[];
    readonly surface?: 'card' | 'overlay';
};

function RouteCornerDock({ corner, children, surface }: { readonly corner: RouteDockCorner; readonly children: ReactNode; readonly surface: 'card' | 'overlay'; }) {
    const isBottom = corner === 'bottom-left' || corner === 'bottom-right';
    const isRight = corner === 'top-right' || corner === 'bottom-right';

    const base: CSSProperties = {
        position: 'absolute',
        zIndex: 4,
        pointerEvents: 'auto',
        width: surface === 'overlay' ? 'min(390px, calc(42vw - 18px))' : 'min(400px, calc(44vw - 16px))',
        maxHeight: 'calc(46dvh - 60px)',
        display: 'flex',
        flexDirection: 'column',
        justifyContent: isBottom ? 'flex-end' : 'flex-start',
        overflow: 'hidden',
    };

    const hPos: CSSProperties = isRight ? { right: 16 } : { left: 16 };
    const vPos: CSSProperties = isBottom ? { bottom: 76 } : { top: 16 };

    return <div style={{ ...base, ...hPos, ...vPos }}>{children}</div>;
}

function RouteDockSurface({ children, surface }: { readonly children: ReactNode; readonly surface: 'card' | 'overlay'; }) {
    if (surface === 'overlay') {
        return (
            <div style={{
                overflow: 'hidden',
                display: 'flex',
                flexDirection: 'column',
                minHeight: 0,
            }}>
                <div className="banana-dock-scroll" style={{
                    flex: '1 1 auto',
                    minHeight: 0,
                    overflow: 'auto',
                    scrollbarWidth: 'none',
                    msOverflowStyle: 'none',
                }}>
                    {children}
                </div>
            </div>
        );
    }

    return (
        <div style={{
            borderRadius: 18,
            border: '1px solid rgba(45, 212, 191, 0.28)',
            background: 'linear-gradient(170deg, rgba(3, 10, 19, 0.9), rgba(1, 7, 14, 0.94))',
            boxShadow: '0 22px 72px rgba(2, 6, 23, 0.48)',
            overflow: 'hidden',
            display: 'flex',
            flexDirection: 'column',
            minHeight: 0,
        }}>
            <div style={{ flex: '1 1 auto', minHeight: 0, overflow: 'auto' }}>
                {children}
            </div>
        </div>
    );
}

export function RouteDockGrid({ entries, surface = 'card' }: RouteDockGridProps) {
    return (
        <>
            <style>{`.banana-dock-scroll::-webkit-scrollbar{display:none}`}</style>
            {entries.map((entry) => (entry.visible ? (
                <RouteCornerDock key={entry.id} corner={entry.corner} surface={surface}>
                    <RouteDockSurface surface={surface}>{entry.content}</RouteDockSurface>
                </RouteCornerDock>
            ) : null))}
        </>
    );
}
