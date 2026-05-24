import {useEffect, useMemo, useState, type CSSProperties} from 'react';
import {
  clearStoredAuthSession,
  logoutAuthSession,
  readStoredAuthSession,
} from '@banana/ui';

import {
  fetchPlayerAccount,
  fetchPlayerInsights,
  resolveApiBaseResolutionError,
  resolveApiBaseUrl,
  type PlayerAccountResponse,
  type PlayerInsightsResponse,
} from '../lib/api';

type LoadState = 'idle'|'loading'|'ready'|'error';

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
      <main style={shellStyle}>
        <section style={panelStyle}>
          <h1 style={titleStyle}>Steam login required</h1>
          <p style={bodyStyle}>
            Sign in with Steam to view your account details and gameplay stats.
          </p>
          <a href="/login" style={actionStyle}>Continue to login</a>
        </section>
      </main>
    );
  }

  return (
    <main style={shellStyle}>
      <section style={panelStyle}>
        <div style={{display: 'flex', justifyContent: 'space-between', gap: 16, flexWrap: 'wrap'}}>
          <div>
            <p style={eyebrowStyle}>Banana player portal</p>
            <h1 style={titleStyle}>Your account and stats</h1>
            <p style={bodyStyle}>
              Public player data only. Admin and operator controls are excluded from this website.
            </p>
          </div>
          <button type="button" onClick={handleSignOut} style={buttonStyle}>Sign out</button>
        </div>

        {loadState === 'loading' ? <p style={bodyStyle}>Loading your latest stats...</p> : null}

        {loadState === 'error' ? (
          <div style={errorPanelStyle}>
            <strong>Unable to load account data.</strong>
            <div style={{marginTop: 6}}>{errorMessage ?? 'Unknown error.'}</div>
          </div>
        ) : null}

        {loadState === 'ready' && account && insights ? (
          <div style={gridStyle}>
            <article style={cardStyle}>
              <h2 style={cardTitleStyle}>Identity</h2>
              <dl style={dlStyle}>
                <div style={rowStyle}><dt style={dtStyle}>Steam ID</dt><dd style={ddStyle}>{account.playerId}</dd></div>
                <div style={rowStyle}><dt style={dtStyle}>Status</dt><dd style={ddStyle}>{account.accountStatus}</dd></div>
                <div style={rowStyle}><dt style={dtStyle}>Version</dt><dd style={ddStyle}>{account.version}</dd></div>
              </dl>
            </article>

            <article style={cardStyle}>
              <h2 style={cardTitleStyle}>Progression</h2>
              <dl style={dlStyle}>
                <div style={rowStyle}><dt style={dtStyle}>Level</dt><dd style={ddStyle}>{insights.progressionSummary.level}</dd></div>
                <div style={rowStyle}><dt style={dtStyle}>XP</dt><dd style={ddStyle}>{insights.progressionSummary.xp}</dd></div>
                <div style={rowStyle}><dt style={dtStyle}>No data flag</dt><dd style={ddStyle}>{String(insights.noData)}</dd></div>
              </dl>
            </article>

            <article style={cardStyle}>
              <h2 style={cardTitleStyle}>Gameplay snapshot</h2>
              <dl style={dlStyle}>
                <div style={rowStyle}><dt style={dtStyle}>Change events</dt><dd style={ddStyle}>{insights.sessionSummary.changeEvents}</dd></div>
                <div style={rowStyle}><dt style={dtStyle}>Distinct items</dt><dd style={ddStyle}>{insights.inventoryTrendSummary.distinctItems}</dd></div>
                <div style={rowStyle}><dt style={dtStyle}>Total quantity</dt><dd style={ddStyle}>{insights.inventoryTrendSummary.totalQuantity}</dd></div>
              </dl>
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
      'radial-gradient(circle at 12% 8%, rgba(251, 191, 36, 0.18), transparent 34%), radial-gradient(circle at 90% 24%, rgba(34, 197, 94, 0.16), transparent 36%), linear-gradient(150deg, #060917 0%, #0c1228 54%, #101a35 100%)',
  color: '#f8fafc',
};

const panelStyle: CSSProperties = {
  width: 'min(100%, 1100px)',
  borderRadius: 24,
  border: '1px solid rgba(148, 163, 184, 0.25)',
  padding: 28,
  background: 'rgba(8, 12, 27, 0.72)',
  boxShadow: '0 24px 90px rgba(2, 6, 23, 0.5)',
};

const eyebrowStyle: CSSProperties = {
  margin: 0,
  textTransform: 'uppercase',
  letterSpacing: '0.13em',
  color: '#bbf7d0',
  fontWeight: 700,
  fontSize: 12,
};

const titleStyle: CSSProperties = {
  margin: '12px 0 8px',
  fontSize: 'clamp(2rem, 4vw, 3.2rem)',
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
  background: 'linear-gradient(135deg, #16a34a, #0f766e)',
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

const gridStyle: CSSProperties = {
  marginTop: 22,
  display: 'grid',
  gap: 14,
  gridTemplateColumns: 'repeat(auto-fit, minmax(230px, 1fr))',
};

const cardStyle: CSSProperties = {
  borderRadius: 16,
  border: '1px solid rgba(148, 163, 184, 0.2)',
  background: 'rgba(15, 23, 42, 0.52)',
  padding: 16,
};

const cardTitleStyle: CSSProperties = {
  margin: '0 0 10px',
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
};
