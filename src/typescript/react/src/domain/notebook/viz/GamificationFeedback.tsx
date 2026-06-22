import { useMemo } from 'react';
import type { InteractionMetrics, TrackedInteraction } from './useInteractionTracking';

type GamificationFeedbackProps = {
    readonly metrics: InteractionMetrics;
    readonly recentInteractions: readonly TrackedInteraction[];
    readonly compact?: boolean;
};

/**
 * Displays gamification feedback: XP, level progression, achievements,
 * and visual confirmation that interactions are being tracked and persisted.
 */
export function GamificationFeedback({
    metrics,
    recentInteractions,
    compact = false,
}: GamificationFeedbackProps) {
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

    // Determine level-up achievement message
    const levelUpMessage = useMemo(() => {
        const messages: Record<number, string> = {
            1: '🎮 Apprentice',
            2: '⭐ Explorer',
            5: '🔥 Practitioner',
            10: '💎 Master',
            20: '👑 Architect',
            50: '🚀 Visionary',
        };
        return messages[metrics.level] || `Level ${metrics.level}`;
    }, [metrics.level]);

    // Recent interaction feedback animations
    const interactionFeedback = useMemo(() => {
        return recentInteractions.slice(-3).map((int) => {
            const xpGain = {
                bootstrap: 50,
                relations: 75,
                'expand-details': 10,
                'collapse-details': 5,
                'workflow-step': 25,
                'model-query': 100,
            }[int.kind] + Math.floor(int.impactScore * 50);

            const labels: Record<string, string> = {
                bootstrap: '▶ Bootstrap',
                relations: '⟲ Relations',
                'expand-details': '▼ Expand',
                'collapse-details': '▲ Collapse',
                'workflow-step': '→ Step',
                'model-query': '🔍 Query',
            };

            return { label: labels[int.kind] as string, xp: xpGain };
        });
    }, [recentInteractions]);

    if (compact) {
        // Minimal inline display
        return (
            <div
                style={{
                    display: 'flex',
                    alignItems: 'center',
                    gap: 8,
                    fontSize: 11,
                    color: '#cbd5e1',
                }}
            >
                <div style={{ display: 'flex', alignItems: 'center', gap: 4 }}>
                    <span style={{ fontSize: 9, color: '#7dd3fc' }}>⭐</span>
                    <span>{metrics.level}</span>
                </div>
                <div
                    style={{
                        display: 'flex',
                        alignItems: 'center',
                        gap: 3,
                        flex: 1,
                        maxWidth: 80,
                    }}
                >
                    <div
                        style={{
                            flex: 1,
                            height: 4,
                            borderRadius: 2,
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
                                transition: 'width 0.3s ease-out',
                            }}
                        />
                    </div>
                    <span style={{ fontSize: 8, color: '#94a3b8', minWidth: 16 }}>
                        {xpProgression.progressPercent}%
                    </span>
                </div>
            </div>
        );
    }

    // Full gamification dashboard
    return (
        <div
            style={{
                display: 'grid',
                gap: 8,
                padding: 8,
                borderRadius: 8,
                border: '1px solid rgba(125, 211, 252, 0.2)',
                background: 'rgba(15, 23, 42, 0.5)',
            }}
        >
            {/* Level Display */}
            <div
                style={{
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'space-between',
                }}
            >
                <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
                    <div
                        style={{
                            width: 32,
                            height: 32,
                            borderRadius: '50%',
                            background: `conic-gradient(#7dd3fc ${metrics.level * 3.6}deg, rgba(125, 211, 252, 0.1) 0deg)`,
                            display: 'flex',
                            alignItems: 'center',
                            justifyContent: 'center',
                            fontSize: 14,
                            fontWeight: 'bold',
                            color: '#0f172a',
                        }}
                    >
                        {metrics.level}
                    </div>
                    <div>
                        <div
                            style={{
                                fontSize: 12,
                                fontWeight: 600,
                                color: '#e0f2fe',
                            }}
                        >
                            {levelUpMessage}
                        </div>
                        <div
                            style={{
                                fontSize: 10,
                                color: '#64748b',
                            }}
                        >
                            {metrics.totalInteractions} total interactions
                        </div>
                    </div>
                </div>
                <div
                    style={{
                        textAlign: 'right',
                        fontSize: 12,
                        color: '#cbd5e1',
                    }}
                >
                    <div style={{ fontWeight: 600, color: '#fbbf24' }}>
                        +{metrics.xpEarned} XP
                    </div>
                    <div style={{ fontSize: 10, color: '#94a3b8' }}>
                        {xpProgression.xpIntoCurrentLevel}/{xpProgression.xpToNextLevel}
                    </div>
                </div>
            </div>

            {/* XP Progress Bar */}
            <div
                style={{
                    display: 'grid',
                    gap: 4,
                }}
            >
                <div
                    style={{
                        height: 8,
                        borderRadius: 4,
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
                <div
                    style={{
                        fontSize: 9,
                        color: '#94a3b8',
                        display: 'flex',
                        justifyContent: 'space-between',
                    }}
                >
                    <span>Progress to Level {metrics.level + 1}</span>
                    <span>{xpProgression.progressPercent}%</span>
                </div>
            </div>

            {/* Interaction Stats */}
            <div
                style={{
                    display: 'grid',
                    gridTemplateColumns: '1fr 1fr',
                    gap: 6,
                    fontSize: 10,
                }}
            >
                <div
                    style={{
                        padding: 6,
                        borderRadius: 4,
                        background: 'rgba(34, 197, 94, 0.1)',
                        border: '1px solid rgba(34, 197, 94, 0.2)',
                        color: '#22c55e',
                    }}
                >
                    <div style={{ fontWeight: 600 }}>{metrics.bootstrapCount}</div>
                    <div style={{ fontSize: 9, color: '#86efac' }}>Bootstrap</div>
                </div>
                <div
                    style={{
                        padding: 6,
                        borderRadius: 4,
                        background: 'rgba(59, 130, 246, 0.1)',
                        border: '1px solid rgba(59, 130, 246, 0.2)',
                        color: '#3b82f6',
                    }}
                >
                    <div style={{ fontWeight: 600 }}>{metrics.relationsCount}</div>
                    <div style={{ fontSize: 9, color: '#93c5fd' }}>Relations</div>
                </div>
            </div>

            {/* Recent Interaction Feedback */}
            {interactionFeedback.length > 0 ? (
                <div
                    style={{
                        display: 'grid',
                        gap: 3,
                        paddingTop: 6,
                        borderTop: '1px solid rgba(148, 163, 184, 0.2)',
                    }}
                >
                    <div
                        style={{
                            fontSize: 9,
                            fontWeight: 600,
                            color: '#94a3b8',
                            textTransform: 'uppercase',
                            letterSpacing: '0.04em',
                        }}
                    >
                        Recent Actions
                    </div>
                    {interactionFeedback.map((feedback, idx) => (
                        <div
                            key={idx}
                            style={{
                                display: 'flex',
                                justifyContent: 'space-between',
                                alignItems: 'center',
                                fontSize: 10,
                                color: '#cbd5e1',
                                padding: 4,
                                borderRadius: 3,
                                background: 'rgba(125, 211, 252, 0.05)',
                                animation: `slideInRight 0.4s ease-out ${idx * 0.1}s`,
                            }}
                        >
                            <span>{feedback.label}</span>
                            <span
                                style={{
                                    fontWeight: 600,
                                    color: '#fbbf24',
                                }}
                            >
                                +{feedback.xp} XP
                            </span>
                        </div>
                    ))}
                </div>
            ) : null}

            {/* Interaction Streak */}
            {metrics.interactionStreak > 1 ? (
                <div
                    style={{
                        padding: 6,
                        borderRadius: 4,
                        background: 'rgba(249, 115, 22, 0.1)',
                        border: '1px solid rgba(249, 115, 22, 0.3)',
                        fontSize: 10,
                        color: '#fb923c',
                        textAlign: 'center',
                    }}
                >
                    🔥 {metrics.interactionStreak} interaction streak!
                </div>
            ) : null}

            <style>{`
        @keyframes slideInRight {
          from {
            opacity: 0;
            transform: translateX(-10px);
          }
          to {
            opacity: 1;
            transform: translateX(0);
          }
        }
      `}</style>
        </div>
    );
}
