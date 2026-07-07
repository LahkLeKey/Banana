import { useMemo, useState } from 'react';
import { Navigate } from 'react-router-dom';
import {
    RouteActionLink,
    RouteActionsRow,
    RouteBody,
    RouteEyebrow,
    RouteInfoCard,
    RouteInfoGrid,
    RoutePanel,
    RouteTitle,
    clearStoredAuthSession,
    readStoredAuthSession,
} from '@banana/ui';

import { GameplayShell } from '../components/gameplay/GameplayShell';
import { buildProtectedLoginPath } from '../lib/authRouting';
import { clearSelectedCharacter, readSelectedCharacter } from '../lib/gameClientFlow';

const WORLD_MAP_BACKGROUND =
    'radial-gradient(circle at 16% 18%, rgba(34, 197, 94, 0.18), transparent 28%), radial-gradient(circle at 82% 14%, rgba(14, 165, 233, 0.2), transparent 32%), radial-gradient(circle at 50% 78%, rgba(168, 85, 247, 0.12), transparent 36%), linear-gradient(150deg, #02101c 0%, #071625 46%, #10233d 100%)';

type RegionNode = {
    readonly id: string;
    readonly label: string;
    readonly top: string;
    readonly left: string;
    readonly tone: string;
    readonly summary: string;
    readonly risk: 'low' | 'medium' | 'high';
};

const REGION_NODES: readonly RegionNode[] = [
    {
        id: 'north-reach',
        label: 'North Reach',
        top: '14%',
        left: '18%',
        tone: '#22c55e',
        summary: 'Stable uplink towers and low hostiles. Best entry for warm-up objectives.',
        risk: 'low',
    },
    {
        id: 'relay-coast',
        label: 'Relay Coast',
        top: '32%',
        left: '66%',
        tone: '#38bdf8',
        summary: 'Dense signal traffic and contested relays. Good for intel sweeps.',
        risk: 'medium',
    },
    {
        id: 'delta-basin',
        label: 'Delta Basin',
        top: '58%',
        left: '26%',
        tone: '#f59e0b',
        summary: 'Anomaly bloom detected. Resource-rich but rapidly shifting topology.',
        risk: 'high',
    },
    {
        id: 'frontier-ring',
        label: 'Frontier Ring',
        top: '70%',
        left: '72%',
        tone: '#a78bfa',
        summary: 'Long-range frontier arcs. Strategic for expansion and deep-scan patrols.',
        risk: 'medium',
    },
];

type Hotspot = {
    readonly id: string;
    readonly title: string;
    readonly regionId: string;
    readonly priority: 'P1' | 'P2' | 'P3';
    readonly note: string;
};

const HOTSPOTS: readonly Hotspot[] = [
    {
        id: 'north-uplink',
        title: 'Uplink Sigma',
        regionId: 'north-reach',
        priority: 'P3',
        note: 'Fast travel lane and low-noise telemetry source.',
    },
    {
        id: 'relay-junction',
        title: 'Relay Junction 7',
        regionId: 'relay-coast',
        priority: 'P2',
        note: 'Frequent packet loss; monitor continuity before committing squads.',
    },
    {
        id: 'delta-core',
        title: 'Delta Core Fault',
        regionId: 'delta-basin',
        priority: 'P1',
        note: 'Highest value target. Heavy instability and enemy pressure expected.',
    },
    {
        id: 'frontier-gate',
        title: 'Frontier Gate',
        regionId: 'frontier-ring',
        priority: 'P2',
        note: 'Open expansion lane with intermittent hostile probes.',
    },
];

export function WorldMapPage() {
    const session = readStoredAuthSession();

    if (!session?.token) {
        return <Navigate to={buildProtectedLoginPath('/world-map')} replace />;
    }

    const activeOperator = readSelectedCharacter();
    const [focusedRegionId, setFocusedRegionId] = useState(REGION_NODES[0].id);

    const handleSignOut = () => {
        clearStoredAuthSession();
        clearSelectedCharacter();
        if (typeof window !== 'undefined') {
            window.location.assign('/login');
        }
    };

    const focusedRegion = useMemo(
        () => REGION_NODES.find((node) => node.id === focusedRegionId) ?? REGION_NODES[0],
        [focusedRegionId],
    );

    const regionHotspots = useMemo(
        () => HOTSPOTS.filter((hotspot) => hotspot.regionId === focusedRegion.id),
        [focusedRegion.id],
    );

    if (!activeOperator) {
        return (
            <GameplayShell
                routeLabel="World Map"
                title="World map locked"
                subtitle="Choose an operator before opening the visualization slice."
                activeOperator={undefined}
                onSignOut={handleSignOut}
                background={WORLD_MAP_BACKGROUND}
            >
                <RoutePanel width="min(860px, 100%)">
                    <RouteEyebrow color="#7dd3fc">Prerequisite missing</RouteEyebrow>
                    <RouteTitle>Pick your current operator first.</RouteTitle>
                    <RouteBody maxWidth={760}>
                        The world map is a gated route. Select a character first, then return for quick navigation and hotspot tracking.
                    </RouteBody>
                    <RouteActionsRow marginTop={22}>
                        <RouteActionLink href="/characters" tone="primary">
                            Choose operator
                        </RouteActionLink>
                        <RouteActionLink href="/" tone="neutral">
                            Back to home
                        </RouteActionLink>
                    </RouteActionsRow>
                </RoutePanel>
            </GameplayShell>
        );
    }

    return (
        <GameplayShell
            routeLabel="World Map"
            title="Exploration command"
            subtitle={`${activeOperator} scoped. Quick navigation and hotspot monitoring online.`}
            activeOperator={activeOperator}
            onSignOut={handleSignOut}
            background={WORLD_MAP_BACKGROUND}
        >
            <RoutePanel width="100%">
                <RouteEyebrow color="#67e8f9">Visualization slice</RouteEyebrow>
                <RouteTitle>World map</RouteTitle>
                <RouteBody maxWidth={980}>
                    Select a region on the map or from quick navigation. The right rail tracks mission hotspots and actionable priorities for the focused zone.
                </RouteBody>

                <div style={surfaceLayoutStyle}>
                    <section style={mapFrameStyle}>
                        <div style={mapGridStyle}>
                            {REGION_NODES.map((node) => {
                                const isFocused = node.id === focusedRegion.id;
                                return (
                                    <button
                                        key={node.id}
                                        type="button"
                                        onClick={() => setFocusedRegionId(node.id)}
                                        style={{
                                            ...regionNodeStyle,
                                            ...(isFocused ? focusedNodeStyle : null),
                                            top: node.top,
                                            left: node.left,
                                            borderColor: `${node.tone}88`,
                                            boxShadow: `0 0 0 1px ${node.tone}33, 0 0 26px ${node.tone}22`,
                                        }}
                                    >
                                        <span style={{ ...regionDotStyle, background: node.tone }} />
                                        <span style={regionLabelStyle}>{node.label}</span>
                                    </button>
                                );
                            })}
                            <div style={pulseRingStyle} />
                            <div style={pulseRingOuterStyle} />
                        </div>

                        <div style={focusedRegionBarStyle}>
                            <span style={focusedRegionBadgeStyle}>{focusedRegion.risk.toUpperCase()} RISK</span>
                            <strong style={focusedRegionNameStyle}>{focusedRegion.label}</strong>
                            <span style={focusedRegionSummaryStyle}>{focusedRegion.summary}</span>
                        </div>
                    </section>

                    <aside style={rightRailStyle}>
                        <section style={railCardStyle}>
                            <h3 style={railTitleStyle}>Quick navigation</h3>
                            <div style={quickNavListStyle}>
                                {REGION_NODES.map((node) => (
                                    <button
                                        key={node.id}
                                        type="button"
                                        onClick={() => setFocusedRegionId(node.id)}
                                        style={{
                                            ...quickNavButtonStyle,
                                            ...(node.id === focusedRegion.id ? quickNavButtonActiveStyle : null),
                                        }}
                                    >
                                        <span style={{ ...quickNavDotStyle, background: node.tone }} />
                                        <span>{node.label}</span>
                                    </button>
                                ))}
                            </div>
                        </section>

                        <section style={railCardStyle}>
                            <h3 style={railTitleStyle}>Hotspots</h3>
                            <div style={hotspotListStyle}>
                                {regionHotspots.map((hotspot) => (
                                    <article key={hotspot.id} style={hotspotCardStyle}>
                                        <div style={hotspotHeaderStyle}>
                                            <strong style={hotspotTitleStyle}>{hotspot.title}</strong>
                                            <span style={hotspotPriorityStyle}>{hotspot.priority}</span>
                                        </div>
                                        <p style={hotspotNoteStyle}>{hotspot.note}</p>
                                    </article>
                                ))}
                            </div>
                        </section>

                        <section style={railCardStyle}>
                            <h3 style={railTitleStyle}>Legacy notebook</h3>
                            <p style={hotspotNoteStyle}>
                                Need the original hypersphere notebook-client surface? Open it as a live reference while new components are scaffolded.
                            </p>
                            <RouteActionLink href="/notebooks" tone="neutral">
                                Open notebook client
                            </RouteActionLink>
                        </section>
                    </aside>
                </div>

                <RouteInfoGrid>
                    <RouteInfoCard title="Active operator">
                        <RouteBody maxWidth={320}>{activeOperator}</RouteBody>
                    </RouteInfoCard>
                    <RouteInfoCard title="Focused region">
                        <RouteBody maxWidth={320}>{focusedRegion.label}</RouteBody>
                    </RouteInfoCard>
                    <RouteInfoCard title="Hotspots in view">
                        <RouteBody maxWidth={320}>{regionHotspots.length}</RouteBody>
                    </RouteInfoCard>
                </RouteInfoGrid>
            </RoutePanel>
        </GameplayShell>
    );
}

const surfaceLayoutStyle = {
    marginTop: 18,
    display: 'grid',
    gridTemplateColumns: '2fr 1fr',
    gap: 14,
    alignItems: 'start',
};

const mapFrameStyle = {
    borderRadius: 28,
    border: '1px solid rgba(148, 163, 184, 0.18)',
    background: 'linear-gradient(180deg, rgba(2, 10, 22, 0.88), rgba(7, 18, 36, 0.9))',
    padding: 14,
    display: 'grid',
    gap: 10,
};

const rightRailStyle = {
    display: 'grid',
    gap: 12,
    alignContent: 'start',
};

const railCardStyle = {
    borderRadius: 16,
    border: '1px solid rgba(148, 163, 184, 0.2)',
    background: 'rgba(8, 13, 28, 0.62)',
    padding: 12,
    display: 'grid',
    gap: 10,
};

const railTitleStyle = {
    margin: 0,
    fontSize: 14,
    textTransform: 'uppercase' as const,
    letterSpacing: '0.08em',
    color: '#7dd3fc',
};

const quickNavListStyle = {
    display: 'grid',
    gap: 8,
};

const quickNavButtonStyle = {
    borderRadius: 10,
    border: '1px solid rgba(148, 163, 184, 0.24)',
    background: 'rgba(2, 6, 23, 0.5)',
    color: '#e2e8f0',
    display: 'flex',
    alignItems: 'center',
    gap: 8,
    padding: '9px 10px',
    cursor: 'pointer',
    fontWeight: 700,
    textAlign: 'left' as const,
};

const quickNavButtonActiveStyle = {
    border: '1px solid rgba(56, 189, 248, 0.52)',
    background: 'rgba(8, 47, 73, 0.52)',
};

const quickNavDotStyle = {
    width: 9,
    height: 9,
    borderRadius: 999,
    boxShadow: '0 0 12px currentColor',
};

const hotspotListStyle = {
    display: 'grid',
    gap: 8,
};

const hotspotCardStyle = {
    borderRadius: 10,
    border: '1px solid rgba(148, 163, 184, 0.2)',
    background: 'rgba(2, 6, 23, 0.45)',
    padding: '10px 10px',
    display: 'grid',
    gap: 6,
};

const hotspotHeaderStyle = {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    gap: 8,
};

const hotspotTitleStyle = {
    fontSize: 13,
    color: '#f8fafc',
};

const hotspotPriorityStyle = {
    fontSize: 10,
    fontWeight: 800,
    letterSpacing: '0.08em',
    textTransform: 'uppercase' as const,
    color: '#fca5a5',
};

const hotspotNoteStyle = {
    margin: 0,
    fontSize: 12,
    lineHeight: 1.4,
    color: '#cbd5e1',
};

const mapGridStyle = {
    position: 'relative' as const,
    minHeight: 520,
    borderRadius: 22,
    overflow: 'hidden',
    background:
        'radial-gradient(circle at 20% 24%, rgba(34, 197, 94, 0.08), transparent 28%), radial-gradient(circle at 72% 26%, rgba(14, 165, 233, 0.1), transparent 30%), radial-gradient(circle at 50% 50%, rgba(56, 189, 248, 0.05), transparent 42%), linear-gradient(145deg, rgba(2, 8, 20, 0.94), rgba(6, 16, 32, 0.98))',
    border: '1px solid rgba(45, 212, 191, 0.16)',
};

const focusedRegionBarStyle = {
    borderRadius: 14,
    border: '1px solid rgba(148, 163, 184, 0.2)',
    background: 'rgba(2, 6, 23, 0.52)',
    padding: '10px 12px',
    display: 'grid',
    gap: 4,
};

const focusedRegionBadgeStyle = {
    fontSize: 10,
    fontWeight: 800,
    letterSpacing: '0.1em',
    textTransform: 'uppercase' as const,
    color: '#fca5a5',
};

const focusedRegionNameStyle = {
    fontSize: 16,
    color: '#f8fafc',
};

const focusedRegionSummaryStyle = {
    fontSize: 13,
    lineHeight: 1.45,
    color: '#cbd5e1',
};

const regionNodeStyle = {
    position: 'absolute' as const,
    display: 'inline-flex',
    alignItems: 'center',
    gap: 8,
    padding: '8px 12px',
    borderRadius: 999,
    background: 'rgba(8, 13, 28, 0.75)',
    border: '1px solid rgba(148, 163, 184, 0.18)',
    backdropFilter: 'blur(10px)',
    cursor: 'pointer',
};

const focusedNodeStyle = {
    transform: 'translateY(-1px) scale(1.02)',
};

const regionDotStyle = {
    width: 10,
    height: 10,
    borderRadius: 999,
    boxShadow: '0 0 18px currentColor',
};

const regionLabelStyle = {
    fontSize: 12,
    fontWeight: 800,
    color: '#e2e8f0',
    letterSpacing: '0.04em',
    textTransform: 'uppercase' as const,
};

const pulseRingStyle = {
    position: 'absolute' as const,
    inset: '18% 26%',
    borderRadius: '50%',
    border: '1px solid rgba(56, 189, 248, 0.22)',
    boxShadow: '0 0 0 1px rgba(56, 189, 248, 0.06), inset 0 0 0 1px rgba(56, 189, 248, 0.05)',
};

const pulseRingOuterStyle = {
    position: 'absolute' as const,
    inset: '8% 16%',
    borderRadius: '50%',
    border: '1px dashed rgba(167, 139, 250, 0.16)',
};
