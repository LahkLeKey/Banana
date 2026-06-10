import { useEffect, useMemo, useState, type CSSProperties } from 'react';
import {
    clearStoredAuthSession,
    logoutAuthSession,
    readStoredAuthSession,
    RouteActionLink,
    RouteBody,
    RouteEyebrow,
    RouteInfoCard,
    RouteInfoGrid,
    RoutePanel,
    RouteShell,
    RouteTitle,
} from '@banana/ui';

import {
    fetchPlayerAccount,
    fetchPlayerInsights,
    resolveApiBaseResolutionError,
    resolveApiBaseUrl,
    type PlayerAccountResponse,
    type PlayerInsightsResponse,
} from '../lib/api';

type LoadState = 'idle' | 'loading' | 'ready' | 'error';

export function PlayerPortalPage() {
    const [loadState, setLoadState] = useState<LoadState>('idle');
    const [errorMessage, setErrorMessage] = useState<string | null>(null);
    const [account, setAccount] = useState<PlayerAccountResponse | null>(null);
    const [insights, setInsights] = useState<PlayerInsightsResponse | null>(null);

    const apiBaseError = resolveApiBaseResolutionError();
    const apiBaseUrl = resolveApiBaseUrl();
    const session = useMemo(() => readStoredAuthSession(), []);

    useEffect(() => {
        if (!session?.token) {
            return;
        }

        if (apiBaseError) {
            setLoadState('error');
            setErrorMessage(apiBaseError);
            return;
        }

        let cancelled = false;
        setLoadState('loading');
        setErrorMessage(null);

        Promise.all([
            fetchPlayerAccount(apiBaseUrl, session.token),
            fetchPlayerInsights(apiBaseUrl, session.token),
        ])
            .then(([accountPayload, insightsPayload]) => {
                if (cancelled) {
                    return;
                }
                setAccount(accountPayload);
                setInsights(insightsPayload);
                setLoadState('ready');
            })
            .catch((error: unknown) => {
                if (cancelled) {
                    return;
                }

                setLoadState('error');
                setErrorMessage(
                    error instanceof Error ? error.message :
                        'Unable to load player account details.');
            });

        return () => {
            cancelled = true;
        };
    }, [apiBaseError, apiBaseUrl, session?.token]);

    const handleSignOut = async () => {
        const activeSession = readStoredAuthSession();
        if (activeSession?.token && apiBaseUrl) {
            await logoutAuthSession(apiBaseUrl, activeSession.token);
        }

        clearStoredAuthSession();
        if (typeof window !== 'undefined') {
            window.location.assign('/login');
        }
    };

    if (!session?.token) {
        return (
            <RouteShell background={shellStyle.background as string}>
                <RoutePanel width="min(100%, 1100px)">
                    <RouteTitle>Steam login required</RouteTitle>
                    <RouteBody>
                        Sign in with Steam to view your account details and gameplay stats.
                    </RouteBody>
                    <div style={{ marginTop: 18 }}>
                        <RouteActionLink href="/login" tone="primary">Continue to login</RouteActionLink>
                    </div>
                </RoutePanel>
            </RouteShell>
        );
    }

    return (
        <RouteShell background={shellStyle.background as string}>
            <RoutePanel width="min(100%, 1100px)">
                <div style={{ display: 'flex', justifyContent: 'space-between', gap: 16, flexWrap: 'wrap' }}>
                    <div>
                        <RouteEyebrow color="#bbf7d0">Banana player portal</RouteEyebrow>
                        <RouteTitle>Your account and stats</RouteTitle>
                        <RouteBody>
                            Public player data only. Admin and operator controls are excluded from this website.
                        </RouteBody>
                    </div>
                    <button type="button" onClick={handleSignOut} style={buttonStyle}>Sign out</button>
                </div>

                {loadState === 'loading' ? <RouteBody>Loading your latest stats...</RouteBody> : null}

                {loadState === 'error' ? (
                    <div style={errorPanelStyle}>
                        <strong>Unable to load account data.</strong>
                        <div style={{ marginTop: 6 }}>{errorMessage ?? 'Unknown error.'}</div>
                    </div>
                ) : null}

                {loadState === 'ready' && account && insights ? (
                    <RouteInfoGrid>
                        <RouteInfoCard title="Identity">
                            <dl style={dlStyle}>
                                <div style={rowStyle}><dt style={dtStyle}>Steam ID</dt><dd style={ddStyle}>{account.playerId}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Status</dt><dd style={ddStyle}>{account.accountStatus}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Version</dt><dd style={ddStyle}>{account.version}</dd></div>
                            </dl>
                        </RouteInfoCard>

                        <RouteInfoCard title="Progression">
                            <dl style={dlStyle}>
                                <div style={rowStyle}><dt style={dtStyle}>Level</dt><dd style={ddStyle}>{insights.progressionSummary.level}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>XP</dt><dd style={ddStyle}>{insights.progressionSummary.xp}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>No data flag</dt><dd style={ddStyle}>{String(insights.noData)}</dd></div>
                            </dl>
                        </RouteInfoCard>

                        <RouteInfoCard title="Gameplay snapshot">
                            <dl style={dlStyle}>
                                <div style={rowStyle}><dt style={dtStyle}>Change events</dt><dd style={ddStyle}>{insights.sessionSummary.changeEvents}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Distinct items</dt><dd style={ddStyle}>{insights.inventoryTrendSummary.distinctItems}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Total quantity</dt><dd style={ddStyle}>{insights.inventoryTrendSummary.totalQuantity}</dd></div>
                            </dl>
                        </RouteInfoCard>
                    </RouteInfoGrid>
                ) : null}
            </RoutePanel>
        </RouteShell>
    );
}

const shellStyle: CSSProperties = {
    minHeight: '100dvh',
    padding: '36px 20px',
    display: 'grid',
    placeItems: 'center',
    background:
        'radial-gradient(circle at 12% 8%, rgba(251, 191, 36, 0.18), transparent 34%), radial-gradient(circle at 90% 24%, rgba(34, 197, 94, 0.16), transparent 36%), linear-gradient(150deg, #060917 0%, #0c1228 54%, #101a35 100%)',
    color: '#f8fafc',
};

const buttonStyle: CSSProperties = {
    alignSelf: 'start',
    border: '1px solid rgba(148, 163, 184, 0.35)',
    background: 'rgba(15, 23, 42, 0.85)',
    color: '#e2e8f0',
    borderRadius: 12,
    padding: '10px 14px',
    fontWeight: 600,
    cursor: 'pointer',
};

const errorPanelStyle: CSSProperties = {
    marginTop: 18,
    padding: 14,
    borderRadius: 14,
    border: '1px solid rgba(248, 113, 113, 0.45)',
    background: 'rgba(127, 29, 29, 0.26)',
    color: '#fecaca',
};

const dlStyle: CSSProperties = {
    margin: 0,
    display: 'grid',
    gap: 10,
};

const rowStyle: CSSProperties = {
    display: 'flex',
    justifyContent: 'space-between',
    gap: 10,
};

const dtStyle: CSSProperties = {
    color: '#94a3b8',
};

const ddStyle: CSSProperties = {
    margin: 0,
    color: '#f8fafc',
    fontWeight: 600,
};
