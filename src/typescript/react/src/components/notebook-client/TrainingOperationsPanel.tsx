import { readStoredAuthSession } from '@banana/ui';
import { useEffect, useMemo, useState, type CSSProperties } from 'react';

import {
    claimTrainingReward,
    executeTrainingJob,
    fetchApiHealth,
    fetchTrainingJobs,
    fetchTrainingLeaderboard,
    fetchTrainingRewards,
    recordTrainingTransitionEvent,
    resolveApiBaseUrl,
    scaffoldTrainingJobs,
    type TrainingJob,
    type TrainingLeaderboardEntry,
    type TrainingReward,
} from '../../lib/api';
import { SurfaceCard } from './SurfacePrimitives';

type TrainingOperationsPanelProps = {
    readonly selectedFile: string;
    readonly selectedContent: string;
    readonly indexedFileCount: number;
    readonly embedded?: boolean;
};

const buttonStyle: CSSProperties = {
    borderRadius: 10,
    border: '1px solid rgba(250, 204, 21, 0.4)',
    background: 'rgba(146, 64, 14, 0.4)',
    color: '#fef9c3',
    fontWeight: 700,
    padding: '8px 12px',
    cursor: 'pointer',
};

function buildBulkQueueTemplate(selectedFile: string):
    Array<{ title: string; sector: string; rewardXp: number; }> {
    const sector = selectedFile.length > 0 ? selectedFile : 'default-sector';
    return [
        {
            title: `Survey frontier output in ${sector}`,
            sector,
            rewardXp: 120,
        },
        {
            title: 'Allocate worker crews and supplies',
            sector,
            rewardXp: 180,
        },
        {
            title: 'Run production cycle for this province',
            sector,
            rewardXp: 260,
        },
        {
            title: 'Audit yields and issue stipend payout',
            sector,
            rewardXp: 140,
        },
    ];
}

function makeCorrelationId(prefix: string): string {
    if (typeof crypto !== 'undefined' && typeof crypto.randomUUID === 'function') {
        return `${prefix}-${crypto.randomUUID()}`;
    }
    return `${prefix}-${Date.now()}`;
}

const SERVER_REQUIRED_MESSAGE =
    'Server-authoritative mode required. Connect to the command relay and authenticate to continue.';

export function TrainingOperationsPanel(
    { selectedFile, selectedContent, indexedFileCount, embedded = false }: TrainingOperationsPanelProps) {
    const [queue, setQueue] = useState<TrainingJob[]>([]);
    const [leaderboard, setLeaderboard] = useState<TrainingLeaderboardEntry[]>([]);
    const [rewards, setRewards] = useState<TrainingReward[]>([]);
    const [isBusy, setIsBusy] = useState(false);
    const [isBackendReachable, setIsBackendReachable] = useState(false);
    const [statusMessage, setStatusMessage] = useState('Waiting for command relay authentication.');
    const [errorMessage, setErrorMessage] = useState<string | null>(null);

    const apiBaseUrl = useMemo(() => resolveApiBaseUrl(), []);
    const token = useMemo(() => readStoredAuthSession()?.token?.trim() ?? '', []);
    const canUseApi = apiBaseUrl.trim().length > 0 && token.length > 0;
    const isApiMode = canUseApi && isBackendReachable;

    const selectedLineCount =
        useMemo(() => selectedContent.split('\n').length, [selectedContent]);
    const trainingPower = useMemo(
        () => Math.max(1, Math.floor(selectedLineCount / 12)),
        [selectedLineCount]);
    const reputationTier = useMemo(() => {
        if (trainingPower >= 30) {
            return 'Legend';
        }
        if (trainingPower >= 18) {
            return 'Elite';
        }
        if (trainingPower >= 8) {
            return 'Rising';
        }
        return 'Rookie';
    }, [trainingPower]);

    const totalRewardXP =
        queue.reduce((sum, item) => sum + (item.status === 'completed' ? item.rewardXp : 0), 0);
    const completedCount = queue.filter((item) => item.status === 'completed').length;
    const pendingRewards = rewards.filter((reward) => reward.status === 'pending');

    useEffect(() => {
        if (!canUseApi) {
            setIsBackendReachable(false);
            setStatusMessage(SERVER_REQUIRED_MESSAGE);
            setErrorMessage('Missing API base URL or auth token for relay access.');
            setQueue([]);
            setLeaderboard([]);
            setRewards([]);
            return;
        }

        let cancelled = false;
        setIsBusy(true);
        setErrorMessage(null);

        fetchApiHealth(apiBaseUrl)
            .then(() => {
                if (cancelled) {
                    return;
                }
                setIsBackendReachable(true);
                return Promise.all([
                    fetchTrainingJobs(apiBaseUrl, token),
                    fetchTrainingLeaderboard(apiBaseUrl, token, 12),
                    fetchTrainingRewards(apiBaseUrl, token),
                ]).then(([jobsPayload, leaderboardPayload, rewardsPayload]) => {
                    if (cancelled) {
                        return;
                    }
                    setQueue(jobsPayload.jobs);
                    setLeaderboard(leaderboardPayload.leaderboard);
                    setRewards(rewardsPayload.rewards);
                    setStatusMessage('Synchronized with live command relay.');
                });
            })
            .catch((error: unknown) => {
                if (cancelled) {
                    return;
                }
                setIsBackendReachable(false);
                const message =
                    error instanceof Error ? error.message : 'Training backend unavailable.';
                setErrorMessage(
                    message.toLowerCase().includes('failed to fetch') ?
                        'Command relay unavailable. Server-authoritative operations are blocked until relay recovers.' :
                        message,
                );
                setStatusMessage(SERVER_REQUIRED_MESSAGE);
                setQueue([]);
                setLeaderboard([]);
                setRewards([]);
            })
            .finally(() => {
                if (!cancelled) {
                    setIsBusy(false);
                }
            });

        return () => {
            cancelled = true;
        };
    }, [apiBaseUrl, canUseApi, token]);

    const scaffoldBulkTodos = async () => {
        if (!isApiMode) {
            setErrorMessage(SERVER_REQUIRED_MESSAGE);
            setStatusMessage(SERVER_REQUIRED_MESSAGE);
            return;
        }

        const seeds = buildBulkQueueTemplate(selectedFile);
        setErrorMessage(null);
        setStatusMessage('Drafting operations queue...');

        setIsBusy(true);
        try {
            const response = await scaffoldTrainingJobs(apiBaseUrl, token, seeds);
            setQueue(response.jobs);
            await recordTrainingTransitionEvent(apiBaseUrl, token, {
                eventType: 'queue.scaffolded',
                correlationId: makeCorrelationId('queue-scaffold'),
                details: {
                    jobCount: response.jobs.length,
                    selectedFile,
                },
            });
            setStatusMessage(`Scaffolded ${response.jobs.length} jobs in persistent queue.`);
        } catch (error: unknown) {
            setErrorMessage(error instanceof Error ? error.message : 'Unable to scaffold queue.');
            setStatusMessage('Draft failed.');
        } finally {
            setIsBusy(false);
        }
    };

    const executeBulkTodos = async () => {
        if (!isApiMode) {
            setErrorMessage(SERVER_REQUIRED_MESSAGE);
            setStatusMessage(SERVER_REQUIRED_MESSAGE);
            return;
        }

        setErrorMessage(null);
        setStatusMessage('Running operations cycle...');

        const queueToRun =
            queue.length > 0 ? queue : (await scaffoldTrainingJobs(apiBaseUrl, token, buildBulkQueueTemplate(selectedFile))).jobs;
        setQueue(queueToRun);
        setIsBusy(true);
        const correlationId = makeCorrelationId('queue-execute');

        try {
            await recordTrainingTransitionEvent(apiBaseUrl, token, {
                eventType: 'queue.execution.started',
                correlationId,
                details: {
                    requestedJobs: queueToRun.length,
                },
            });

            for (let index = 0; index < queueToRun.length; index += 1) {
                const job = queueToRun[index];
                if (!job || job.status === 'completed') {
                    continue;
                }

                setQueue((current) => current.map((item) =>
                    item.jobId === job.jobId ? { ...item, status: 'running' } : item,
                ));

                const durationMs = 5_000 + selectedLineCount * 25 + index * 300;
                const scoreCap = Math.floor(durationMs / 5) + 4_500;
                const score = Math.max(
                    600,
                    Math.min(scoreCap, selectedLineCount * 28 + indexedFileCount + index * 120),
                );

                const completed = await executeTrainingJob(apiBaseUrl, token, job.jobId, {
                    score,
                    durationMs,
                    integrityProof: `${selectedFile || 'default'}:${job.jobId}:${durationMs}:${score}`,
                });

                setQueue((current) => current.map((item) =>
                    item.jobId === completed.job.jobId ? completed.job : item,
                ));
            }

            const [leaderboardPayload, rewardsPayload] = await Promise.all([
                fetchTrainingLeaderboard(apiBaseUrl, token, 12),
                fetchTrainingRewards(apiBaseUrl, token),
            ]);

            setLeaderboard(leaderboardPayload.leaderboard);
            setRewards(rewardsPayload.rewards);

            await recordTrainingTransitionEvent(apiBaseUrl, token, {
                eventType: 'queue.execution.completed',
                correlationId,
                details: {
                    completedJobs: queueToRun.length,
                    selectedFile,
                },
            });
            setStatusMessage(`Cycle complete: ${queueToRun.length} operations processed.`);
        } catch (error: unknown) {
            setErrorMessage(error instanceof Error ? error.message : 'Queue execution failed.');
            setStatusMessage('Cycle failed.');
        } finally {
            setIsBusy(false);
        }
    };

    const claimAllRewards = async () => {
        if (!isApiMode) {
            setErrorMessage(SERVER_REQUIRED_MESSAGE);
            setStatusMessage(SERVER_REQUIRED_MESSAGE);
            return;
        }

        if (pendingRewards.length === 0) {
            return;
        }

        setIsBusy(true);
        setErrorMessage(null);
        setStatusMessage('Claiming rewards...');

        try {
            for (const reward of pendingRewards) {
                await recordTrainingTransitionEvent(apiBaseUrl, token, {
                    eventType: 'reward.claim.attempted',
                    correlationId: makeCorrelationId('reward-claim'),
                    details: {
                        rewardId: reward.rewardId,
                        xp: reward.xp,
                    },
                });

                await claimTrainingReward(apiBaseUrl, token, reward.rewardId);

                await recordTrainingTransitionEvent(apiBaseUrl, token, {
                    eventType: 'reward.claim.succeeded',
                    correlationId: makeCorrelationId('reward-claim'),
                    details: {
                        rewardId: reward.rewardId,
                        xp: reward.xp,
                    },
                });
            }

            const [leaderboardPayload, rewardsPayload] = await Promise.all([
                fetchTrainingLeaderboard(apiBaseUrl, token, 12),
                fetchTrainingRewards(apiBaseUrl, token),
            ]);
            setLeaderboard(leaderboardPayload.leaderboard);
            setRewards(rewardsPayload.rewards);
            setStatusMessage('Stipends collected and standings refreshed.');
        } catch (error: unknown) {
            await recordTrainingTransitionEvent(apiBaseUrl, token, {
                eventType: 'reward.claim.failed',
                correlationId: makeCorrelationId('reward-claim'),
                details: {
                    pendingRewardCount: pendingRewards.length,
                },
            }).catch(() => undefined);
            setErrorMessage(error instanceof Error ? error.message : 'Reward claim failed.');
            setStatusMessage('Stipend collection failed.');
        } finally {
            setIsBusy(false);
        }
    };

    const content = (
        <>
            <p style={{ margin: '8px 0 0', color: '#fef9c3', fontSize: 12 }}>
                Network Mode: {isApiMode ? 'Live Relay (Server Authoritative)' : 'Relay Required (Server Authoritative)'}
                {isBusy ? ' | syncing...' : ''}
            </p>

            <div style={{ display: 'grid', gap: 8, gridTemplateColumns: 'repeat(auto-fit, minmax(150px, 1fr))', marginTop: 12 }}>
                <div style={{ border: '1px solid rgba(251, 191, 36, 0.25)', borderRadius: 10, padding: 10, background: 'rgba(120, 53, 15, 0.2)' }}>
                    <div style={{ fontSize: 12, color: '#fcd34d', textTransform: 'uppercase' }}>Governor Tier</div>
                    <div style={{ fontWeight: 700, fontSize: 18, marginTop: 3 }}>{reputationTier}</div>
                </div>
                <div style={{ border: '1px solid rgba(251, 191, 36, 0.25)', borderRadius: 10, padding: 10, background: 'rgba(120, 53, 15, 0.2)' }}>
                    <div style={{ fontSize: 12, color: '#fcd34d', textTransform: 'uppercase' }}>Command Capacity</div>
                    <div style={{ fontWeight: 700, fontSize: 18, marginTop: 3 }}>{trainingPower}</div>
                </div>
                <div style={{ border: '1px solid rgba(251, 191, 36, 0.25)', borderRadius: 10, padding: 10, background: 'rgba(120, 53, 15, 0.2)' }}>
                    <div style={{ fontSize: 12, color: '#fcd34d', textTransform: 'uppercase' }}>Treasury XP</div>
                    <div style={{ fontWeight: 700, fontSize: 18, marginTop: 3 }}>{totalRewardXP}</div>
                </div>
                <div style={{ border: '1px solid rgba(251, 191, 36, 0.25)', borderRadius: 10, padding: 10, background: 'rgba(120, 53, 15, 0.2)' }}>
                    <div style={{ fontSize: 12, color: '#fcd34d', textTransform: 'uppercase' }}>Pending Stipends</div>
                    <div style={{ fontWeight: 700, fontSize: 18, marginTop: 3 }}>{pendingRewards.length}</div>
                </div>
            </div>

            <h3 style={{ margin: '14px 0 6px', fontSize: 14, color: '#fef08a', letterSpacing: '0.03em', textTransform: 'uppercase' }}>
                Realm Standings
            </h3>
            <div style={{ display: 'grid', gap: 6 }}>
                {isApiMode ? leaderboard.map((entry, index) => (
                    <div key={`${entry.playerId}-${index}`} style={{ display: 'grid', gridTemplateColumns: '28px minmax(0, 1fr) auto auto', alignItems: 'center', gap: 8, border: '1px solid rgba(251, 191, 36, 0.18)', borderRadius: 10, padding: '6px 10px', background: index === 0 ? 'rgba(251, 191, 36, 0.15)' : 'rgba(120, 53, 15, 0.15)' }}>
                        <span style={{ color: '#fef08a', fontWeight: 700 }}>#{index + 1}</span>
                        <span style={{ color: '#fde68a', fontWeight: entry.playerId === queue[0]?.playerId ? 800 : 600 }}>{entry.playerId}</span>
                        <span style={{ color: '#fef9c3' }}>Score {entry.totalScore}</span>
                        <span style={{ color: '#fcd34d' }}>Jobs {entry.completedJobs}</span>
                    </div>
                )) : (
                    <div style={{ fontSize: 13, color: '#fde68a' }}>
                        Leaderboard unavailable until relay authentication is active.
                    </div>
                )}
            </div>

            <h3 style={{ margin: '14px 0 6px', fontSize: 14, color: '#fef08a', letterSpacing: '0.03em', textTransform: 'uppercase' }}>
                Production Queue
            </h3>
            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8, marginBottom: 8 }}>
                <button type="button" style={buttonStyle} onClick={() => void scaffoldBulkTodos()} disabled={isBusy}>
                    Draft Orders
                </button>
                <button type="button" style={buttonStyle} onClick={() => void executeBulkTodos()} disabled={isBusy || (!isApiMode && queue.length === 0)}>
                    Run Cycle
                </button>
                <button
                    type="button"
                    style={buttonStyle}
                    onClick={() => void claimAllRewards()}
                    disabled={isBusy || pendingRewards.length === 0}
                >
                    Collect Stipends
                </button>
            </div>
            <div style={{ display: 'grid', gap: 6 }}>
                {queue.map((item) => (
                    <div key={item.jobId} style={{ display: 'grid', gap: 2, border: '1px solid rgba(251, 191, 36, 0.18)', borderRadius: 10, padding: '8px 10px', background: item.status === 'completed' ? 'rgba(34, 197, 94, 0.18)' : item.status === 'running' ? 'rgba(14, 165, 233, 0.16)' : item.status === 'failed' ? 'rgba(239, 68, 68, 0.2)' : 'rgba(120, 53, 15, 0.15)' }}>
                        <div style={{ display: 'flex', justifyContent: 'space-between', gap: 8, color: '#fef9c3' }}>
                            <span>{item.title}</span>
                            <span>{item.status} | {item.antiCheatVerdict}</span>
                        </div>
                        <div style={{ fontSize: 12, color: '#fcd34d' }}>Reward {item.rewardXp} XP</div>
                    </div>
                ))}
                {queue.length === 0 ? (
                    <div style={{ fontSize: 13, color: '#fde68a' }}>
                        No operations queued yet. Start with "Draft Orders".
                    </div>
                ) : null}
            </div>

            {queue.length > 0 ? (
                <p style={{ margin: '10px 0 0', color: '#fef9c3', fontSize: 13 }}>
                    Completed {completedCount}/{queue.length} operations this cycle.
                </p>
            ) : null}

            {statusMessage.length > 0 ? (
                <p style={{ margin: '8px 0 0', color: '#fde68a', fontSize: 12 }}>
                    {statusMessage}
                </p>
            ) : null}

            {errorMessage ? (
                <p style={{ margin: '8px 0 0', color: '#fecaca', fontSize: 12 }}>
                    {errorMessage}
                </p>
            ) : null}
        </>
    );

    if (embedded) {
        return content;
    }

    return (
        <SurfaceCard
            title="Logistics Directorate"
            tone="amber"
            description="Province-scale command surface for planning orders, resolving cycles, and collecting stipends across the realm."
        >
            {content}
        </SurfaceCard>
    );
}
