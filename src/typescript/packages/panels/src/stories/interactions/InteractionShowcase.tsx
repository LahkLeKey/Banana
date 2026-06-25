import * as React from 'react';

import {
    PanelOverlay,
    PanelStage,
    ResizableDockGrid,
    ResizablePanel,
    applyPanelTheme,
    resetPanelTheme,
    type ResizableDockEntry,
} from '../../index';
import { ThemeBackdrop } from '../themes/ThemeBackdrop';

type PanelRect = {
    readonly x: number;
    readonly y: number;
    readonly width: number;
    readonly height: number;
};

type OverlayScenario = 'inspect' | 'handoff';

const dragCanvasRect: PanelRect = { x: 16, y: 20, width: 360, height: 250 };
const dragInspectorRect: PanelRect = { x: 408, y: 20, width: 340, height: 250 };
const dragLogRect: PanelRect = { x: 780, y: 20, width: 320, height: 250 };

const closeNavigatorRect: PanelRect = { x: 20, y: 24, width: 300, height: 220 };
const closeInspectorRect: PanelRect = { x: 344, y: 24, width: 320, height: 220 };
const closeConsoleRect: PanelRect = { x: 688, y: 24, width: 320, height: 220 };
const closeControlRect: PanelRect = { x: 272, y: 280, width: 480, height: 220 };

const overlayLauncherRect: PanelRect = { x: 24, y: 24, width: 380, height: 250 };
const overlayNotesRect: PanelRect = { x: 430, y: 24, width: 380, height: 250 };

const relationshipsEditorRect: PanelRect = { x: 18, y: 20, width: 330, height: 240 };
const relationshipsInspectorRect: PanelRect = { x: 380, y: 20, width: 330, height: 240 };
const relationshipsExternalRect: PanelRect = { x: 742, y: 20, width: 330, height: 240 };

function useStoryTheme() {
    React.useEffect(() => {
        applyPanelTheme('aurora-grid-dark');
        return () => resetPanelTheme();
    }, []);
}

function EventList({ entries }: { readonly entries: readonly string[]; }) {
    return (
        <div className="space-y-1 text-xs text-panel-content/85">
            {entries.map((entry, index) => (
                <div key={`${entry}-${index}`} className="rounded border border-panel-border/60 bg-black/10 px-2 py-1">
                    {entry}
                </div>
            ))}
        </div>
    );
}

function InfoBullets({ items }: { readonly items: readonly string[]; }) {
    return (
        <ul className="list-disc space-y-1 pl-4 text-xs text-panel-content/85">
            {items.map((item) => (
                <li key={item}>{item}</li>
            ))}
        </ul>
    );
}

export function DragSnapShowcase({ isolateThirdPanel = false }: { readonly isolateThirdPanel?: boolean; }) {
    useStoryTheme();

    const [canvasRect, setCanvasRect] = React.useState(dragCanvasRect);
    const [inspectorRect, setInspectorRect] = React.useState(dragInspectorRect);
    const [logRect, setLogRect] = React.useState(dragLogRect);
    const [events, setEvents] = React.useState<string[]>([
        'Drag Canvas and Inspector together to test snap commit behavior.',
        isolateThirdPanel
            ? 'Scope Guard uses a different interaction scope and should not snap to the shared pair.'
            : 'All three panels share one scope in this story.',
    ]);

    const appendEvent = React.useCallback((entry: string) => {
        setEvents((current) => [entry, ...current].slice(0, 6));
    }, []);

    const sharedScope = 'drag-snap-shared';
    const guardScope = isolateThirdPanel ? 'drag-snap-guard' : sharedScope;

    return (
        <ThemeBackdrop label="Interactions · Drag and snap" immersive>
            <PanelStage style={{ inset: '8px 8px 44px 8px' }} label="Drag and snap stage">
                <ResizablePanel
                    id="drag-canvas"
                    title="Canvas"
                    hostMode="container"
                    interactionScope={sharedScope}
                    x={canvasRect.x}
                    y={canvasRect.y}
                    width={canvasRect.width}
                    height={canvasRect.height}
                    minWidth={280}
                    minHeight={180}
                    onMove={(_, next) => setCanvasRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setCanvasRect(rect)}
                    onFocus={() => { }}
                    onAnchorCommit={(targetId, sourceSide) => appendEvent(`Canvas snapped ${sourceSide} to ${targetId}`)}
                >
                    <div className="space-y-3 text-sm text-panel-content/90">
                        <div className="text-panel-accent">Free-form drag target</div>
                        <InfoBullets
                            items={[
                                'Drag this panel near Inspector to commit a snap.',
                                'Resize handles stay active while movement remains container-scoped.',
                                'Snap commits appear in the event log panel.',
                            ]}
                        />
                    </div>
                </ResizablePanel>

                <ResizablePanel
                    id="drag-inspector"
                    title="Inspector"
                    hostMode="container"
                    interactionScope={sharedScope}
                    x={inspectorRect.x}
                    y={inspectorRect.y}
                    width={inspectorRect.width}
                    height={inspectorRect.height}
                    minWidth={280}
                    minHeight={180}
                    onMove={(_, next) => setInspectorRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setInspectorRect(rect)}
                    onFocus={() => { }}
                    onAnchorCommit={(targetId, sourceSide) => appendEvent(`Inspector snapped ${sourceSide} to ${targetId}`)}
                >
                    <div className="space-y-3 text-sm text-panel-content/90">
                        <div className="text-panel-accent">Shared interaction target</div>
                        <table className="w-full text-xs">
                            <tbody>
                                <tr><td>Scope</td><td className="text-panel-accent">{sharedScope}</td></tr>
                                <tr><td>Snap eligible</td><td className="text-panel-accent">yes</td></tr>
                                <tr><td>Host mode</td><td className="text-panel-accent">container</td></tr>
                            </tbody>
                        </table>
                    </div>
                </ResizablePanel>

                <ResizablePanel
                    id="drag-log"
                    title={isolateThirdPanel ? 'Scope guard' : 'Event log'}
                    hostMode="container"
                    interactionScope={guardScope}
                    x={logRect.x}
                    y={logRect.y}
                    width={logRect.width}
                    height={logRect.height}
                    minWidth={280}
                    minHeight={180}
                    onMove={(_, next) => setLogRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setLogRect(rect)}
                    onFocus={() => { }}
                    onAnchorCommit={(targetId, sourceSide) => appendEvent(`Scope guard snapped ${sourceSide} to ${targetId}`)}
                >
                    <div className="space-y-3 text-sm text-panel-content/90">
                        <div className="text-panel-accent">{isolateThirdPanel ? 'Isolated scope behavior' : 'Recent interaction events'}</div>
                        {isolateThirdPanel ? (
                            <InfoBullets
                                items={[
                                    `This panel uses ${guardScope}.`,
                                    'It should move and resize normally.',
                                    'It should not snap to the shared-scope pair.',
                                ]}
                            />
                        ) : null}
                        <EventList entries={events} />
                    </div>
                </ResizablePanel>
            </PanelStage>
        </ThemeBackdrop>
    );
}

export function DialogOverlayShowcase({
    closeOnBackdropClick = true,
    closeOnEscape = true,
}: {
    readonly closeOnBackdropClick?: boolean;
    readonly closeOnEscape?: boolean;
}) {
    useStoryTheme();

    const [launcherRect, setLauncherRect] = React.useState(overlayLauncherRect);
    const [notesRect, setNotesRect] = React.useState(overlayNotesRect);
    const [overlayMode, setOverlayMode] = React.useState<OverlayScenario | null>(null);

    const overlayTitle = overlayMode === 'handoff' ? 'Handoff dialog' : 'Inspector dialog';

    return (
        <ThemeBackdrop label="Interactions · Dialog and overlay" immersive>
            <PanelStage style={{ inset: '8px 8px 44px 8px' }} label="Dialog and overlay stage">
                <ResizablePanel
                    id="dialog-launcher"
                    title="Dialog launcher"
                    hostMode="container"
                    x={launcherRect.x}
                    y={launcherRect.y}
                    width={launcherRect.width}
                    height={launcherRect.height}
                    minWidth={300}
                    minHeight={190}
                    onMove={(_, next) => setLauncherRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setLauncherRect(rect)}
                    onFocus={() => { }}
                >
                    <div className="space-y-3 text-sm text-panel-content/90">
                        <div className="text-panel-accent">Open panel-styled dialogs from inside a workspace panel.</div>
                        <div className="flex flex-wrap gap-2">
                            <button className="rounded border border-panel-border/70 px-3 py-1 text-xs" onClick={() => setOverlayMode('inspect')}>Open inspector dialog</button>
                            <button className="rounded border border-panel-border/70 px-3 py-1 text-xs" onClick={() => setOverlayMode('handoff')}>Open handoff dialog</button>
                        </div>
                        <table className="w-full text-xs">
                            <tbody>
                                <tr><td>Backdrop close</td><td className="text-panel-accent">{String(closeOnBackdropClick)}</td></tr>
                                <tr><td>Escape close</td><td className="text-panel-accent">{String(closeOnEscape)}</td></tr>
                                <tr><td>Overlay state</td><td className="text-panel-accent">{overlayMode ?? 'closed'}</td></tr>
                            </tbody>
                        </table>
                    </div>
                </ResizablePanel>

                <ResizablePanel
                    id="dialog-notes"
                    title="Behavior notes"
                    hostMode="container"
                    x={notesRect.x}
                    y={notesRect.y}
                    width={notesRect.width}
                    height={notesRect.height}
                    minWidth={300}
                    minHeight={190}
                    onMove={(_, next) => setNotesRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setNotesRect(rect)}
                    onFocus={() => { }}
                >
                    <InfoBullets
                        items={[
                            'Backdrop click behavior is configurable per story.',
                            'Escape handling is controlled independently from backdrop dismissal.',
                            'The dialog keeps the same panel chrome as the rest of the system.',
                        ]}
                    />
                </ResizablePanel>

                <PanelOverlay
                    isOpen={overlayMode !== null}
                    title={overlayTitle}
                    onClose={() => setOverlayMode(null)}
                    closeOnBackdropClick={closeOnBackdropClick}
                    closeOnEscape={closeOnEscape}
                    chrome={{ movable: true, resizable: true, dockable: false }}
                >
                    <div className="space-y-3 text-sm text-panel-content/90">
                        <div className="text-panel-accent">{overlayMode === 'handoff' ? 'Interruptive approval flow' : 'Focused inspection flow'}</div>
                        <InfoBullets
                            items={overlayMode === 'handoff'
                                ? [
                                    'Use this for moments that require acknowledgement before returning to the workspace.',
                                    'Try the backdrop and Escape key to verify close rules for this story.',
                                ]
                                : [
                                    'Use this for contextual drill-down without leaving the workspace.',
                                    'The close button always works, independent of backdrop behavior.',
                                ]}
                        />
                    </div>
                </PanelOverlay>
            </PanelStage>
        </ThemeBackdrop>
    );
}

export function ClosablePanelsShowcase({
    startsCollapsed = false,
}: {
    readonly startsCollapsed?: boolean;
}) {
    useStoryTheme();

    const [navigatorRect, setNavigatorRect] = React.useState(closeNavigatorRect);
    const [inspectorRect, setInspectorRect] = React.useState(closeInspectorRect);
    const [consoleRect, setConsoleRect] = React.useState(closeConsoleRect);
    const [controlRect, setControlRect] = React.useState(closeControlRect);
    const [collapsed, setCollapsed] = React.useState({
        navigator: startsCollapsed,
        inspector: false,
        console: startsCollapsed,
    });

    const setPanelCollapsed = (panel: keyof typeof collapsed, next: boolean) => {
        setCollapsed((current) => ({ ...current, [panel]: next }));
    };

    return (
        <ThemeBackdrop label="Interactions · Closable panels" immersive>
            <PanelStage style={{ inset: '8px 8px 44px 8px' }} label="Closable panels stage">
                <ResizablePanel
                    id="closable-navigator"
                    title="Navigator"
                    hostMode="container"
                    x={navigatorRect.x}
                    y={navigatorRect.y}
                    width={navigatorRect.width}
                    height={navigatorRect.height}
                    minWidth={260}
                    minHeight={180}
                    isCollapsed={collapsed.navigator}
                    onMove={(_, next) => setNavigatorRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setNavigatorRect(rect)}
                    onFocus={() => { }}
                    onCollapse={() => setPanelCollapsed('navigator', true)}
                    onExpand={() => setPanelCollapsed('navigator', false)}
                >
                    <InfoBullets items={['Use the header close control to collapse this panel.', 'The collapsed launcher keeps its screen position for restore.']} />
                </ResizablePanel>

                <ResizablePanel
                    id="closable-inspector"
                    title="Inspector"
                    hostMode="container"
                    x={inspectorRect.x}
                    y={inspectorRect.y}
                    width={inspectorRect.width}
                    height={inspectorRect.height}
                    minWidth={260}
                    minHeight={180}
                    isCollapsed={collapsed.inspector}
                    onMove={(_, next) => setInspectorRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setInspectorRect(rect)}
                    onFocus={() => { }}
                    onCollapse={() => setPanelCollapsed('inspector', true)}
                    onExpand={() => setPanelCollapsed('inspector', false)}
                >
                    <InfoBullets items={['Keep one panel open while collapsing others to test mixed workspace states.', 'Restore uses the panel launcher, not an external control system.']} />
                </ResizablePanel>

                <ResizablePanel
                    id="closable-console"
                    title="Console"
                    hostMode="container"
                    x={consoleRect.x}
                    y={consoleRect.y}
                    width={consoleRect.width}
                    height={consoleRect.height}
                    minWidth={260}
                    minHeight={180}
                    isCollapsed={collapsed.console}
                    onMove={(_, next) => setConsoleRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setConsoleRect(rect)}
                    onFocus={() => { }}
                    onCollapse={() => setPanelCollapsed('console', true)}
                    onExpand={() => setPanelCollapsed('console', false)}
                >
                    <InfoBullets items={['Collapse this one too and verify multiple launchers can coexist.', 'Resize before collapsing to confirm the reopened panel preserves the current rect.']} />
                </ResizablePanel>

                <ResizablePanel
                    id="closable-controls"
                    title="Workspace controls"
                    hostMode="container"
                    x={controlRect.x}
                    y={controlRect.y}
                    width={controlRect.width}
                    height={controlRect.height}
                    minWidth={320}
                    minHeight={180}
                    onMove={(_, next) => setControlRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setControlRect(rect)}
                    onFocus={() => { }}
                >
                    <div className="space-y-3 text-sm text-panel-content/90">
                        <div className="text-panel-accent">Optional workspace-level helpers</div>
                        <div className="flex flex-wrap gap-2">
                            <button className="rounded border border-panel-border/70 px-3 py-1 text-xs" onClick={() => setCollapsed({ navigator: false, inspector: false, console: false })}>Restore all</button>
                            <button className="rounded border border-panel-border/70 px-3 py-1 text-xs" onClick={() => setCollapsed({ navigator: true, inspector: false, console: true })}>Reset demo state</button>
                        </div>
                        <table className="w-full text-xs">
                            <tbody>
                                <tr><td>Navigator</td><td className="text-panel-accent">{collapsed.navigator ? 'collapsed' : 'open'}</td></tr>
                                <tr><td>Inspector</td><td className="text-panel-accent">{collapsed.inspector ? 'collapsed' : 'open'}</td></tr>
                                <tr><td>Console</td><td className="text-panel-accent">{collapsed.console ? 'collapsed' : 'open'}</td></tr>
                            </tbody>
                        </table>
                    </div>
                </ResizablePanel>
            </PanelStage>
        </ThemeBackdrop>
    );
}

export function DockingLabelsShowcase() {
    useStoryTheme();

    const [guideRect, setGuideRect] = React.useState<PanelRect>({ x: 18, y: 20, width: 340, height: 260 });
    const [visibleIds, setVisibleIds] = React.useState<Record<string, boolean>>({
        context: true,
        metrics: true,
        alerts: true,
    });

    const restoreAll = () => setVisibleIds({ context: true, metrics: true, alerts: true });

    const dockEntries = React.useMemo<ResizableDockEntry[]>(() => [
        {
            id: 'context',
            title: 'Context',
            corner: 'top-left',
            visible: visibleIds.context,
            defaultWidth: 290,
            defaultHeight: 180,
            content: <InfoBullets items={['Drag this docked panel toward another docked panel to create anchor labels.', 'Use unlink and lock buttons from the header once a group is formed.']} />,
        },
        {
            id: 'metrics',
            title: 'Metrics',
            corner: 'top-right',
            visible: visibleIds.metrics,
            defaultWidth: 290,
            defaultHeight: 180,
            content: <InfoBullets items={['Grouped dock panels display side badges that reflect anchor direction.', 'Resize lock is only meaningful after at least two dock panels are linked.']} />,
        },
        {
            id: 'alerts',
            title: 'Alerts',
            corner: 'bottom-right',
            visible: visibleIds.alerts,
            defaultWidth: 290,
            defaultHeight: 180,
            content: <InfoBullets items={['Close any docked panel from its header to test restore flows.', 'Reopen hidden panels from the guide panel.']} />,
        },
    ], [visibleIds]);

    return (
        <ThemeBackdrop label="Interactions · Docking labels" immersive>
            <PanelStage style={{ inset: '8px 8px 44px 8px' }} label="Docking labels stage">
                <ResizablePanel
                    id="dock-guide"
                    title="Dock guide"
                    hostMode="container"
                    x={guideRect.x}
                    y={guideRect.y}
                    width={guideRect.width}
                    height={guideRect.height}
                    minWidth={280}
                    minHeight={220}
                    onMove={(_, next) => setGuideRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setGuideRect(rect)}
                    onFocus={() => { }}
                >
                    <div className="space-y-3 text-sm text-panel-content/90">
                        <div className="text-panel-accent">Dock-specific behaviors to inspect</div>
                        <InfoBullets
                            items={[
                                'Drag dock panels together to expose left, right, top, or bottom anchor badges.',
                                'Use the chain icon to unlink a group after linking panels.',
                                'Use the lock icon to test grouped resize behavior after linking.',
                            ]}
                        />
                        <div className="flex flex-wrap gap-2">
                            {Object.entries(visibleIds).map(([id, visible]) => (
                                <button
                                    key={id}
                                    className="rounded border border-panel-border/70 px-3 py-1 text-xs"
                                    onClick={() => setVisibleIds((current) => ({ ...current, [id]: true }))}
                                    disabled={visible}
                                >
                                    Restore {id}
                                </button>
                            ))}
                            <button className="rounded border border-panel-border/70 px-3 py-1 text-xs" onClick={restoreAll}>Restore all</button>
                        </div>
                    </div>
                </ResizablePanel>

                <ResizableDockGrid
                    hostMode="container"
                    entries={dockEntries}
                    initialAnchorLinks={[
                        { sourceId: 'context', targetId: 'metrics', sourceSide: 'right' },
                        { sourceId: 'metrics', targetId: 'alerts', sourceSide: 'bottom' },
                    ]}
                    onPanelClose={(panelId) => setVisibleIds((current) => ({ ...current, [panelId]: false }))}
                />
            </PanelStage>
        </ThemeBackdrop>
    );
}

export function PanelRelationshipsShowcase({
    isolateExternalScope = true,
}: {
    readonly isolateExternalScope?: boolean;
}) {
    useStoryTheme();

    const [editorRect, setEditorRect] = React.useState(relationshipsEditorRect);
    const [inspectorRect, setInspectorRect] = React.useState(relationshipsInspectorRect);
    const [externalRect, setExternalRect] = React.useState(relationshipsExternalRect);
    const [events, setEvents] = React.useState<string[]>([
        isolateExternalScope
            ? 'External panel is isolated and should not snap to the alpha pair.'
            : 'All panels share one scope in this story.',
    ]);

    const appendEvent = React.useCallback((entry: string) => {
        setEvents((current) => [entry, ...current].slice(0, 6));
    }, []);

    const alphaScope = 'workspace-alpha';
    const betaScope = isolateExternalScope ? 'workspace-beta' : alphaScope;

    return (
        <ThemeBackdrop label="Interactions · Panel relationships" immersive>
            <PanelStage style={{ inset: '8px 8px 44px 8px' }} label="Panel relationships stage">
                <ResizablePanel
                    id="relationship-editor"
                    title="Editor · alpha"
                    hostMode="container"
                    interactionScope={alphaScope}
                    x={editorRect.x}
                    y={editorRect.y}
                    width={editorRect.width}
                    height={editorRect.height}
                    minWidth={280}
                    minHeight={180}
                    onMove={(_, next) => setEditorRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setEditorRect(rect)}
                    onFocus={() => { }}
                    onAnchorCommit={(targetId, sourceSide) => appendEvent(`Editor snapped ${sourceSide} to ${targetId}`)}
                >
                    <InfoBullets items={['Editor and Inspector share workspace-alpha.', 'They should snap to one another when dragged into range.']} />
                </ResizablePanel>

                <ResizablePanel
                    id="relationship-inspector"
                    title="Inspector · alpha"
                    hostMode="container"
                    interactionScope={alphaScope}
                    x={inspectorRect.x}
                    y={inspectorRect.y}
                    width={inspectorRect.width}
                    height={inspectorRect.height}
                    minWidth={280}
                    minHeight={180}
                    onMove={(_, next) => setInspectorRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setInspectorRect(rect)}
                    onFocus={() => { }}
                    onAnchorCommit={(targetId, sourceSide) => appendEvent(`Inspector snapped ${sourceSide} to ${targetId}`)}
                >
                    <table className="w-full text-xs text-panel-content/90">
                        <tbody>
                            <tr><td>Scope</td><td className="text-panel-accent">{alphaScope}</td></tr>
                            <tr><td>Relationship</td><td className="text-panel-accent">shared</td></tr>
                        </tbody>
                    </table>
                </ResizablePanel>

                <ResizablePanel
                    id="relationship-external"
                    title={isolateExternalScope ? 'External · beta' : 'External · alpha'}
                    hostMode="container"
                    interactionScope={betaScope}
                    x={externalRect.x}
                    y={externalRect.y}
                    width={externalRect.width}
                    height={externalRect.height}
                    minWidth={280}
                    minHeight={180}
                    onMove={(_, next) => setExternalRect((prev) => ({ ...prev, ...next }))}
                    onResize={(_, rect) => setExternalRect(rect)}
                    onFocus={() => { }}
                    onAnchorCommit={(targetId, sourceSide) => appendEvent(`External snapped ${sourceSide} to ${targetId}`)}
                >
                    <div className="space-y-3 text-sm text-panel-content/90">
                        <div className="text-panel-accent">Scope-aware panel relationship</div>
                        <table className="w-full text-xs">
                            <tbody>
                                <tr><td>Scope</td><td className="text-panel-accent">{betaScope}</td></tr>
                                <tr><td>Isolation</td><td className="text-panel-accent">{isolateExternalScope ? 'isolated' : 'shared'}</td></tr>
                            </tbody>
                        </table>
                        <EventList entries={events} />
                    </div>
                </ResizablePanel>
            </PanelStage>
        </ThemeBackdrop>
    );
}