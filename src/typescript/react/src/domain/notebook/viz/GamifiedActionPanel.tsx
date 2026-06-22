import { useMemo } from 'react';
import type { InteractionMetrics, TrackedInteraction } from './useInteractionTracking';

type GamifiedActionPanelProps = {
    readonly metrics: InteractionMetrics;
    readonly recentInteractions: readonly TrackedInteraction[];
    readonly isBootstrapActive: boolean;
    readonly isRelationsDisabled: boolean;
    readonly onBootstrap: () => void;
    readonly onRelations: () => void;
    readonly mobile: boolean;
    readonly overlayTop: number;
    readonly mobileDrawerOpen?: boolean;
};

/**
 * Unified gamified action panel combining metrics, buttons, and feedback
 * into a single integrated interaction point. Reduces visual clutter while
 * providing immediate feedback for user actions.
 */
export function GamifiedActionPanel({
    metrics,
    recentInteractions,
    isBootstrapActive,
    isRelationsDisabled,
    onBootstrap,
    onRelations,
    mobile,
    overlayTop,
    mobileDrawerOpen = false,
}: GamifiedActionPanelProps) {
    // Calculate XP progress to next level
    const xpProgression = useMemo(() => {
        const xpPerLevelFn = (lv: number) => 100 + lv * 50;
        const currentLevelXp = Array.from({ length: metrics.level - 1 }, (_, i) => xpPerLevelFn(i + 1)).reduce((a, b) => a + b, 0);
        const nextLevelXp = xpPerLevelFn(metrics.level);
        const xpIntoCurrentLevel = metrics.xpEarned - currentLevelXp;
        const xpToNextLevel = nextLevelXp;
        const progressPercent = Math.round((xpIntoCurrentLevel / xpToNextLevel) * 100);

        return { xpIntoCurrentLevel, xpToNextLevel, progressPercent };
    }, [metrics.level, metrics.xpEarned]);

    // Get level achievement emoji
    const levelEmoji = useMemo(() => {
        const emojis: Record<number, string> = {
            1: '🎮', 2: '⭐', 5: '🔥', 10: '💎', 20: '👑', 50: '🚀',
        };
        return emojis[metrics.level] || '✨';
    }, [metrics.level]);

    // Most recent interaction for animation
    const lastInteraction = recentInteractions.length > 0 ? recentInteractions[recentInteractions.length - 1] : null;

    if (mobile) {
        // Mobile: Compact stacked layout with metrics inline
        return (
            <div
                style={{
                    position: 'absolute',
                    right: 8,
                    bottom: mobileDrawerOpen ? 188 : 92,
                    display: 'flex',
                    flexDirection: 'column',
                    gap: 6,
                    pointerEvents: 'auto',
                    zIndex: 5,
                }}
            >
                {/* Level & XP mini badge */}
                <div
                    style={{
                        display: 'flex',
                        alignItems: 'center',
                        gap: 4,
                        fontSize: 10,
                        color: '#cbd5e1',
                        background: 'rgba(15, 23, 42, 0.7)',
                        border: '1px solid rgba(125, 211, 252, 0.2)',
                        borderRadius: 6,
                        padding: '4px 6px',
                        minWidth: 'max-content',
                    }}
                >
                    <span style={{ fontSize: 12 }}>{levelEmoji}</span>
                    <span style={{ fontWeight: 600, color: '#7dd3fc' }}>{metrics.level}</span>
                    <div
                        style={{
                            width: 20,
                            height: 3,
                            borderRadius: 2,
                            background: 'rgba(125, 211, 252, 0.1)',
                            overflow: 'hidden',
                        }}
                    >
                        <div
                            style={{
                                width: `${xpProgression.progressPercent}%`,
                                height: '100%',
                                background: '#7dd3fc',
                                transition: 'width 0.3s ease-out',
                            }}
                        />
                    </div>
                </div>

                {/* Action buttons */}
                <button
                    type="button"
                    onClick={onBootstrap}
                    disabled={isBootstrapActive}
                    style={{
                        width: 40,
                        height: 40,
                        borderRadius: 999,
                        border: '1px solid rgba(45, 212, 191, 0.5)',
                        background: 'rgba(2, 10, 20, 0.55)',
                        color: isBootstrapActive ? '#64748b' : '#99f6e4',
                        fontSize: 14,
                        fontWeight: 700,
                        cursor: isBootstrapActive ? 'not-allowed' : 'pointer',
                        opacity: isBootstrapActive ? 0.6 : 1,
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        transition: 'all 0.2s ease',
                    }}
                    title="Bootstrap training"
                    aria-label="Bootstrap"
                >
                    ▶
                </button>
                <button
                    type="button"
                    disabled={isRelationsDisabled}
                    onClick={onRelations}
                    style={{
                        width: 40,
                        height: 40,
                        borderRadius: 999,
                        border: '1px solid rgba(56, 189, 248, 0.5)',
                        background: 'rgba(2, 10, 20, 0.55)',
                        color: isRelationsDisabled ? '#64748b' : '#bae6fd',
                        fontSize: 14,
                        fontWeight: 700,
                        cursor: isRelationsDisabled ? 'not-allowed' : 'pointer',
                        opacity: isRelationsDisabled ? 0.5 : 1,
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        transition: 'all 0.2s ease',
                    }}
                    title="Relations analysis"
                    aria-label="Relations"
                >
                    ⟲
                </button>

                {/* Last interaction feedback */}
                {lastInteraction && (
                    <div
                        style={{
                            fontSize: 8,
                            color: '#fbbf24',
                            textAlign: 'center',
                            padding: '2px 4px',
                            borderRadius: 3,
                            background: 'rgba(251, 191, 36, 0.1)',
                            animation: 'pulse 0.6s ease-out',
                        }}
                    >
                        +{['bootstrap', 'relations'].includes(lastInteraction.kind) ? 50 : 10}
                    </div>
                )}
            </div>
        );
    }

    // Desktop: Larger vertical layout with full metrics display
    return (
        <div
            style={{
                position: 'absolute',
                right: 16,
                top: overlayTop,
                display: 'flex',
                flexDirection: 'column',
                gap: 8,
                pointerEvents: 'auto',
                zIndex: 5,
                width: 'auto',
                maxWidth: 160,
            }}
        >
            {/* Stats display */}
            <div
                style={{
                    background: 'rgba(15, 23, 42, 0.6)',
                    border: '1px solid rgba(125, 211, 252, 0.2)',
                    borderRadius: 8,
                    padding: 8,
                    display: 'grid',
                    gap: 6,
                }}
            >
                {/* Level with XP bar */}
                <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
                    <div
                        style={{
                            width: 32,
                            height: 32,
                            borderRadius: '50%',
                            background: `conic-gradient(#7dd3fc ${metrics.level * 3.6}deg, rgba(125, 211, 252, 0.1) 0deg)`,
                            display: 'flex',
                            alignItems: 'center',
                            justifyContent: 'center',
                            fontSize: 12,
                            fontWeight: 'bold',
                            color: '#0f172a',
                            flexShrink: 0,
                        }}
                    >
                        {metrics.level}
                    </div>
                    <div style={{ flex: 1, minWidth: 0 }}>
                        <div style={{ fontSize: 9, color: '#94a3b8', marginBottom: 2 }}>Level {metrics.level}</div>
                        <div
                            style={{
                                height: 6,
                                borderRadius: 3,
                                background: 'rgba(125, 211, 252, 0.1)',
                                border: '1px solid rgba(125, 211, 252, 0.2)',
                                overflow: 'hidden',
                            }}
                        >
                            <div
                                style={{
                                    width: `${xpProgression.progressPercent}%`,
                                    height: '100%',
                                    background: 'linear-gradient(90deg, #7dd3fc, #06b6d4)',
                                    transition: 'width 0.4s ease-out',
                                }}
                            />
                        </div>
                    </div>
                </div>

                {/* Quick stats */}
                <div
                    style={{
                        display: 'grid',
                        gridTemplateColumns: '1fr 1fr',
                        gap: 4,
                        fontSize: 9,
                    }}
                >
                    <div style={{ textAlign: 'center', color: '#22c55e' }}>
                        <div style={{ fontWeight: 600 }}>{metrics.bootstrapCount}</div>
                        <div style={{ color: '#86efac' }}>Bootstrap</div>
                    </div>
                    <div style={{ textAlign: 'center', color: '#3b82f6' }}>
                        <div style={{ fontWeight: 600 }}>{metrics.relationsCount}</div>
                        <div style={{ color: '#93c5fd' }}>Relations</div>
                    </div>
                </div>

                {/* Total XP */}
                <div
                    style={{
                        fontSize: 10,
                        color: '#fbbf24',
                        fontWeight: 600,
                        textAlign: 'center',
                    }}
                >
                    {metrics.xpEarned.toLocaleString()} XP
                </div>

                {/* Streak indicator */}
                {metrics.interactionStreak > 1 && (
                    <div
                        style={{
                            padding: 4,
                            borderRadius: 4,
                            background: 'rgba(249, 115, 22, 0.1)',
                            border: '1px solid rgba(249, 115, 22, 0.3)',
                            fontSize: 9,
                            color: '#fb923c',
                            textAlign: 'center',
                        }}
                    >
                        🔥 {metrics.interactionStreak}x
                    </div>
                )}
            </div>

            {/* Action buttons */}
            <div style={{ display: 'flex', flexDirection: 'column', gap: 6 }}>
                <button
                    type="button"
                    onClick={onBootstrap}
                    disabled={isBootstrapActive}
                    style={{
                        width: 34,
                        height: 34,
                        borderRadius: 999,
                        border: '1px solid rgba(45, 212, 191, 0.5)',
                        background: 'rgba(2, 10, 20, 0.55)',
                        color: isBootstrapActive ? '#64748b' : '#99f6e4',
                        fontSize: 12,
                        fontWeight: 700,
                        cursor: isBootstrapActive ? 'not-allowed' : 'pointer',
                        opacity: isBootstrapActive ? 0.6 : 1,
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        transition: 'all 0.2s ease',
                    }}
                    title="Bootstrap training"
                    aria-label="Bootstrap"
                >
                    ▶
                </button>
                <button
                    type="button"
                    disabled={isRelationsDisabled}
                    onClick={onRelations}
                    style={{
                        width: 34,
                        height: 34,
                        borderRadius: 999,
                        border: '1px solid rgba(56, 189, 248, 0.5)',
                        background: 'rgba(2, 10, 20, 0.55)',
                        color: isRelationsDisabled ? '#64748b' : '#bae6fd',
                        fontSize: 12,
                        fontWeight: 700,
                        cursor: isRelationsDisabled ? 'not-allowed' : 'pointer',
                        opacity: isRelationsDisabled ? 0.5 : 1,
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        transition: 'all 0.2s ease',
                    }}
                    title="Relations analysis"
                    aria-label="Relations"
                >
                    ⟲
                </button>
            </div>

            {/* Last action feedback */}
            {lastInteraction && (
                <div
                    style={{
                        fontSize: 8,
                        color: '#fbbf24',
                        textAlign: 'center',
                        padding: '3px 6px',
                        borderRadius: 4,
                        background: 'rgba(251, 191, 36, 0.1)',
                        border: '1px solid rgba(251, 191, 36, 0.2)',
                        animation: 'fadeInOut 1s ease-out',
                    }}
                >
                    +{['bootstrap', 'relations'].includes(lastInteraction.kind) ? 50 : 10} XP
                </div>
            )}

            <style>{`
        @keyframes pulse {
          0% { opacity: 1; transform: scale(1); }
          100% { opacity: 0; transform: scale(0.8); }
        }
        @keyframes fadeInOut {
          0%, 100% { opacity: 0.3; }
          50% { opacity: 1; }
        }
      `}</style>
        </div>
    );
}
