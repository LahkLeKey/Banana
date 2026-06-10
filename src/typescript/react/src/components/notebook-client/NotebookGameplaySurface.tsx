import { useEffect, useMemo, useState, type CSSProperties } from 'react';

import type { QuestProgress } from '../../lib/notebook-client/gamification';
import { Pill, SurfaceCard } from './SurfacePrimitives';

type NotebookGameplaySurfaceProps = {
    readonly selectedFile: string;
    readonly selectedContent: string;
    readonly fullBleed?: boolean;
    readonly indexedFileCount?: number;
    readonly notebookCellCount?: number;
    readonly selectedDomain?: string;
    readonly rarityLabel?: string;
    readonly rarityAccent?: string;
    readonly playerLevel?: number;
    readonly totalXp?: number;
    readonly comboStreak?: number;
    readonly lastXpGain?: number;
    readonly questProgress?: QuestProgress[];
    readonly questToast?: string;
    readonly nextDomainTarget?: string;
};

type IntelEvent = {
    readonly id: string;
    readonly label: string;
    readonly severity: 'low' | 'medium' | 'high';
    readonly detail: string;
};

function deriveSectorMissionMeta(file: string): { title: string; domain: string; code: string; } {
    if (!file) {
        return { title: 'Unassigned Sector', domain: 'Other', code: 'OT-000' };
    }

    const normalized = file.replace(/\\/g, '/');
    const lane = normalized.split('/').filter(Boolean).pop() ?? normalized;
    const title = lane
        .replace(/\.[^.]+$/, '')
        .replace(/[_-]+/g, ' ')
        .replace(/\b\w/g, (match) => match.toUpperCase());

    let domain = 'Other';
    if (normalized.includes('/ai/')) {
        domain = 'AI';
    } else if (normalized.includes('/physics/')) {
        domain = 'Physics';
    } else if (normalized.includes('/render/')) {
        domain = 'Render';
    } else if (normalized.includes('/world/')) {
        domain = 'World';
    } else if (normalized.includes('/runtime/')) {
        domain = 'Runtime';
    }

    const checksum = Math.abs(normalized.split('').reduce((acc, char) => acc + char.charCodeAt(0), 0)) % 1000;
    return {
        title,
        domain,
        code: `${domain.slice(0, 2).toUpperCase()}-${String(checksum).padStart(3, '0')}`,
    };
}

function normalizeDependencyLabel(rawTarget: string): string {
    const cleaned = rawTarget.replace(/["<>]/g, '').split('/').pop() ?? rawTarget;
    return cleaned
        .replace(/\.[^.]+$/, '')
        .replace(/[_-]+/g, ' ')
        .replace(/\b\w/g, (match) => match.toUpperCase());
}

const codeSurfaceStyle: CSSProperties = {
    width: '100%',
    minHeight: 'min(440px, 56dvh)',
    maxHeight: '62dvh',
    borderRadius: 12,
    border: '1px solid rgba(45, 212, 191, 0.35)',
    background: 'radial-gradient(circle at top left, rgba(20, 184, 166, 0.14), rgba(2, 6, 23, 0.95) 30%)',
    fontFamily: '"IBM Plex Mono", SFMono-Regular, Menlo, Consolas, monospace',
    fontSize: 13,
    lineHeight: 1.55,
    color: '#d1fae5',
    overflow: 'auto',
    whiteSpace: 'pre-wrap',
    overflowWrap: 'anywhere',
    wordBreak: 'break-word',
    padding: 14,
    position: 'relative',
};

// Scanline overlay for the fullBleed stage — gives a subtle CRT feel
const scanlineUri = `url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='2' height='4'%3E%3Crect width='2' height='2' fill='rgba(0,0,0,0.08)'/%3E%3C/svg%3E")`;

const domainTheme: Record<string, { aura: string; accent: string; panel: string; radar: string; }> = {
    ai: {
        aura: 'rgba(16, 185, 129, 0.24)',
        accent: '#6ee7b7',
        panel: 'rgba(6, 33, 25, 0.86)',
        radar: '#34d399',
    },
    physics: {
        aura: 'rgba(59, 130, 246, 0.24)',
        accent: '#93c5fd',
        panel: 'rgba(8, 22, 42, 0.86)',
        radar: '#60a5fa',
    },
    render: {
        aura: 'rgba(168, 85, 247, 0.24)',
        accent: '#d8b4fe',
        panel: 'rgba(28, 18, 45, 0.86)',
        radar: '#c084fc',
    },
    world: {
        aura: 'rgba(245, 158, 11, 0.22)',
        accent: '#fcd34d',
        panel: 'rgba(42, 24, 9, 0.86)',
        radar: '#f59e0b',
    },
    runtime: {
        aura: 'rgba(20, 184, 166, 0.22)',
        accent: '#5eead4',
        panel: 'rgba(6, 29, 33, 0.86)',
        radar: '#2dd4bf',
    },
    other: {
        aura: 'rgba(14, 165, 233, 0.18)',
        accent: '#67e8f9',
        panel: 'rgba(5, 22, 34, 0.86)',
        radar: '#22d3ee',
    },
};

export function NotebookGameplaySurface({
    selectedFile,
    selectedContent,
    fullBleed = false,
    indexedFileCount = 0,
    notebookCellCount = 0,
    selectedDomain = 'other',
    rarityLabel = 'Common',
    rarityAccent = '#86efac',
    playerLevel = 1,
    totalXp = 0,
    comboStreak = 0,
    lastXpGain = 0,
    questProgress = [],
    questToast = '',
    nextDomainTarget = 'runtime',
}: NotebookGameplaySurfaceProps) {
    const [intelMode, setIntelMode] = useState<'summary' | 'raw'>('summary');
    const [developerIntelUnlocked, setDeveloperIntelUnlocked] = useState(false);
    const [activeDock, setActiveDock] = useState<'intel' | 'objectives' | 'player' | null>('intel');
    const [showFullSourceDump, setShowFullSourceDump] = useState(false);
    const lineCount = selectedContent.split('\n').length;
    const selectedLines = useMemo(
        () => selectedContent.split('\n'),
        [selectedContent],
    );
    const functionCount = useMemo(
        () => (selectedContent.match(/\b[a-zA-Z_]\w*\s*\(/g) ?? []).length,
        [selectedContent],
    );
    const functionNames = useMemo(() => {
        const matches = selectedContent.match(/\b([a-zA-Z_]\w*)\s*\(/g) ?? [];
        const names = matches
            .map((match) => match.replace('(', '').trim())
            .filter((name) => !['if', 'for', 'while', 'switch', 'return', 'sizeof'].includes(name));
        return Array.from(new Set(names)).slice(0, 3);
    }, [selectedContent]);
    const includeCount = useMemo(
        () => (selectedContent.match(/^\s*#include\b/gm) ?? []).length,
        [selectedContent],
    );
    const includeTargets = useMemo(() => {
        const matches = selectedContent.match(/^\s*#include\s+[<"].+[>"]/gm) ?? [];
        return matches.slice(0, 3).map((entry) => entry.replace(/^\s*#include\s+/, ''));
    }, [selectedContent]);
    const primaryDependencyLabel = useMemo(
        () => includeTargets[0] ? normalizeDependencyLabel(includeTargets[0]) : 'No dependency route detected',
        [includeTargets],
    );
    const ifCount = useMemo(
        () => (selectedContent.match(/\bif\s*\(/g) ?? []).length,
        [selectedContent],
    );
    const loopCount = useMemo(
        () => (selectedContent.match(/\b(for|while)\s*\(/g) ?? []).length,
        [selectedContent],
    );
    const pointerCount = useMemo(
        () => (selectedContent.match(/\*/g) ?? []).length,
        [selectedContent],
    );
    const callDensity = useMemo(() => {
        if (lineCount <= 0) {
            return 0;
        }
        return Math.round((functionCount / lineCount) * 100);
    }, [functionCount, lineCount]);
    const branchPressure = useMemo(
        () => Math.min(100, ifCount * 6 + loopCount * 9),
        [ifCount, loopCount],
    );
    const memoryRisk = useMemo(
        () => Math.min(100, pointerCount),
        [pointerCount],
    );
    const dependencyPulse = useMemo(
        () => Math.min(100, includeCount * 14),
        [includeCount],
    );
    const hotspotScore = useMemo(
        () => Math.min(100, Math.round((branchPressure * 0.45) + (memoryRisk * 0.35) + (dependencyPulse * 0.2))),
        [branchPressure, dependencyPulse, memoryRisk],
    );
    const riskLabel = useMemo(() => {
        if (hotspotScore >= 75) {
            return 'Critical';
        }
        if (hotspotScore >= 50) {
            return 'Elevated';
        }
        if (hotspotScore >= 25) {
            return 'Guarded';
        }
        return 'Stable';
    }, [hotspotScore]);
    const activeTheme = useMemo(
        () => domainTheme[selectedDomain] ?? domainTheme.other,
        [selectedDomain],
    );
    const completedQuestCount = useMemo(
        () => questProgress.filter((quest) => quest.completed).length,
        [questProgress],
    );
    const questPercent = useMemo(() => {
        if (questProgress.length === 0) {
            return 0;
        }
        return Math.round((completedQuestCount / questProgress.length) * 100);
    }, [completedQuestCount, questProgress.length]);
    const missionSector = useMemo(
        () => deriveSectorMissionMeta(selectedFile),
        [selectedFile],
    );
    const intelEvents = useMemo<IntelEvent[]>(() => {
        const events: IntelEvent[] = [];

        events.push({
            id: 'threat',
            label: branchPressure >= 70 ? 'Threat Surge' : branchPressure >= 40 ? 'Tactical Tension' : 'Stable Frontline',
            severity: branchPressure >= 70 ? 'high' : branchPressure >= 40 ? 'medium' : 'low',
            detail: `Branch pressure ${branchPressure}% with ${ifCount} branch gates and ${loopCount} loop engines detected.`,
        });

        events.push({
            id: 'memory',
            label: memoryRisk >= 60 ? 'Memory Hazard' : memoryRisk >= 30 ? 'Pointer Watch' : 'Memory Secure',
            severity: memoryRisk >= 60 ? 'high' : memoryRisk >= 30 ? 'medium' : 'low',
            detail: `${pointerCount} pointer vectors mapped across this sector lane.`,
        });

        events.push({
            id: 'dependency',
            label: dependencyPulse >= 60 ? 'Dependency Spike' : dependencyPulse >= 30 ? 'Dependency Drift' : 'Dependency Calm',
            severity: dependencyPulse >= 60 ? 'high' : dependencyPulse >= 30 ? 'medium' : 'low',
            detail: includeTargets[0]
                ? `Primary route anchored to ${primaryDependencyLabel} (${includeCount} dependency channels).`
                : `No include route detected in this sector snapshot.`,
        });

        events.push({
            id: 'signal',
            label: callDensity >= 22 ? 'High Signal Traffic' : callDensity >= 10 ? 'Moderate Signal Traffic' : 'Low Signal Traffic',
            severity: callDensity >= 22 ? 'high' : callDensity >= 10 ? 'medium' : 'low',
            detail: functionNames[0]
                ? `Lead signature ${functionNames[0]} with ${functionCount} callable nodes.`
                : `No stable function signature detected yet.`,
        });

        return events;
    }, [
        branchPressure,
        callDensity,
        functionCount,
        functionNames,
        ifCount,
        includeCount,
        includeTargets,
        loopCount,
        memoryRisk,
        primaryDependencyLabel,
        pointerCount,
    ]);
    const sourcePreviewWithLineNumbers = useMemo(
        () => selectedLines.slice(0, 80).map((line, index) => `${String(index + 1).padStart(3, '0')} | ${line}`).join('\n'),
        [selectedLines],
    );

    useEffect(() => {
        setIntelMode('summary');
        setActiveDock(selectedFile ? 'intel' : 'objectives');
        setShowFullSourceDump(false);
    }, [selectedFile]);

    useEffect(() => {
        if (typeof window === 'undefined') {
            return;
        }

        const stored = window.localStorage.getItem('banana.notebooks.devIntelUnlocked');
        setDeveloperIntelUnlocked(stored === '1');
    }, []);

    useEffect(() => {
        if (!developerIntelUnlocked && intelMode === 'raw') {
            setIntelMode('summary');
        }
    }, [developerIntelUnlocked, intelMode]);

    useEffect(() => {
        if (intelMode !== 'raw') {
            setShowFullSourceDump(false);
        }
    }, [intelMode]);

    const toggleDeveloperUnlock = () => {
        setDeveloperIntelUnlocked((previous) => {
            const next = !previous;
            if (typeof window !== 'undefined') {
                window.localStorage.setItem(
                    'banana.notebooks.devIntelUnlocked',
                    next ? '1' : '0',
                );
            }
            return next;
        });
    };

    if (fullBleed) {
        return (
            <div style={{
                position: 'absolute',
                inset: 0,
                borderRadius: 18,
                border: '1px solid rgba(45, 212, 191, 0.22)',
                background: `${scanlineUri}, radial-gradient(ellipse at 12% 8%, ${activeTheme.aura}, transparent 38%), radial-gradient(ellipse at 88% 92%, rgba(14, 165, 233, 0.12), transparent 40%), linear-gradient(160deg, #020c18 0%, #071525 55%, #050f1e 100%)`,
                overflow: 'hidden',
                zIndex: 1,
            }}>
                {questToast.length > 0 ? (
                    <div style={{
                        position: 'absolute',
                        top: 16,
                        left: 16,
                        zIndex: 5,
                        borderRadius: 12,
                        border: '1px solid rgba(251, 191, 36, 0.6)',
                        background: 'linear-gradient(135deg, rgba(120, 53, 15, 0.9), rgba(146, 64, 14, 0.82))',
                        color: '#fef3c7',
                        padding: '10px 12px',
                        fontSize: 12,
                        letterSpacing: '0.05em',
                        textTransform: 'uppercase',
                        fontWeight: 700,
                        boxShadow: '0 14px 28px rgba(120, 53, 15, 0.38)',
                    }}>
                        {questToast}
                    </div>
                ) : null}

                <div style={{
                    position: 'absolute',
                    top: 14,
                    left: '50%',
                    transform: 'translateX(-50%)',
                    zIndex: 3,
                    fontSize: 11,
                    color: 'rgba(103, 232, 249, 0.78)',
                    textTransform: 'uppercase',
                    letterSpacing: '0.12em',
                    pointerEvents: 'none',
                    whiteSpace: 'nowrap',
                }}>
                    {selectedFile ? `Sector Live Feed - ${missionSector.title} (${missionSector.code})` : 'Mission Staging Bay'}
                </div>

                <div style={{
                    position: 'absolute',
                    inset: '54px 14px 130px',
                    borderRadius: 16,
                    border: '1px solid rgba(45, 212, 191, 0.18)',
                    background: 'linear-gradient(180deg, rgba(4, 17, 33, 0.58), rgba(3, 11, 25, 0.52))',
                    overflow: 'hidden',
                    zIndex: 1,
                }}>
                    <div style={{
                        position: 'absolute',
                        inset: 0,
                        backgroundImage: 'linear-gradient(rgba(148, 163, 184, 0.08) 1px, transparent 1px), linear-gradient(90deg, rgba(148, 163, 184, 0.08) 1px, transparent 1px)',
                        backgroundSize: '44px 44px',
                        opacity: 0.28,
                    }} />
                    <div style={{
                        position: 'absolute',
                        left: '50%',
                        top: '46%',
                        transform: 'translate(-50%, -50%)',
                        width: 'min(52vw, 640px)',
                        height: 'min(52vw, 640px)',
                        borderRadius: '50%',
                        border: `1px solid ${activeTheme.accent}55`,
                        background: `radial-gradient(circle at 50% 50%, ${activeTheme.aura}, rgba(2, 6, 23, 0.14) 52%, rgba(2, 6, 23, 0.04) 72%)`,
                        boxShadow: `0 0 42px ${activeTheme.aura}`,
                    }} />
                    <svg
                        aria-hidden="true"
                        viewBox="0 0 100 100"
                        preserveAspectRatio="none"
                        style={{ position: 'absolute', inset: 0, width: '100%', height: '100%', opacity: 0.6 }}
                    >
                        <polyline points="0,65 16,60 33,63 49,52 65,58 82,47 100,54" fill="none" stroke="rgba(34,211,238,0.48)" strokeWidth="1.2" />
                        <polyline points="0,74 16,71 33,75 49,66 65,69 82,62 100,66" fill="none" stroke="rgba(94,234,212,0.38)" strokeWidth="1" />
                    </svg>

                    {selectedFile ? (
                        <div style={{
                            position: 'absolute',
                            top: 14,
                            left: 14,
                            borderRadius: 10,
                            border: '1px solid rgba(148, 163, 184, 0.3)',
                            background: 'rgba(2, 8, 18, 0.58)',
                            padding: '8px 10px',
                            maxWidth: 'min(66ch, calc(100% - 28px))',
                        }}>
                            <div style={{
                                fontSize: 11,
                                letterSpacing: '0.08em',
                                textTransform: 'uppercase',
                                color: activeTheme.accent,
                                fontWeight: 700,
                            }}>
                                Gameplay Camera Ready · {missionSector.domain}
                            </div>
                            <div style={{ marginTop: 5, fontSize: 13, color: '#cbd5e1', lineHeight: 1.45 }}>
                                Center viewport reserved for world visualizations in {missionSector.title}. Expand bottom dock menus for intel, objectives, and player stats.
                            </div>
                        </div>
                    ) : (
                        <div style={{
                            position: 'absolute',
                            top: '50%',
                            left: '50%',
                            transform: 'translate(-50%, -50%)',
                            textAlign: 'center',
                            width: 'min(680px, calc(100% - 28px))',
                        }}>
                            <div style={{
                                fontSize: 12,
                                textTransform: 'uppercase',
                                letterSpacing: '0.12em',
                                color: '#67e8f9',
                                fontWeight: 700,
                            }}>
                                Mission Briefing
                            </div>
                            <h2 style={{
                                margin: '10px 0 8px',
                                fontSize: 'clamp(1.35rem, 3vw, 1.95rem)',
                                color: '#ecfeff',
                                lineHeight: 1.16,
                                fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
                            }}>
                                Select a sector to populate the world scene
                            </h2>
                            <p style={{ margin: 0, color: '#bae6fd', lineHeight: 1.55 }}>
                                The bottom command dock carries operational cards so this central space can stay open for live gameplay visualizations.
                            </p>
                        </div>
                    )}
                </div>

                <div style={{
                    position: 'absolute',
                    left: '50%',
                    bottom: 12,
                    transform: 'translateX(-50%)',
                    width: 'min(980px, calc(100% - 20px))',
                    zIndex: 4,
                    display: 'grid',
                    gap: 8,
                }}>
                    {activeDock ? (
                        <div style={{
                            borderRadius: 14,
                            border: '1px solid rgba(148, 163, 184, 0.32)',
                            background: 'linear-gradient(180deg, rgba(4, 12, 24, 0.9), rgba(2, 7, 17, 0.9))',
                            boxShadow: '0 16px 28px rgba(2, 6, 23, 0.42)',
                            padding: '10px 12px',
                            maxHeight: '38dvh',
                            overflow: 'auto',
                        }}>
                            {activeDock === 'intel' ? (
                                <div style={{ display: 'grid', gap: 10 }}>
                                    <div style={{ display: 'flex', justifyContent: 'space-between', gap: 10, alignItems: 'center', flexWrap: 'wrap' }}>
                                        <span style={{ fontSize: 11, letterSpacing: '0.08em', textTransform: 'uppercase', color: '#93c5fd' }}>
                                            Intel Dock
                                        </span>
                                        <span style={{ fontSize: 11, color: '#cbd5e1' }}>
                                            {missionSector.title} · {missionSector.code} · {lineCount} lines · {functionCount} signals
                                        </span>
                                    </div>
                                    <div style={{ display: 'flex', gap: 8, flexWrap: 'wrap' }}>
                                        <button
                                            type="button"
                                            onClick={() => setIntelMode('summary')}
                                            style={{
                                                borderRadius: 999,
                                                border: '1px solid rgba(125, 211, 252, 0.45)',
                                                background: intelMode === 'summary' ? 'rgba(8, 47, 73, 0.9)' : 'rgba(8, 47, 73, 0.45)',
                                                color: '#e0f2fe',
                                                padding: '4px 10px',
                                                fontSize: 11,
                                                fontWeight: 700,
                                                letterSpacing: '0.04em',
                                                cursor: 'pointer',
                                            }}
                                        >
                                            Tactical
                                        </button>
                                        <button
                                            type="button"
                                            onClick={() => setIntelMode('raw')}
                                            disabled={!developerIntelUnlocked}
                                            style={{
                                                borderRadius: 999,
                                                border: '1px solid rgba(45, 212, 191, 0.45)',
                                                background: !developerIntelUnlocked ? 'rgba(15, 23, 42, 0.65)' : (intelMode === 'raw' ? 'rgba(15, 118, 110, 0.92)' : 'rgba(15, 118, 110, 0.45)'),
                                                color: !developerIntelUnlocked ? '#94a3b8' : '#ecfeff',
                                                padding: '4px 10px',
                                                fontSize: 11,
                                                fontWeight: 700,
                                                letterSpacing: '0.04em',
                                                cursor: developerIntelUnlocked ? 'pointer' : 'not-allowed',
                                                opacity: developerIntelUnlocked ? 1 : 0.9,
                                            }}
                                        >
                                            {developerIntelUnlocked ? 'Raw Intel' : 'Raw Intel (Locked)'}
                                        </button>
                                        <button
                                            type="button"
                                            onClick={toggleDeveloperUnlock}
                                            style={{
                                                borderRadius: 999,
                                                border: '1px solid rgba(148, 163, 184, 0.45)',
                                                background: developerIntelUnlocked ? 'rgba(30, 41, 59, 0.92)' : 'rgba(15, 23, 42, 0.68)',
                                                color: developerIntelUnlocked ? '#86efac' : '#cbd5e1',
                                                padding: '4px 10px',
                                                fontSize: 11,
                                                fontWeight: 700,
                                                letterSpacing: '0.04em',
                                                cursor: 'pointer',
                                            }}
                                        >
                                            {developerIntelUnlocked ? 'Developer Unlock: ON' : 'Developer Unlock'}
                                        </button>
                                    </div>

                                    {intelMode === 'summary' ? (
                                        <div style={{
                                            display: 'grid',
                                            gridTemplateColumns: 'repeat(auto-fit, minmax(160px, 1fr))',
                                            gap: 8,
                                        }}>
                                            <div style={{ borderRadius: 10, border: '1px solid rgba(244, 114, 182, 0.28)', background: 'rgba(131, 24, 67, 0.22)', padding: 9 }}>
                                                <div style={{ fontSize: 10, color: '#f9a8d4', textTransform: 'uppercase', letterSpacing: '0.08em' }}>Hotspot</div>
                                                <div style={{ marginTop: 5, display: 'flex', justifyContent: 'space-between', alignItems: 'baseline' }}>
                                                    <span style={{ color: '#fce7f3', fontWeight: 800, fontSize: 20 }}>{hotspotScore}</span>
                                                    <span style={{ color: '#fbcfe8', fontSize: 11, fontWeight: 700 }}>{riskLabel}</span>
                                                </div>
                                            </div>
                                            <div style={{ borderRadius: 10, border: '1px solid rgba(125, 211, 252, 0.28)', background: 'rgba(8, 47, 73, 0.24)', padding: 9 }}>
                                                <div style={{ fontSize: 10, color: '#7dd3fc', textTransform: 'uppercase', letterSpacing: '0.08em' }}>Branch Pressure</div>
                                                <div style={{ marginTop: 5, color: '#e0f2fe', fontWeight: 700 }}>{branchPressure}%</div>
                                            </div>
                                            <div style={{ borderRadius: 10, border: '1px solid rgba(251, 191, 36, 0.28)', background: 'rgba(120, 53, 15, 0.2)', padding: 9 }}>
                                                <div style={{ fontSize: 10, color: '#fcd34d', textTransform: 'uppercase', letterSpacing: '0.08em' }}>Dependency Pulse</div>
                                                <div style={{ marginTop: 5, color: '#fef3c7', fontWeight: 700 }}>{dependencyPulse}%</div>
                                            </div>
                                            <div style={{ borderRadius: 10, border: '1px solid rgba(167, 139, 250, 0.28)', background: 'rgba(49, 46, 129, 0.22)', padding: 9 }}>
                                                <div style={{ fontSize: 10, color: '#c4b5fd', textTransform: 'uppercase', letterSpacing: '0.08em' }}>Signal Density</div>
                                                <div style={{ marginTop: 5, color: '#ede9fe', fontWeight: 700 }}>{callDensity}%</div>
                                            </div>
                                            <div style={{ borderRadius: 10, border: '1px solid rgba(74, 222, 128, 0.28)', background: 'rgba(20, 83, 45, 0.28)', padding: 9 }}>
                                                <div style={{ fontSize: 10, color: '#86efac', textTransform: 'uppercase', letterSpacing: '0.08em' }}>Prime Function</div>
                                                <div style={{ marginTop: 5, color: '#dcfce7', fontWeight: 700 }}>
                                                    {functionNames[0] ?? 'No function signature detected'}
                                                </div>
                                            </div>
                                            <div style={{ borderRadius: 10, border: '1px solid rgba(103, 232, 249, 0.3)', background: 'rgba(8, 47, 73, 0.32)', padding: 9 }}>
                                                <div style={{ fontSize: 10, color: '#67e8f9', textTransform: 'uppercase', letterSpacing: '0.08em' }}>Dependency Route</div>
                                                <div style={{ marginTop: 5, color: '#cffafe', fontWeight: 700 }}>
                                                    {primaryDependencyLabel}
                                                </div>
                                            </div>
                                        </div>
                                    ) : (
                                        <div style={{ display: 'grid', gap: 10 }}>
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
                                                    Raw mode is intentionally secondary to keep gameplay intel readable.
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
                                                    overflow: 'auto',
                                                    whiteSpace: 'pre',
                                                    overflowWrap: 'anywhere',
                                                    wordBreak: 'break-word',
                                                    padding: 10,
                                                    maxHeight: '24dvh',
                                                }}>
                                                    {sourcePreviewWithLineNumbers}
                                                </div>
                                            ) : null}
                                        </div>
                                    )}
                                </div>
                            ) : null}

                            {activeDock === 'objectives' ? (
                                <div style={{ display: 'grid', gap: 10 }}>
                                    <div style={{ display: 'flex', justifyContent: 'space-between', gap: 10, alignItems: 'center', flexWrap: 'wrap' }}>
                                        <span style={{ fontSize: 11, letterSpacing: '0.08em', textTransform: 'uppercase', color: '#93c5fd' }}>
                                            Mission Objectives
                                        </span>
                                        <span style={{ fontSize: 11, color: '#e2e8f0', fontWeight: 700 }}>
                                            {completedQuestCount}/{questProgress.length} complete · {questPercent}%
                                        </span>
                                    </div>
                                    <div style={{ height: 8, borderRadius: 999, background: 'rgba(30, 41, 59, 0.76)', overflow: 'hidden' }}>
                                        <div style={{ width: `${questPercent}%`, height: '100%', background: 'linear-gradient(90deg, #38bdf8, #34d399, #f59e0b)' }} />
                                    </div>
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
                                    <div style={{ fontSize: 12, color: '#cbd5e1' }}>
                                        Next hunt: <strong style={{ color: '#e9d5ff', textTransform: 'uppercase' }}>capture {nextDomainTarget} sector</strong>
                                    </div>
                                </div>
                            ) : null}

                            {activeDock === 'player' ? (
                                <div style={{
                                    display: 'grid',
                                    gridTemplateColumns: 'repeat(auto-fit, minmax(170px, 1fr))',
                                    gap: 8,
                                }}>
                                    <div style={{ borderRadius: 10, border: '1px solid rgba(167, 139, 250, 0.3)', background: 'rgba(76, 29, 149, 0.24)', padding: 10 }}>
                                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#c4b5fd' }}>Commander Level</div>
                                        <div style={{ marginTop: 6, fontSize: 22, fontWeight: 700, color: '#ede9fe' }}>{playerLevel}</div>
                                    </div>
                                    <div style={{ borderRadius: 10, border: '1px solid rgba(34, 211, 238, 0.3)', background: 'rgba(8, 47, 73, 0.34)', padding: 10 }}>
                                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#67e8f9' }}>Experience</div>
                                        <div style={{ marginTop: 6, fontSize: 22, fontWeight: 700, color: '#cffafe' }}>{totalXp}</div>
                                    </div>
                                    <div style={{ borderRadius: 10, border: '1px solid rgba(244, 114, 182, 0.3)', background: 'rgba(131, 24, 67, 0.24)', padding: 10 }}>
                                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#f9a8d4' }}>Combo Chain</div>
                                        <div style={{ marginTop: 6, fontSize: 22, fontWeight: 700, color: '#fbcfe8' }}>x{Math.max(1, comboStreak)}</div>
                                    </div>
                                    <div style={{ borderRadius: 10, border: '1px solid rgba(251, 191, 36, 0.3)', background: 'rgba(120, 53, 15, 0.3)', padding: 10 }}>
                                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#fcd34d' }}>Last Reward</div>
                                        <div style={{ marginTop: 6, fontSize: 22, fontWeight: 700, color: '#fde68a' }}>+{lastXpGain} XP</div>
                                    </div>
                                    <div style={{ borderRadius: 10, border: '1px solid rgba(148, 163, 184, 0.32)', background: 'rgba(15, 23, 42, 0.52)', padding: 10 }}>
                                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#7dd3fc' }}>Rarity</div>
                                        <div style={{ marginTop: 6, fontSize: 16, fontWeight: 700, color: rarityAccent, textTransform: 'uppercase' }}>{rarityLabel}</div>
                                    </div>
                                </div>
                            ) : null}
                        </div>
                    ) : null}

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
                </div>
                {/* Corner vignette borders */}
                <div style={{ position: 'absolute', inset: 0, borderRadius: 18, boxShadow: 'inset 0 0 60px rgba(2, 6, 23, 0.55)', pointerEvents: 'none', zIndex: 2 }} />
            </div>
        );
    }

    return (
        <SurfaceCard
            title="Mission Viewport"
            description="Source-driven runtime feed for the current sector."
        >
            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8 }}>
                <Pill color="#5eead4" borderColor="rgba(45, 212, 191, 0.4)">Sector: {selectedFile || 'none'}</Pill>
                <Pill color="#a5b4fc" borderColor="rgba(148, 163, 184, 0.35)">Lines: {lineCount}</Pill>
                <Pill color="#86efac" borderColor="rgba(34, 197, 94, 0.35)">Renderer: notebook</Pill>
                <Pill color={rarityAccent} borderColor="rgba(148, 163, 184, 0.35)">Rarity: {rarityLabel}</Pill>
                <Pill color="#fcd34d" borderColor="rgba(251, 191, 36, 0.35)">XP +{lastXpGain}</Pill>
            </div>
            <div style={{ ...codeSurfaceStyle, marginTop: 10 }}>
                {selectedContent}
            </div>
        </SurfaceCard>
    );
}
