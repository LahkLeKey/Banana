import { useEffect, useMemo, useRef, useState, type ReactNode } from 'react';

type PanelId = 'explorer' | 'menu' | 'operations' | 'status' | 'visualizations' | 'training' | 'bulkMonitor' | 'intelNode' | 'objectiveNode' | 'playerNode' | 'questLog' | 'nodeOps';

type RouteHudControlStripProps = {
    readonly routeLabel: string;
    readonly showExplorer?: boolean;
    readonly explorerDropupOpen?: boolean;
    readonly showMenu: boolean;
    readonly showOperations: boolean;
    readonly showStatus: boolean;
    readonly showVisualizations?: boolean;
    readonly showTraining?: boolean;
    readonly showBulkMonitor?: boolean;
    readonly showTrainingControl?: boolean;
    readonly showBulkMonitorControl?: boolean;
    readonly showIntelNode?: boolean;
    readonly showObjectiveNode?: boolean;
    readonly showPlayerNode?: boolean;
    readonly showQuestLog?: boolean;
    readonly showNodeOps?: boolean;
    readonly onToggleExplorer?: () => void;
    readonly onToggleMenu: () => void;
    readonly onToggleOperations: () => void;
    readonly onToggleStatus: () => void;
    readonly onToggleVisualizations?: () => void;
    readonly onToggleTraining?: () => void;
    readonly onToggleBulkMonitor?: () => void;
    readonly onToggleIntelNode?: () => void;
    readonly onToggleObjectiveNode?: () => void;
    readonly onTogglePlayerNode?: () => void;
    readonly onToggleQuestLog?: () => void;
    readonly onToggleNodeOps?: () => void;
    readonly pinnedPanels?: readonly PanelId[];
    readonly onTogglePanelPin?: (panel: PanelId) => void;
    readonly onResetPanel?: (panel: PanelId) => void;
    readonly onFocusViewport: () => void;
    readonly onResetHud: () => void;
    readonly onCloseAllPanels?: () => void;
    readonly onReopenPanels?: () => void;
};

type HudButtonProps = {
    readonly label: string;
    readonly icon?: ReactNode;
    readonly onClick: () => void;
    readonly active?: boolean;
    readonly muted?: boolean;
    readonly compact?: boolean;
};

function HudButton({ label, icon, onClick, active = false, muted = false, compact = false }: HudButtonProps) {
    const hasIcon = icon !== undefined;
    return (
        <button
            type="button"
            onClick={onClick}
            aria-label={label}
            title={label}
            style={{
                borderRadius: 999,
                border: active ? '1px solid rgba(45, 212, 191, 0.6)' : '1px solid rgba(148, 163, 184, 0.3)',
                background: active ? 'rgba(13, 148, 136, 0.18)' : 'rgba(8, 13, 28, 0.4)',
                color: muted ? '#94a3b8' : '#e2e8f0',
                padding: hasIcon
                    ? (compact ? '4px 6px' : '6px 8px')
                    : (compact ? '4px 9px' : '6px 11px'),
                minWidth: hasIcon ? (compact ? 30 : 34) : undefined,
                cursor: 'pointer',
                fontSize: compact ? 10 : 11,
                fontWeight: 700,
                whiteSpace: 'nowrap',
                display: 'inline-flex',
                alignItems: 'center',
                justifyContent: 'center',
                gap: compact ? 5 : 6,
            }}
        >
            {icon ?? label}
        </button>
    );
}

function PanelIcon({ id }: { readonly id: PanelId }) {
    const svgStyle = { width: 13, height: 13 } as const;

    switch (id) {
        case 'explorer':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M2 4h4l1.2 1.5H14v6.5H2z" fill="none" stroke="currentColor" strokeWidth="1.4" strokeLinejoin="round" />
                </svg>
            );
        case 'menu':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M3 4h10M3 8h10M3 12h10" fill="none" stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" />
                </svg>
            );
        case 'operations':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M9.5 2.5l4 4-7 7H2.5v-4z" fill="none" stroke="currentColor" strokeWidth="1.4" strokeLinejoin="round" />
                </svg>
            );
        case 'status':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M2.5 13.5V8.8l2.2-2.2 2.2 2.2 2.2-3.8 2.2 2.2 2.2-3.2v9.5z" fill="none" stroke="currentColor" strokeWidth="1.3" strokeLinejoin="round" />
                </svg>
            );
        case 'visualizations':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M2.5 12.5V9.5m3 3V6.5m3 6V3.5m3 9V8.5" fill="none" stroke="currentColor" strokeWidth="1.4" strokeLinecap="round" />
                    <path d="M2 13.5h12" fill="none" stroke="currentColor" strokeWidth="1.2" strokeLinecap="round" />
                </svg>
            );
        case 'training':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M3 12.5 6.4 8.8l2.1 1.8L13 4.5" fill="none" stroke="currentColor" strokeWidth="1.3" strokeLinecap="round" strokeLinejoin="round" />
                    <path d="M11.1 4.5H13v1.9" fill="none" stroke="currentColor" strokeWidth="1.3" strokeLinecap="round" strokeLinejoin="round" />
                    <path d="M2.5 13.5h11" fill="none" stroke="currentColor" strokeWidth="1.2" strokeLinecap="round" />
                </svg>
            );
        case 'bulkMonitor':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M2 4h5v3H2zM9 4h5v3H9zM2 9.5h5v3H2zM9 9.5h5v3H9z" fill="none" stroke="currentColor" strokeWidth="1.3" strokeLinejoin="round" />
                    <path d="M4 6.5v.5M4 12v.5" fill="none" stroke="currentColor" strokeWidth="1.2" strokeLinecap="round" />
                </svg>
            );
        case 'intelNode':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <circle cx="8" cy="8" r="5.2" fill="none" stroke="currentColor" strokeWidth="1.4" />
                    <circle cx="8" cy="8" r="1.6" fill="currentColor" />
                </svg>
            );
        case 'objectiveNode':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M8 2.5l5 2.8v5.4L8 13.5l-5-2.8V5.3z" fill="none" stroke="currentColor" strokeWidth="1.3" strokeLinejoin="round" />
                </svg>
            );
        case 'playerNode':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <circle cx="8" cy="5.2" r="2.3" fill="none" stroke="currentColor" strokeWidth="1.4" />
                    <path d="M3.2 13.2c.8-2.1 2.5-3.3 4.8-3.3s4 1.2 4.8 3.3" fill="none" stroke="currentColor" strokeWidth="1.4" strokeLinecap="round" />
                </svg>
            );
        case 'questLog':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M4 2.5h8v11H4zM6 6h4M6 9h4" fill="none" stroke="currentColor" strokeWidth="1.3" strokeLinecap="round" strokeLinejoin="round" />
                </svg>
            );
        case 'nodeOps':
            return (
                <svg viewBox="0 0 16 16" aria-hidden="true" style={svgStyle}>
                    <path d="M8 2.3v2.1M8 11.6v2.1M2.3 8h2.1M11.6 8h2.1M4 4l1.5 1.5M10.5 10.5 12 12M12 4l-1.5 1.5M5.5 10.5 4 12" fill="none" stroke="currentColor" strokeWidth="1.3" strokeLinecap="round" />
                    <circle cx="8" cy="8" r="2.1" fill="none" stroke="currentColor" strokeWidth="1.3" />
                </svg>
            );
    }
}

function Bar({ children, mobile, ultraShort }: { readonly children: ReactNode; readonly mobile: boolean; readonly ultraShort: boolean; }) {
    return (
        <div style={{
            position: 'absolute',
            left: '50%',
            bottom: mobile ? 4 : 8,
            transform: 'translateX(-50%)',
            zIndex: 5,
            pointerEvents: 'auto',
            width: mobile ? 'min(980px, calc(100vw - 10px))' : 'min(1100px, calc(100vw - 20px))',
            borderRadius: mobile ? 10 : 14,
            border: mobile ? '1px solid rgba(45, 212, 191, 0.14)' : '1px solid rgba(45, 212, 191, 0.26)',
            background: mobile
                ? 'linear-gradient(180deg, rgba(2, 10, 20, 0.5), rgba(3, 20, 33, 0.56))'
                : 'linear-gradient(180deg, rgba(2, 10, 20, 0.74), rgba(3, 20, 33, 0.78))',
            boxShadow: mobile ? '0 5px 14px rgba(2, 6, 23, 0.26)' : '0 10px 24px rgba(2, 6, 23, 0.38)',
            backdropFilter: mobile ? 'blur(4px)' : 'blur(6px)',
            padding: mobile ? (ultraShort ? '4px 6px' : '5px 7px') : '8px 12px',
            display: mobile ? 'grid' : 'flex',
            gridTemplateColumns: mobile ? '1fr auto' : undefined,
            justifyContent: 'space-between',
            alignItems: 'center',
            gap: mobile ? 6 : 12,
        }}>
            {children}
        </div>
    );
}

const MOBILE_STRIP_RESUME_MS = 8000;

export function RouteHudControlStrip({
    routeLabel,
    showExplorer = false,
    onToggleExplorer,
    showMenu,
    showOperations,
    showStatus,
    showVisualizations = false,
    showTraining = false,
    showBulkMonitor = false,
    showTrainingControl = true,
    showBulkMonitorControl = true,
    showIntelNode = false,
    showObjectiveNode = false,
    showPlayerNode = false,
    showQuestLog = false,
    showNodeOps = false,
    onToggleMenu,
    onToggleOperations,
    onToggleStatus,
    onToggleVisualizations,
    onToggleTraining,
    onToggleBulkMonitor,
    onToggleIntelNode,
    onToggleObjectiveNode,
    onTogglePlayerNode,
    onToggleQuestLog,
    onToggleNodeOps,
    pinnedPanels = ['explorer', 'menu', 'operations', 'status', 'visualizations', 'training', 'bulkMonitor', 'intelNode', 'objectiveNode', 'playerNode', 'questLog', 'nodeOps'],
    onTogglePanelPin,
    onResetPanel,
    onFocusViewport,
    onResetHud,
    onCloseAllPanels,
    onReopenPanels,
}: RouteHudControlStripProps) {
    const [isDropupOpen, setIsDropupOpen] = useState(false);
    const [viewportWidth, setViewportWidth] = useState<number>(
        typeof window === 'undefined' ? 1280 : window.innerWidth,
    );
    const [mobileTelemetryOverlayOpen, setMobileTelemetryOverlayOpen] = useState(false);
    const [stripDismissed, setStripDismissed] = useState(false);
    const [showWhileOverlayOpen, setShowWhileOverlayOpen] = useState(false);
    const rootRef = useRef<HTMLDivElement | null>(null);
    const stripTouchStartYRef = useRef<number | null>(null);
    const stripTouchStartXRef = useRef<number | null>(null);
    const panelIcons: Array<{
        readonly id: PanelId;
        readonly label: string;
        readonly active: boolean;
        readonly onClick: () => void;
    }> = [
            {
                id: 'explorer',
                label: 'Explorer',
                active: showExplorer,
                onClick: onToggleExplorer ?? (() => { }),
            },
            {
                id: 'menu',
                label: 'Command Menu',
                active: showMenu,
                onClick: onToggleMenu,
            },
            {
                id: 'operations',
                label: 'Operations',
                active: showOperations,
                onClick: onToggleOperations,
            },
            {
                id: 'status',
                label: 'Status',
                active: showStatus,
                onClick: onToggleStatus,
            },
            {
                id: 'visualizations',
                label: 'Visualizations',
                active: showVisualizations,
                onClick: onToggleVisualizations ?? (() => { }),
            },
            ...(showTrainingControl ? [{
                id: 'training' as const,
                label: 'Training',
                active: showTraining,
                onClick: onToggleTraining ?? (() => { }),
            }] : []),
            ...(showBulkMonitorControl ? [{
                id: 'bulkMonitor' as const,
                label: 'Bulk Monitor',
                active: showBulkMonitor,
                onClick: onToggleBulkMonitor ?? (() => { }),
            }] : []),
            {
                id: 'intelNode',
                label: 'Intel Node',
                active: showIntelNode,
                onClick: onToggleIntelNode ?? (() => { }),
            },
            {
                id: 'objectiveNode',
                label: 'Objective Node',
                active: showObjectiveNode,
                onClick: onToggleObjectiveNode ?? (() => { }),
            },
            {
                id: 'playerNode',
                label: 'Player Node',
                active: showPlayerNode,
                onClick: onTogglePlayerNode ?? (() => { }),
            },
            {
                id: 'questLog',
                label: 'Quest Log',
                active: showQuestLog,
                onClick: onToggleQuestLog ?? (() => { }),
            },
            {
                id: 'nodeOps',
                label: 'Node Ops',
                active: showNodeOps,
                onClick: onToggleNodeOps ?? (() => { }),
            },
        ];
    const pinnedSet = useMemo(() => new Set(pinnedPanels), [pinnedPanels]);
    const pinnedIconPanels = panelIcons.filter((panel) => pinnedSet.has(panel.id));
    const activePanelCount = panelIcons.reduce((count, panel) => count + (panel.active ? 1 : 0), 0);
    const mobileViewport = viewportWidth < 900;
    const ultraShortMobileViewport = mobileViewport && viewportWidth < 380;
    const basePinnedLimit = viewportWidth < 430 ? 2 : (ultraShortMobileViewport ? 3 : 4);
    const denseMobileMode = mobileViewport && (isDropupOpen || activePanelCount >= 2 || viewportWidth < 430);
    const collapsedRouteBadge = mobileViewport && (denseMobileMode || ultraShortMobileViewport);
    const visiblePinnedLimit = mobileViewport
        ? Math.max(2, basePinnedLimit - (denseMobileMode ? 1 : 0))
        : pinnedIconPanels.length;
    const visiblePinnedIconPanels = mobileViewport
        ? pinnedIconPanels.slice(0, visiblePinnedLimit)
        : pinnedIconPanels;
    const overlayAutoHidden = mobileViewport && mobileTelemetryOverlayOpen && !showWhileOverlayOpen;
    const stripHidden = mobileViewport && (overlayAutoHidden || stripDismissed);

    useEffect(() => {
        if (typeof window === 'undefined') {
            return;
        }

        const onResize = () => {
            setViewportWidth(window.innerWidth);
        };

        window.addEventListener('resize', onResize);
        return () => {
            window.removeEventListener('resize', onResize);
        };
    }, []);

    useEffect(() => {
        if (!isDropupOpen) {
            return;
        }

        const handlePointerDown = (event: MouseEvent) => {
            if (!rootRef.current) {
                return;
            }
            const target = event.target;
            if (target instanceof Node && !rootRef.current.contains(target)) {
                setIsDropupOpen(false);
            }
        };

        const handleEscape = (event: KeyboardEvent) => {
            if (event.key === 'Escape') {
                setIsDropupOpen(false);
            }
        };

        window.addEventListener('mousedown', handlePointerDown);
        window.addEventListener('keydown', handleEscape);

        return () => {
            window.removeEventListener('mousedown', handlePointerDown);
            window.removeEventListener('keydown', handleEscape);
        };
    }, [isDropupOpen]);

    useEffect(() => {
        if (typeof window === 'undefined') {
            return;
        }

        const onOverlayVisibility = (event: Event) => {
            const customEvent = event as CustomEvent<{ open?: boolean; mobile?: boolean }>;
            const overlayOpen = Boolean(customEvent.detail?.open);
            const overlayMobile = Boolean(customEvent.detail?.mobile);
            setMobileTelemetryOverlayOpen(overlayOpen && overlayMobile);
        };

        window.addEventListener('banana:telemetry-overlay-visibility', onOverlayVisibility as EventListener);
        return () => {
            window.removeEventListener('banana:telemetry-overlay-visibility', onOverlayVisibility as EventListener);
        };
    }, []);

    useEffect(() => {
        if (!mobileTelemetryOverlayOpen) {
            setShowWhileOverlayOpen(false);
        }
    }, [mobileTelemetryOverlayOpen]);

    useEffect(() => {
        if (!(stripDismissed || showWhileOverlayOpen)) {
            return;
        }

        const timer = window.setTimeout(() => {
            setStripDismissed(false);
            setShowWhileOverlayOpen(false);
        }, MOBILE_STRIP_RESUME_MS);

        return () => {
            window.clearTimeout(timer);
        };
    }, [stripDismissed, showWhileOverlayOpen]);

    useEffect(() => {
        if (!overlayAutoHidden) {
            return;
        }

        setIsDropupOpen(false);
    }, [overlayAutoHidden]);

    const handlePanelAction = (action: () => void) => {
        action();
        setIsDropupOpen(false);
    };

    const handleStripTouchStart = (event: React.TouchEvent<HTMLDivElement>) => {
        if (!mobileViewport) {
            return;
        }

        stripTouchStartYRef.current = event.touches[0]?.clientY ?? null;
        stripTouchStartXRef.current = event.touches[0]?.clientX ?? null;
    };

    const handleStripTouchEnd = (event: React.TouchEvent<HTMLDivElement>) => {
        if (!mobileViewport) {
            return;
        }

        const startY = stripTouchStartYRef.current;
        const startX = stripTouchStartXRef.current;
        const endY = event.changedTouches[0]?.clientY;
        const endX = event.changedTouches[0]?.clientX;

        stripTouchStartYRef.current = null;
        stripTouchStartXRef.current = null;

        if (
            typeof startY !== 'number' ||
            typeof startX !== 'number' ||
            typeof endY !== 'number' ||
            typeof endX !== 'number'
        ) {
            return;
        }

        const deltaY = endY - startY;
        const deltaX = Math.abs(endX - startX);
        if (deltaY > 42 && deltaX < 38) {
            setStripDismissed(true);
            setShowWhileOverlayOpen(false);
            setIsDropupOpen(false);
        }
    };

    if (stripHidden) {
        return (
            <button
                type="button"
                aria-label="Show HUD controls"
                title="Show HUD controls"
                onClick={() => {
                    setStripDismissed(false);
                    if (mobileTelemetryOverlayOpen) {
                        setShowWhileOverlayOpen(true);
                    }
                }}
                style={{
                    position: 'absolute',
                    right: 8,
                    bottom: 6,
                    zIndex: 5,
                    width: 30,
                    height: 30,
                    borderRadius: 999,
                    border: '1px solid rgba(125, 211, 252, 0.45)',
                    background: 'rgba(8, 13, 28, 0.68)',
                    color: '#7dd3fc',
                    display: 'inline-flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                    cursor: 'pointer',
                    backdropFilter: 'blur(5px)',
                    boxShadow: '0 6px 14px rgba(2, 6, 23, 0.35)',
                    fontSize: 14,
                    fontWeight: 700,
                }}
            >
                ▴
            </button>
        );
    }

    return (
        <div onTouchStart={handleStripTouchStart} onTouchEnd={handleStripTouchEnd}>
            <Bar mobile={mobileViewport} ultraShort={ultraShortMobileViewport}>
                <div ref={rootRef} style={{ display: 'flex', gap: 6, flexWrap: 'wrap', alignItems: 'center', position: 'relative' }}>
                    <div style={{
                        borderRadius: 999,
                        border: mobileViewport ? '1px solid rgba(148, 163, 184, 0.2)' : '1px solid rgba(148, 163, 184, 0.28)',
                        background: mobileViewport ? 'rgba(8, 13, 28, 0.32)' : 'rgba(8, 13, 28, 0.44)',
                        padding: mobileViewport ? 2 : 3,
                        display: 'inline-flex',
                        gap: mobileViewport ? 3 : 4,
                        alignItems: 'center',
                    }}>
                        {visiblePinnedIconPanels.map((panel) => (
                            <HudButton
                                key={panel.id}
                                label={panel.label}
                                icon={<PanelIcon id={panel.id} />}
                                onClick={panel.onClick}
                                active={panel.active}
                                compact={mobileViewport}
                            />
                        ))}
                        <HudButton
                            label={isDropupOpen ? 'Close Navigation' : 'Open Navigation'}
                            icon="▴"
                            onClick={() => setIsDropupOpen((current) => !current)}
                            active={isDropupOpen}
                            muted={!isDropupOpen}
                            compact={mobileViewport}
                        />
                    </div>
                    {isDropupOpen ? (
                        <div style={{
                            position: 'absolute',
                            left: mobileViewport ? 0 : 12,
                            bottom: mobileViewport ? 44 : 56,
                            width: mobileViewport ? 'min(420px, calc(100vw - 16px))' : 'min(420px, calc(100vw - 36px))',
                            borderRadius: 12,
                            border: mobileViewport ? '1px solid rgba(45, 212, 191, 0.2)' : '1px solid rgba(45, 212, 191, 0.24)',
                            background: 'linear-gradient(180deg, rgba(2, 10, 20, 0.92), rgba(3, 20, 33, 0.95))',
                            boxShadow: mobileViewport ? '0 10px 20px rgba(2, 6, 23, 0.34)' : '0 14px 28px rgba(2, 6, 23, 0.42)',
                            backdropFilter: 'blur(6px)',
                            padding: mobileViewport ? 7 : 8,
                            display: 'grid',
                            gap: 6,
                            zIndex: 6,
                        }}>
                            {panelIcons.map((panel) => {
                                const pinned = pinnedSet.has(panel.id);
                                return (
                                    <div key={panel.id} style={{
                                        display: 'grid',
                                        gridTemplateColumns: 'auto 1fr auto auto auto',
                                        alignItems: 'center',
                                        gap: 8,
                                        borderRadius: 8,
                                        border: panel.active ? '1px solid rgba(45, 212, 191, 0.45)' : '1px solid rgba(148, 163, 184, 0.2)',
                                        background: panel.active ? 'rgba(13, 148, 136, 0.12)' : 'rgba(8, 13, 28, 0.48)',
                                        padding: '6px 8px',
                                    }}>
                                        <span style={{
                                            width: 24,
                                            height: 24,
                                            borderRadius: 999,
                                            display: 'inline-flex',
                                            alignItems: 'center',
                                            justifyContent: 'center',
                                            border: '1px solid rgba(148, 163, 184, 0.32)',
                                            fontSize: 11,
                                            fontWeight: 700,
                                            color: '#e2e8f0',
                                        }}>
                                            <PanelIcon id={panel.id} />
                                        </span>
                                        <span style={{ fontSize: 11, color: '#cbd5e1', fontWeight: 700, letterSpacing: '0.02em' }}>{panel.label}</span>
                                        <button
                                            type="button"
                                            onClick={() => handlePanelAction(panel.onClick)}
                                            style={{
                                                borderRadius: 999,
                                                border: panel.active ? '1px solid rgba(45, 212, 191, 0.6)' : '1px solid rgba(148, 163, 184, 0.32)',
                                                background: panel.active ? 'rgba(13, 148, 136, 0.2)' : 'rgba(15, 23, 42, 0.6)',
                                                color: panel.active ? '#99f6e4' : '#cbd5e1',
                                                fontSize: 10,
                                                fontWeight: 700,
                                                padding: '4px 8px',
                                                cursor: 'pointer',
                                            }}
                                        >
                                            {panel.active ? 'Shown' : 'Show'}
                                        </button>
                                        <button
                                            type="button"
                                            onClick={() => {
                                                onResetPanel?.(panel.id);
                                                setIsDropupOpen(false);
                                            }}
                                            aria-label={`Reset ${panel.label} window`}
                                            title={`Reset ${panel.label} window size and position`}
                                            style={{
                                                borderRadius: 999,
                                                border: '1px solid rgba(56, 189, 248, 0.4)',
                                                background: 'rgba(12, 74, 110, 0.35)',
                                                color: '#bae6fd',
                                                fontSize: 10,
                                                fontWeight: 700,
                                                padding: '4px 8px',
                                                cursor: 'pointer',
                                                minWidth: 46,
                                            }}
                                        >
                                            Reset
                                        </button>
                                        <button
                                            type="button"
                                            onClick={() => {
                                                onTogglePanelPin?.(panel.id);
                                                setIsDropupOpen(false);
                                            }}
                                            aria-label={`${pinned ? 'Unpin' : 'Pin'} ${panel.label}`}
                                            title={`${pinned ? 'Unpin' : 'Pin'} ${panel.label}`}
                                            style={{
                                                borderRadius: 999,
                                                border: pinned ? '1px solid rgba(251, 191, 36, 0.58)' : '1px solid rgba(148, 163, 184, 0.32)',
                                                background: pinned ? 'rgba(120, 53, 15, 0.34)' : 'rgba(15, 23, 42, 0.6)',
                                                color: pinned ? '#fcd34d' : '#94a3b8',
                                                fontSize: 10,
                                                fontWeight: 700,
                                                padding: '4px 8px',
                                                cursor: 'pointer',
                                                minWidth: 42,
                                            }}
                                        >
                                            {pinned ? 'Unpin' : 'Pin'}
                                        </button>
                                    </div>
                                );
                            })}
                            {mobileViewport ? (
                                <div style={{ display: 'flex', gap: 5, flexWrap: 'wrap', paddingTop: 4, borderTop: '1px solid rgba(148, 163, 184, 0.2)' }}>
                                    {onCloseAllPanels ? (
                                        <HudButton
                                            label="Close Windows"
                                            onClick={onCloseAllPanels}
                                            muted
                                            compact
                                        />
                                    ) : null}
                                    {onReopenPanels ? (
                                        <HudButton
                                            label="Reopen Windows"
                                            onClick={onReopenPanels}
                                            muted
                                            compact
                                        />
                                    ) : null}
                                    <HudButton
                                        label="Focus Viewport"
                                        onClick={onFocusViewport}
                                        muted
                                        compact
                                    />
                                    <HudButton
                                        label="Reset HUD"
                                        onClick={onResetHud}
                                        muted
                                        compact
                                    />
                                </div>
                            ) : null}
                        </div>
                    ) : null}
                </div>

                {!collapsedRouteBadge ? (
                    <div style={{
                        borderRadius: 999,
                        border: mobileViewport ? '1px solid rgba(148, 163, 184, 0.22)' : '1px solid rgba(148, 163, 184, 0.3)',
                        background: mobileViewport ? 'rgba(8, 13, 28, 0.34)' : 'rgba(8, 13, 28, 0.5)',
                        color: '#93c5fd',
                        padding: mobileViewport ? '4px 8px' : '7px 12px',
                        fontSize: mobileViewport ? 9 : 11,
                        fontWeight: 700,
                        letterSpacing: '0.04em',
                        textTransform: 'uppercase',
                        whiteSpace: 'nowrap',
                    }}>
                        {routeLabel}
                    </div>
                ) : (
                    <div
                        aria-label={routeLabel}
                        title={routeLabel}
                        style={{
                            width: ultraShortMobileViewport ? 8 : 10,
                            height: ultraShortMobileViewport ? 8 : 10,
                            borderRadius: 999,
                            border: '1px solid rgba(125, 211, 252, 0.45)',
                            background: 'rgba(56, 189, 248, 0.32)',
                            justifySelf: 'end',
                        }}
                    />
                )}

                <div style={{ display: mobileViewport ? 'none' : 'flex', gap: 6, flexWrap: 'wrap' }}>
                    {onCloseAllPanels ? (
                        <HudButton
                            label="Close Windows"
                            onClick={onCloseAllPanels}
                            muted
                        />
                    ) : null}
                    {onReopenPanels ? (
                        <HudButton
                            label="Reopen Windows"
                            onClick={onReopenPanels}
                            muted
                        />
                    ) : null}
                    <HudButton
                        label="Focus Viewport"
                        onClick={onFocusViewport}
                        muted
                    />
                    <HudButton
                        label="Reset HUD"
                        onClick={onResetHud}
                        muted
                    />
                </div>
            </Bar>
        </div>
    );
}
