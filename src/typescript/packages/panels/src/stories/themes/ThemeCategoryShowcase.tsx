import * as React from 'react';
import { ResizableDockGrid, type ResizableDockEntry } from '../../resizable-dock-grid';
import { ResizablePanel } from '../../resizable-panel';
import {
    applyPanelTheme,
    getPanelThemeMeta,
    PanelStage,
    resetPanelTheme,
    type PanelThemeName,
    resolvePanelTheme,
    type PanelThemeCategory,
    type PanelThemeMode,
} from '../../index';
import { ThemeBackdrop } from './ThemeBackdrop';

export type ThemeCategoryStoryArgs = {
    readonly themeName?: PanelThemeName;
    readonly mode: PanelThemeMode;
    readonly accentOverride: string;
    readonly showLegend: boolean;
    readonly showDockedComponents: boolean;
    readonly showCommandWindow: boolean;
    readonly showInsightsWindow: boolean;
    readonly showAuditWindow: boolean;
};

type WindowRect = {
    readonly x: number;
    readonly y: number;
    readonly width: number;
    readonly height: number;
};

function SignalBarsSvg() {
    return (
        <svg viewBox="0 0 140 40" className="h-10 w-full">
            <rect x="6" y="22" width="16" height="12" rx="3" fill="currentColor" opacity="0.35" />
            <rect x="30" y="18" width="16" height="16" rx="3" fill="currentColor" opacity="0.45" />
            <rect x="54" y="14" width="16" height="20" rx="3" fill="currentColor" opacity="0.55" />
            <rect x="78" y="10" width="16" height="24" rx="3" fill="currentColor" opacity="0.75" />
            <rect x="102" y="6" width="16" height="28" rx="3" fill="currentColor" />
        </svg>
    );
}

function OrbitSvg() {
    return (
        <svg viewBox="0 0 160 80" className="h-16 w-full">
            <ellipse cx="80" cy="40" rx="60" ry="20" fill="none" stroke="currentColor" strokeWidth="2" opacity="0.35" />
            <ellipse cx="80" cy="40" rx="44" ry="14" fill="none" stroke="currentColor" strokeWidth="2" opacity="0.55" />
            <circle cx="40" cy="40" r="4" fill="currentColor" />
            <circle cx="104" cy="30" r="5" fill="currentColor" opacity="0.85" />
            <circle cx="122" cy="45" r="3" fill="currentColor" opacity="0.65" />
        </svg>
    );
}

const initialCommand: WindowRect = { x: 16, y: 24, width: 360, height: 260 };
const initialInsights: WindowRect = { x: 392, y: 24, width: 360, height: 260 };
const initialAudit: WindowRect = { x: 768, y: 24, width: 320, height: 260 };
const initialLegend: WindowRect = { x: 16, y: 300, width: 420, height: 220 };

export function ThemeCategoryShowcase({
    category,
    args,
}: {
    readonly category: PanelThemeCategory;
    readonly args: ThemeCategoryStoryArgs;
}) {
    const theme = args.themeName ?? resolvePanelTheme(category, args.mode);
    const meta = getPanelThemeMeta(theme);

    const [commandRect, setCommandRect] = React.useState(initialCommand);
    const [insightsRect, setInsightsRect] = React.useState(initialInsights);
    const [auditRect, setAuditRect] = React.useState(initialAudit);
    const [legendRect, setLegendRect] = React.useState(initialLegend);
    const [showHeatmap, setShowHeatmap] = React.useState(true);
    const [showLatency, setShowLatency] = React.useState(true);

    React.useEffect(() => {
        applyPanelTheme(theme);

        if (args.accentOverride.trim().length > 0) {
            document.documentElement.style.setProperty('--banana-panel-accent', args.accentOverride.trim());
        }

        return () => {
            resetPanelTheme();
        };
    }, [args.accentOverride, theme]);

    const dockEntries = React.useMemo<ResizableDockEntry[]>(
        () => [
            {
                id: `${category}-dock-nav`,
                title: 'Predocked Queue',
                corner: 'top-left',
                visible: true,
                defaultWidth: 340,
                defaultHeight: 190,
                content: (
                    <table className="w-full text-xs text-panel-content/90">
                        <thead>
                            <tr className="text-left text-panel-accent">
                                <th className="pb-1">Lane</th>
                                <th className="pb-1">Items</th>
                                <th className="pb-1">SLA</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr><td>Intake</td><td>12</td><td>4m</td></tr>
                            <tr><td>Review</td><td>5</td><td>9m</td></tr>
                            <tr><td>Publish</td><td>3</td><td>2m</td></tr>
                        </tbody>
                    </table>
                ),
            },
            {
                id: `${category}-dock-props`,
                title: 'Predocked Signals',
                corner: 'top-right',
                visible: true,
                defaultWidth: 350,
                defaultHeight: 200,
                content: (
                    <div className="space-y-2 text-xs text-panel-content/90">
                        <div className="text-panel-accent">Signal Throughput</div>
                        <div className="text-panel-accent"><SignalBarsSvg /></div>
                    </div>
                ),
            },
            {
                id: `${category}-dock-events`,
                title: 'Predocked Timeline',
                corner: 'bottom-left',
                visible: true,
                defaultWidth: 460,
                defaultHeight: 180,
                content: (
                    <div className="space-y-2 text-xs text-panel-content/90">
                        <div>09:21 Theme token sync completed</div>
                        <div>09:23 Dock layout persisted</div>
                        <div>09:25 Overlay interaction snapshot captured</div>
                        <div className="text-panel-accent"><OrbitSvg /></div>
                    </div>
                ),
            },
        ],
        [category],
    );

    return (
        <ThemeBackdrop label={`Themes · ${meta.label}`} immersive>
            <PanelStage style={{ inset: '8px 8px 44px 8px' }} label={`${meta.label} stage`}>
                {args.showCommandWindow ? (
                    <ResizablePanel
                        id={`${category}-command-window`}
                        title="Command Window"
                        hostMode="container"
                        {...commandRect}
                        minWidth={300}
                        minHeight={180}
                        onMove={(_, next) => setCommandRect((prev) => ({ ...prev, ...next }))}
                        onResize={(_, rect) => setCommandRect(rect)}
                        onFocus={() => { }}
                        onAnchorCommit={() => { }}
                    >
                        <div className="space-y-2 text-sm text-panel-content/90">
                            <p className="text-panel-accent">Category: {meta.category}</p>
                            <table className="w-full text-xs">
                                <thead>
                                    <tr className="text-left text-panel-accent">
                                        <th className="pb-1">Task</th>
                                        <th className="pb-1">Owner</th>
                                        <th className="pb-1">ETA</th>
                                    </tr>
                                </thead>
                                <tbody>
                                    <tr><td>Theme pass</td><td>Ari</td><td>3m</td></tr>
                                    <tr><td>Dock align</td><td>Dee</td><td>6m</td></tr>
                                    <tr><td>Visual QA</td><td>Kai</td><td>4m</td></tr>
                                </tbody>
                            </table>
                            <div className="text-panel-accent"><SignalBarsSvg /></div>
                        </div>
                    </ResizablePanel>
                ) : null}

                {args.showInsightsWindow ? (
                    <ResizablePanel
                        id={`${category}-insights-window`}
                        title="Insights Window"
                        hostMode="container"
                        {...insightsRect}
                        minWidth={300}
                        minHeight={180}
                        onMove={(_, next) => setInsightsRect((prev) => ({ ...prev, ...next }))}
                        onResize={(_, rect) => setInsightsRect(rect)}
                        onFocus={() => { }}
                        onAnchorCommit={() => { }}
                    >
                        <div className="space-y-2 text-sm text-panel-content/90">
                            <p>Mode: {meta.mode}</p>
                            <p>Accent: <span className="text-panel-accent">{args.accentOverride || 'preset token'}</span></p>
                            <div className="flex gap-3 text-xs">
                                <label className="inline-flex items-center gap-1">
                                    <input type="checkbox" checked={showHeatmap} onChange={(event) => setShowHeatmap(event.target.checked)} />
                                    Heatmap
                                </label>
                                <label className="inline-flex items-center gap-1">
                                    <input type="checkbox" checked={showLatency} onChange={(event) => setShowLatency(event.target.checked)} />
                                    Latency
                                </label>
                            </div>
                            <div className="rounded border border-panel-border/70 p-2 text-xs">
                                {showHeatmap ? 'Heatmap active' : 'Heatmap muted'} · {showLatency ? 'Latency trace active' : 'Latency trace muted'}
                            </div>
                            <div className="text-panel-accent"><OrbitSvg /></div>
                        </div>
                    </ResizablePanel>
                ) : null}

                {args.showAuditWindow ? (
                    <ResizablePanel
                        id={`${category}-audit-window`}
                        title="Audit Window"
                        hostMode="container"
                        {...auditRect}
                        minWidth={280}
                        minHeight={170}
                        onMove={(_, next) => setAuditRect((prev) => ({ ...prev, ...next }))}
                        onResize={(_, rect) => setAuditRect(rect)}
                        onFocus={() => { }}
                        onAnchorCommit={() => { }}
                    >
                        <div className="space-y-2 text-sm text-panel-content/90">
                            <div>Theme pipeline is active and publishing CSS variables.</div>
                            <table className="w-full text-xs">
                                <tbody>
                                    <tr><td>--surface-rgb</td><td className="text-panel-accent">ok</td></tr>
                                    <tr><td>--border-rgb</td><td className="text-panel-accent">ok</td></tr>
                                    <tr><td>--font-family</td><td className="text-panel-accent">ok</td></tr>
                                </tbody>
                            </table>
                        </div>
                    </ResizablePanel>
                ) : null}

                {args.showLegend ? (
                    <ResizablePanel
                        id={`${category}-legend-window`}
                        title="Feature Legend"
                        hostMode="container"
                        {...legendRect}
                        minWidth={340}
                        minHeight={220}
                        onMove={(_, next) => setLegendRect((prev) => ({ ...prev, ...next }))}
                        onResize={(_, rect) => setLegendRect(rect)}
                        onFocus={() => { }}
                        onAnchorCommit={() => { }}
                    >
                        <div className="grid grid-cols-[1fr_auto] gap-x-3 gap-y-2 text-xs text-panel-content/90">
                            <span>Command Window</span><strong>{String(args.showCommandWindow)}</strong>
                            <span>Insights Window</span><strong>{String(args.showInsightsWindow)}</strong>
                            <span>Audit Window</span><strong>{String(args.showAuditWindow)}</strong>
                            <span>Predocked Components</span><strong>{String(args.showDockedComponents)}</strong>
                            <span>Mode</span><strong>{args.mode}</strong>
                            <span>Theme</span><strong className="text-panel-accent">{meta.label}</strong>
                        </div>
                    </ResizablePanel>
                ) : null}

                {args.showDockedComponents ? <ResizableDockGrid entries={dockEntries} hostMode="container" /> : null}
            </PanelStage>
        </ThemeBackdrop>
    );
}
