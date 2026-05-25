import { useEffect, useMemo, useState, type CSSProperties } from 'react';

import { ErrorText } from '../components/ErrorText';
import { tokens } from '../tokens';
import { fetchGameAccountOverview, fetchPlayerAccount, fetchPlayerInsights, resolveApiBaseResolutionError, resolveApiBaseUrl, updatePlayerAccountProfile, type GameAccountOverviewResponse, type PlayerAccountResponse, type PlayerInsightsResponse, } from '../game-engine/api';
import { clearStoredAuthSession, readStoredAuthSession } from './session';

type LoadState = 'idle' | 'loading' | 'ready' | 'error';

function readProfileDisplayName(profile: Record<string, unknown>): string {
    const raw = profile['displayName'];
    if (typeof raw !== 'string') {
        return '';
    }
    return raw.trim();
}

export function SessionRoomPage() {
    const session = useMemo(() => readStoredAuthSession(), []);
    const apiBaseError = resolveApiBaseResolutionError();
    const apiBaseUrl = resolveApiBaseUrl();

    const [loadState, setLoadState] = useState<LoadState>('idle');
    const [error, setError] = useState<string | null>(null);
    const [onboardingSaving, setOnboardingSaving] = useState(false);

    const [account, setAccount] = useState<PlayerAccountResponse | null>(null);
    const [insights, setInsights] = useState<PlayerInsightsResponse | null>(null);
    const [overview, setOverview] = useState<GameAccountOverviewResponse | null>(null);
    const [displayNameInput, setDisplayNameInput] = useState('');

    const needsOnboarding =
        account ? readProfileDisplayName(account.profile).length === 0 : false;

    useEffect(() => {
        if (!session?.token) {
            return;
        }

        if (apiBaseError) {
            setLoadState('error');
            setError(apiBaseError);
            return;
        }

        let cancelled = false;
        setLoadState('loading');
        setError(null);

        Promise.all([
            fetchPlayerAccount(apiBaseUrl, session.token),
            fetchPlayerInsights(apiBaseUrl, session.token),
            fetchGameAccountOverview(apiBaseUrl, session.token),
        ])
            .then(([accountPayload, insightsPayload, overviewPayload]) => {
                if (cancelled) {
                    return;
                }

                setAccount(accountPayload);
                setInsights(insightsPayload);
                setOverview(overviewPayload);
                setDisplayNameInput(readProfileDisplayName(accountPayload.profile));
                setLoadState('ready');
            })
            .catch((cause: unknown) => {
                if (cancelled) {
                    return;
                }

                setLoadState('error');
                setError(cause instanceof Error ? cause.message : String(cause));
            });

        return () => {
            cancelled = true;
        };
    }, [apiBaseError, apiBaseUrl, session?.token]);

    const handleSignOut = () => {
        clearStoredAuthSession();
        sessionStorage.removeItem('banana-game-session');
        window.location.assign('/login');
    };

    const handleOnboardingSubmit = async () => {
        if (!session?.token || !account) {
            return;
        }

        const displayName = displayNameInput.trim();
        if (!displayName) {
            setError('Enter a character name to finish onboarding.');
            return;
        }

        setOnboardingSaving(true);
        setError(null);

        try {
            const updated = await updatePlayerAccountProfile(
                apiBaseUrl,
                session.token,
                account.version,
                {
                    ...account.profile,
                    displayName,
                    onboardingCompletedAt: new Date().toISOString(),
                },
            );
            setAccount(updated);
            sessionStorage.removeItem('banana-game-session');
        } catch (cause: unknown) {
            setError(cause instanceof Error ? cause.message : String(cause));
        } finally {
            setOnboardingSaving(false);
        }
    };

    if (!session?.token) {
        return (
            <main style={shellStyle}>
                <section style={panelStyle}>
                    <h1 style={titleStyle}>Steam login required</h1>
                    <p style={bodyStyle}>
                        Sign in with Steam to load your production-backed character and world state.
                    </p>
                    <a href="/login" style={actionStyle}>Go to login</a>
                </section>
            </main>
        );
    }

    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <div style={headerRowStyle}>
                    <div>
                        <p style={eyebrowStyle}>Local game client session room</p>
                        <h1 style={titleStyle}>Character menu and onboarding</h1>
                        <p style={bodyStyle}>
                            Steam-authenticated production data powers this local game-client menu.
                            Admin and operator controls stay out of this user-facing flow.
                        </p>
                    </div>
                    <div style={{ display: 'grid', gap: 10 }}>
                        <button type="button" onClick={handleSignOut} style={buttonSecondaryStyle}>
                            Sign out
                        </button>
                    </div>
                </div>

                <div style={noticeStyle}>
                    Web gameplay is removed from this monorepo. Use the DX12 client for gameplay runtime.
                    This menu now manages Steam auth-backed character state and onboarding only.
                </div>

                {loadState === 'loading' ? <p style={bodyStyle}>Loading character and world data...</p> : null}
                {error ? <ErrorText>{error}</ErrorText> : null}

                {loadState === 'ready' && account && insights && overview ? (
                    <div style={gridStyle}>
                        <article style={cardStyle}>
                            <h2 style={cardTitleStyle}>Character identity</h2>
                            <dl style={dlStyle}>
                                <div style={rowStyle}><dt style={dtStyle}>Steam ID</dt><dd style={ddStyle}>{overview.steamId}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Player GUID</dt><dd style={ddStyle}>{overview.playerGuid}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Display name</dt><dd style={ddStyle}>{readProfileDisplayName(account.profile) || 'Unassigned'}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Account status</dt><dd style={ddStyle}>{account.accountStatus}</dd></div>
                            </dl>
                        </article>

                        <article style={cardStyle}>
                            <h2 style={cardTitleStyle}>World state</h2>
                            <dl style={dlStyle}>
                                <div style={rowStyle}><dt style={dtStyle}>Mode</dt><dd style={ddStyle}>{overview.mode}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Entities</dt><dd style={ddStyle}>{overview.world.entityCount}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Last snapshot tick</dt><dd style={ddStyle}>{overview.world.lastSnapshotTick}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Session ID</dt><dd style={ddStyle}>{overview.sessionId}</dd></div>
                            </dl>
                        </article>

                        <article style={cardStyle}>
                            <h2 style={cardTitleStyle}>Progression snapshot</h2>
                            <dl style={dlStyle}>
                                <div style={rowStyle}><dt style={dtStyle}>Level</dt><dd style={ddStyle}>{insights.progressionSummary.level}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>XP</dt><dd style={ddStyle}>{insights.progressionSummary.xp}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Distinct items</dt><dd style={ddStyle}>{insights.inventoryTrendSummary.distinctItems}</dd></div>
                                <div style={rowStyle}><dt style={dtStyle}>Build attack</dt><dd style={ddStyle}>{overview.buildStats.attack}</dd></div>
                            </dl>
                        </article>

                        <article style={cardStyle}>
                            <h2 style={cardTitleStyle}>New player onboarding</h2>
                            {needsOnboarding ? (
                                <>
                                    <p style={bodyStyle}>First login detected. Choose a character name to finish onboarding.</p>
                                    <input
                                        value={displayNameInput}
                                        onChange={(event) => setDisplayNameInput(event.target.value)}
                                        placeholder="Enter character name"
                                        style={inputStyle}
                                    />
                                    <button type="button" onClick={handleOnboardingSubmit} style={buttonPrimaryStyle} disabled={onboardingSaving}>
                                        {onboardingSaving ? 'Saving...' : 'Save character profile'}
                                    </button>
                                </>
                            ) : (
                                <p style={bodyStyle}>
                                    Onboarding complete. Your character profile is ready for launch.
                                </p>
                            )}
                        </article>
                    </div>
                ) : null}
            </section>
        </main>
    );
}

const shellStyle: CSSProperties = {
    minHeight: '100dvh',
    padding: '36px 20px',
    display: 'grid',
    placeItems: 'center',
    background:
        'radial-gradient(circle at 10% 12%, rgba(56, 189, 248, 0.16), transparent 32%), radial-gradient(circle at 86% 18%, rgba(250, 204, 21, 0.18), transparent 36%), linear-gradient(160deg, #050816 0%, #0b1327 54%, #111f3f 100%)',
    color: '#f8fafc',
};

const panelStyle: CSSProperties = {
    width: 'min(100%, 1160px)',
    borderRadius: 24,
    border: '1px solid rgba(148, 163, 184, 0.24)',
    padding: 28,
    background: 'rgba(8, 12, 27, 0.76)',
    boxShadow: '0 24px 90px rgba(2, 6, 23, 0.52)',
};

const headerRowStyle: CSSProperties = {
    display: 'flex',
    justifyContent: 'space-between',
    gap: 18,
    flexWrap: 'wrap',
};

const eyebrowStyle: CSSProperties = {
    margin: 0,
    textTransform: 'uppercase',
    letterSpacing: '0.13em',
    color: '#7dd3fc',
    fontWeight: tokens.font.weight.semibold,
    fontSize: tokens.font.size.xs,
};

const titleStyle: CSSProperties = {
    margin: '12px 0 8px',
    fontSize: 'clamp(2rem, 4vw, 3rem)',
    lineHeight: 1,
};

const bodyStyle: CSSProperties = {
    margin: 0,
    color: '#cbd5e1',
    lineHeight: 1.6,
};

const actionStyle: CSSProperties = {
    marginTop: 18,
    display: 'inline-flex',
    alignItems: 'center',
    padding: '12px 16px',
    borderRadius: 12,
    textDecoration: 'none',
    color: '#fff',
    fontWeight: 700,
    background: 'linear-gradient(135deg, #2563eb, #0891b2)',
};

const noticeStyle: CSSProperties = {
    marginTop: 16,
    borderRadius: 14,
    border: '1px solid rgba(56, 189, 248, 0.3)',
    background: 'rgba(8, 47, 73, 0.32)',
    color: '#bae6fd',
    padding: '12px 14px',
    lineHeight: 1.5,
};

const buttonPrimaryStyle: CSSProperties = {
    border: '1px solid rgba(59, 130, 246, 0.34)',
    borderRadius: 12,
    padding: '10px 14px',
    background: 'linear-gradient(135deg, #2563eb, #0f766e)',
    color: '#eff6ff',
    fontWeight: 700,
    cursor: 'pointer',
};

const buttonSecondaryStyle: CSSProperties = {
    border: '1px solid rgba(148, 163, 184, 0.34)',
    borderRadius: 12,
    padding: '10px 14px',
    background: 'rgba(15, 23, 42, 0.82)',
    color: '#e2e8f0',
    fontWeight: 600,
    cursor: 'pointer',
};

const gridStyle: CSSProperties = {
    marginTop: 24,
    display: 'grid',
    gap: 14,
    gridTemplateColumns: 'repeat(auto-fit, minmax(250px, 1fr))',
};

const cardStyle: CSSProperties = {
    borderRadius: 16,
    border: '1px solid rgba(148, 163, 184, 0.22)',
    background: 'rgba(15, 23, 42, 0.58)',
    padding: 16,
    display: 'grid',
    gap: 10,
};

const cardTitleStyle: CSSProperties = {
    margin: 0,
    fontSize: 18,
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
    maxWidth: '65%',
    textAlign: 'right',
    wordBreak: 'break-word',
};

const inputStyle: CSSProperties = {
    borderRadius: 12,
    border: '1px solid rgba(148, 163, 184, 0.35)',
    background: 'rgba(2, 6, 23, 0.72)',
    color: '#f8fafc',
    padding: '10px 12px',
    fontSize: tokens.font.size.md,
};
