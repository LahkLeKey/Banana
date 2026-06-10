import type { Dispatch, SetStateAction } from 'react';
import { RouteSubActionBar } from '@banana/ui';

import type { IntelEvent } from '../../domain/notebook/intel-domain';
import type { QuestProgress } from '../../lib/notebook-client/gamification';

type IntelDockPanelProps = {
    readonly missionTitle: string;
    readonly missionCode: string;
    readonly sectorReadiness: number;
    readonly intelMode: 'summary' | 'raw';
    readonly setIntelMode: Dispatch<SetStateAction<'summary' | 'raw'>>;
    readonly developerIntelUnlocked: boolean;
    readonly toggleDeveloperUnlock: () => void;
    readonly intelPanel: 'threat' | 'signal' | 'route';
    readonly setIntelPanel: Dispatch<SetStateAction<'threat' | 'signal' | 'route'>>;
    readonly hotspotScore: number;
    readonly riskLabel: 'Critical' | 'Elevated' | 'Guarded' | 'Stable';
    readonly branchPressure: number;
    readonly dependencyPulse: number;
    readonly callDensity: number;
    readonly functionName: string;
    readonly functionCount: number;
    readonly primaryDependencyLabel: string;
    readonly includeCount: number;
    readonly intelEvents: IntelEvent[];
    readonly showFullSourceDump: boolean;
    readonly setShowFullSourceDump: Dispatch<SetStateAction<boolean>>;
    readonly sourcePreviewWithLineNumbers: string;
};

export function IntelDockPanel({
    missionTitle,
    missionCode,
    sectorReadiness,
    intelMode,
    setIntelMode,
    developerIntelUnlocked,
    toggleDeveloperUnlock,
    intelPanel,
    setIntelPanel,
    hotspotScore,
    riskLabel,
    branchPressure,
    dependencyPulse,
    callDensity,
    functionName,
    functionCount,
    primaryDependencyLabel,
    includeCount,
    intelEvents,
    showFullSourceDump,
    setShowFullSourceDump,
    sourcePreviewWithLineNumbers,
}: IntelDockPanelProps) {
    const intelBodyMaxHeight = intelMode === 'raw'
        ? (showFullSourceDump ? '29dvh' : '24dvh')
        : '18dvh';

    return (
        <div style={{ display: 'flex', flexDirection: 'column', gap: 0, minHeight: 0 }}>
            <RouteSubActionBar
                tabs={[
                    { id: 'summary', label: 'Tactical' },
                    { id: 'raw', label: developerIntelUnlocked ? 'Raw Intel' : 'Raw Intel 🔒' },
                ]}
                activeTab={intelMode}
                onTabChange={(id) => {
                    if (id === 'raw' && !developerIntelUnlocked) { return; }
                    setIntelMode(id as 'summary' | 'raw');
                }}
                actions={
                    <button
                        type="button"
                        onClick={toggleDeveloperUnlock}
                        style={{ border: '1px solid rgba(148,163,184,0.3)', borderRadius: 6, background: developerIntelUnlocked ? 'rgba(30,41,59,0.9)' : 'rgba(15,23,42,0.6)', color: developerIntelUnlocked ? '#86efac' : '#94a3b8', fontSize: 10, fontWeight: 700, letterSpacing: '0.05em', padding: '3px 7px', cursor: 'pointer', textTransform: 'uppercase' }}
                    >
                        {developerIntelUnlocked ? 'Dev On' : 'Dev'}
                    </button>
                }
                meta={`${missionTitle} · ${missionCode} · ${sectorReadiness}% ready`}
            />

            {/* removed: old header div and loose button row */}
            <div style={{ maxHeight: intelBodyMaxHeight, overflowY: 'auto', paddingRight: 4, display: 'grid', gap: 8 }}>
                {intelMode === 'summary' ? (
                    <>
                        <div style={{ display: 'flex', gap: 8, flexWrap: 'wrap' }}>
                            {([
                                { key: 'threat', label: 'Threat' },
                                { key: 'signal', label: 'Signals' },
                                { key: 'route', label: 'Routing' },
                            ] as const).map((item) => (
                                <button
                                    key={item.key}
                                    type="button"
                                    onClick={() => setIntelPanel(item.key)}
                                    style={{
                                        borderRadius: 999,
                                        border: '1px solid rgba(148, 163, 184, 0.4)',
                                        background: intelPanel === item.key ? 'rgba(8, 47, 73, 0.88)' : 'rgba(15, 23, 42, 0.62)',
                                        color: intelPanel === item.key ? '#e0f2fe' : '#cbd5e1',
                                        padding: '4px 10px',
                                        fontSize: 11,
                                        fontWeight: 700,
                                        letterSpacing: '0.04em',
                                        textTransform: 'uppercase',
                                        cursor: 'pointer',
                                    }}
                                >
                                    {item.label}
                                </button>
                            ))}
                        </div>

                        {intelPanel === 'threat' ? (
                            <div style={{ borderRadius: 10, border: '1px solid rgba(244, 114, 182, 0.28)', background: 'rgba(131, 24, 67, 0.22)', padding: 10 }}>
                                <div style={{ fontSize: 10, color: '#f9a8d4', textTransform: 'uppercase', letterSpacing: '0.08em' }}>Hotspot</div>
                                <div style={{ marginTop: 6, display: 'flex', justifyContent: 'space-between', alignItems: 'baseline' }}>
                                    <span style={{ color: '#fce7f3', fontWeight: 800, fontSize: 22 }}>{hotspotScore}</span>
                                    <span style={{ color: '#fbcfe8', fontSize: 12, fontWeight: 700 }}>{riskLabel}</span>
                                </div>
                                <div style={{ marginTop: 7, color: '#fbcfe8', fontSize: 12 }}>
                                    Frontline pressure {branchPressure}% · Logistics load {dependencyPulse}%
                                </div>
                            </div>
                        ) : null}

                        {intelPanel === 'signal' ? (
                            <div style={{ borderRadius: 10, border: '1px solid rgba(167, 139, 250, 0.28)', background: 'rgba(49, 46, 129, 0.22)', padding: 10 }}>
                                <div style={{ fontSize: 10, color: '#c4b5fd', textTransform: 'uppercase', letterSpacing: '0.08em' }}>Signal Density</div>
                                <div style={{ marginTop: 6, color: '#ede9fe', fontWeight: 800, fontSize: 20 }}>{callDensity}%</div>
                                <div style={{ marginTop: 7, color: '#ddd6fe', fontSize: 12 }}>
                                    Lead command relay {functionName} with {functionCount} active signal points.
                                </div>
                            </div>
                        ) : null}

                        {intelPanel === 'route' ? (
                            <div style={{ borderRadius: 10, border: '1px solid rgba(103, 232, 249, 0.3)', background: 'rgba(8, 47, 73, 0.32)', padding: 10 }}>
                                <div style={{ fontSize: 10, color: '#67e8f9', textTransform: 'uppercase', letterSpacing: '0.08em' }}>Supply Route</div>
                                <div style={{ marginTop: 6, color: '#cffafe', fontWeight: 700, fontSize: 16 }}>
                                    {primaryDependencyLabel}
                                </div>
                                <div style={{ marginTop: 7, color: '#bae6fd', fontSize: 12 }}>
                                    {includeCount} support channels connected to this mission lane.
                                </div>
                            </div>
                        ) : null}
                    </>
                ) : (
                    <>
                        <div style={{
                            borderRadius: 10,
                            border: '1px solid rgba(45, 212, 191, 0.32)',
                            background: 'rgba(6, 18, 32, 0.65)',
                            padding: 10,
                        }}>
                            <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#5eead4', marginBottom: 6 }}>
                                Mission Event Feed
                            </div>
                            <div style={{ display: 'grid', gap: 6 }}>
                                {intelEvents.map((event) => (
                                    <div key={event.id} style={{
                                        borderRadius: 8,
                                        border: '1px solid rgba(148, 163, 184, 0.28)',
                                        background: 'rgba(15, 23, 42, 0.62)',
                                        padding: '6px 8px',
                                        display: 'grid',
                                        gap: 3,
                                    }}>
                                        <div style={{
                                            fontSize: 11,
                                            textTransform: 'uppercase',
                                            letterSpacing: '0.06em',
                                            color: event.severity === 'high' ? '#fda4af' : event.severity === 'medium' ? '#fcd34d' : '#86efac',
                                            fontWeight: 700,
                                        }}>
                                            {event.label}
                                        </div>
                                        <div style={{
                                            color: '#cbd5e1',
                                            fontSize: 12,
                                            lineHeight: 1.45,
                                        }}>
                                            {event.detail}
                                        </div>
                                    </div>
                                ))}
                            </div>
                        </div>

                        <div style={{ display: 'flex', gap: 8, alignItems: 'center', flexWrap: 'wrap' }}>
                            <button
                                type="button"
                                onClick={() => setShowFullSourceDump((previous) => !previous)}
                                style={{
                                    borderRadius: 999,
                                    border: '1px solid rgba(148, 163, 184, 0.45)',
                                    background: showFullSourceDump ? 'rgba(127, 29, 29, 0.72)' : 'rgba(30, 41, 59, 0.72)',
                                    color: showFullSourceDump ? '#fecaca' : '#cbd5e1',
                                    padding: '5px 11px',
                                    fontSize: 11,
                                    fontWeight: 700,
                                    letterSpacing: '0.04em',
                                    cursor: 'pointer',
                                    textTransform: 'uppercase',
                                }}
                            >
                                {showFullSourceDump ? 'Hide Full Source Dump' : 'Reveal Full Source Dump'}
                            </button>
                            <span style={{ fontSize: 11, color: '#94a3b8' }}>
                                Tactical mode stays primary so frontline intel is readable at a glance.
                            </span>
                        </div>

                        {showFullSourceDump ? (
                            <div style={{
                                borderRadius: 10,
                                border: '1px solid rgba(239, 68, 68, 0.36)',
                                background: 'radial-gradient(circle at top left, rgba(127, 29, 29, 0.22), rgba(2, 6, 23, 0.96) 38%)',
                                fontFamily: '"IBM Plex Mono", SFMono-Regular, Menlo, Consolas, monospace',
                                fontSize: 12,
                                lineHeight: 1.55,
                                color: '#fecaca',
                                overflowX: 'auto',
                                overflowY: 'visible',
                                whiteSpace: 'pre',
                                overflowWrap: 'anywhere',
                                wordBreak: 'break-word',
                                padding: 10,
                            }}>
                                {sourcePreviewWithLineNumbers}
                            </div>
                        ) : null}
                    </>
                )}
            </div>
        </div>
    );
}

type ObjectivesDockPanelProps = {
    readonly completedQuestCount: number;
    readonly questProgress: QuestProgress[];
    readonly questPercent: number;
    readonly objectivePanel: 'board' | 'timeline';
    readonly setObjectivePanel: Dispatch<SetStateAction<'board' | 'timeline'>>;
    readonly nextDomainTarget: string;
};

export function ObjectivesDockPanel({
    completedQuestCount,
    questProgress,
    questPercent,
    objectivePanel,
    setObjectivePanel,
    nextDomainTarget,
}: ObjectivesDockPanelProps) {
    const objectivesBodyStyle = objectivePanel === 'timeline'
        ? { maxHeight: 'none', overflowY: 'visible' as const }
        : { maxHeight: '24dvh', overflowY: 'auto' as const };

    return (
        <div style={{ display: 'flex', flexDirection: 'column', gap: 0, minHeight: 0 }}>
            <RouteSubActionBar
                tabs={[
                    { id: 'board', label: 'Quest Board' },
                    { id: 'timeline', label: 'Timeline' },
                ]}
                activeTab={objectivePanel}
                onTabChange={(id) => setObjectivePanel(id as 'board' | 'timeline')}
                meta={`${completedQuestCount}/${questProgress.length} complete · ${questPercent}%`}
            />
            {/* removed: old loose button row — promoted to RouteSubActionBar */}

            <div style={{ ...objectivesBodyStyle, paddingRight: 4, display: 'grid', gap: 8 }}>
                {objectivePanel === 'timeline' ? (
                    <>
                        <div style={{ height: 8, borderRadius: 999, background: 'rgba(30, 41, 59, 0.76)', overflow: 'hidden' }}>
                            <div style={{ width: `${questPercent}%`, height: '100%', background: 'linear-gradient(90deg, #38bdf8, #34d399, #f59e0b)' }} />
                        </div>
                        <div style={{ fontSize: 12, color: '#cbd5e1' }}>
                            Next hunt: <strong style={{ color: '#e9d5ff', textTransform: 'uppercase' }}>capture {nextDomainTarget} sector</strong>
                        </div>
                    </>
                ) : (
                    <div style={{ display: 'grid', gap: 6 }}>
                        {questProgress.map((quest) => (
                            <div key={quest.id} style={{
                                display: 'flex',
                                justifyContent: 'space-between',
                                gap: 10,
                                alignItems: 'center',
                                fontSize: 12,
                                color: '#cbd5e1',
                                borderRadius: 8,
                                border: `1px solid ${quest.completed ? 'rgba(74, 222, 128, 0.5)' : 'rgba(148, 163, 184, 0.32)'}`,
                                background: quest.completed ? 'rgba(20, 83, 45, 0.45)' : 'rgba(15, 23, 42, 0.52)',
                                padding: '6px 8px',
                            }}>
                                <span>{quest.title}</span>
                                <span style={{
                                    color: quest.completed ? '#86efac' : '#7dd3fc',
                                    fontWeight: 700,
                                    whiteSpace: 'nowrap',
                                }}>
                                    {quest.progress}/{quest.target}
                                </span>
                            </div>
                        ))}
                    </div>
                )}
            </div>
        </div>
    );
}

type PlayerDockPanelProps = {
    readonly playerPanel: 'rank' | 'economy' | 'streak';
    readonly setPlayerPanel: Dispatch<SetStateAction<'rank' | 'economy' | 'streak'>>;
    readonly playerLevel: number;
    readonly rarityLabel: string;
    readonly rarityAccent: string;
    readonly totalXp: number;
    readonly lastXpGain: number;
    readonly comboStreak: number;
};

export function PlayerDockPanel({
    playerPanel,
    setPlayerPanel,
    playerLevel,
    rarityLabel,
    rarityAccent,
    totalXp,
    lastXpGain,
    comboStreak,
}: PlayerDockPanelProps) {
    return (
        <div style={{ display: 'flex', flexDirection: 'column', gap: 0, minHeight: 0 }}>
            <RouteSubActionBar
                tabs={[
                    { id: 'rank', label: 'Rank' },
                    { id: 'economy', label: 'Economy' },
                    { id: 'streak', label: 'Momentum' },
                ]}
                activeTab={playerPanel}
                onTabChange={(id) => setPlayerPanel(id as 'rank' | 'economy' | 'streak')}
            />

            <div style={{ paddingRight: 4 }}>
                {playerPanel === 'rank' ? (
                    <div style={{ borderRadius: 10, border: '1px solid rgba(167, 139, 250, 0.3)', background: 'rgba(76, 29, 149, 0.24)', padding: 10 }}>
                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#c4b5fd' }}>Commander Level</div>
                        <div style={{ marginTop: 6, fontSize: 24, fontWeight: 700, color: '#ede9fe' }}>{playerLevel}</div>
                        <div style={{ marginTop: 7, fontSize: 12, color: '#ddd6fe' }}>Current mission rarity: <strong style={{ color: rarityAccent }}>{rarityLabel}</strong></div>
                    </div>
                ) : null}

                {playerPanel === 'economy' ? (
                    <div style={{ borderRadius: 10, border: '1px solid rgba(34, 211, 238, 0.3)', background: 'rgba(8, 47, 73, 0.34)', padding: 10 }}>
                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#67e8f9' }}>Experience Bank</div>
                        <div style={{ marginTop: 6, fontSize: 24, fontWeight: 700, color: '#cffafe' }}>{totalXp}</div>
                        <div style={{ marginTop: 7, fontSize: 12, color: '#bae6fd' }}>Last reward +{lastXpGain} XP</div>
                    </div>
                ) : null}

                {playerPanel === 'streak' ? (
                    <div style={{ borderRadius: 10, border: '1px solid rgba(244, 114, 182, 0.3)', background: 'rgba(131, 24, 67, 0.24)', padding: 10 }}>
                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#f9a8d4' }}>Combo Momentum</div>
                        <div style={{ marginTop: 6, fontSize: 24, fontWeight: 700, color: '#fbcfe8' }}>x{Math.max(1, comboStreak)}</div>
                        <div style={{ marginTop: 7, fontSize: 12, color: '#fbcfe8' }}>Maintain chain for bonus intel rewards.</div>
                    </div>
                ) : null}
            </div>
        </div>
    );
}

type DockToggleBarProps = {
    readonly activeDock: 'intel' | 'objectives' | 'player' | null;
    readonly setActiveDock: Dispatch<SetStateAction<'intel' | 'objectives' | 'player' | null>>;
};

export function DockToggleBar({ activeDock, setActiveDock }: DockToggleBarProps) {
    return (
        <div style={{
            display: 'flex',
            justifyContent: 'center',
            gap: 8,
            flexWrap: 'wrap',
        }}>
            {(['intel', 'objectives', 'player'] as const).map((dock) => (
                <button
                    key={dock}
                    type="button"
                    onClick={() => setActiveDock((previous) => (previous === dock ? null : dock))}
                    style={{
                        borderRadius: 999,
                        border: '1px solid rgba(148, 163, 184, 0.38)',
                        background: activeDock === dock ? 'rgba(8, 47, 73, 0.9)' : 'rgba(15, 23, 42, 0.7)',
                        color: activeDock === dock ? '#e0f2fe' : '#cbd5e1',
                        padding: '8px 14px',
                        fontSize: 12,
                        fontWeight: 700,
                        letterSpacing: '0.06em',
                        textTransform: 'uppercase',
                        cursor: 'pointer',
                        boxShadow: activeDock === dock ? '0 0 18px rgba(56, 189, 248, 0.24)' : 'none',
                    }}
                >
                    {dock}
                </button>
            ))}
        </div>
    );
}
