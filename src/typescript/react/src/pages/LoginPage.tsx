import {
    buildAuthStartUrl,
    hasStoredAuthSession,
    parseAuthCallbackHash,
    readStoredAuthSession,
    storeAuthSession,
} from '@banana/ui';
import { useEffect, useMemo, useState } from 'react';
import { useLocation, useNavigate } from 'react-router-dom';

import { resolveApiBaseResolutionError, resolveApiBaseUrl } from '../lib/api';
import { buildLoginStartUrl, resolveLoginReturnTarget } from '../lib/authRouting';

export function LoginPage() {
    const navigate = useNavigate();
    const location = useLocation();
    const [loading, setLoading] = useState(false);
    const [errorMessage, setErrorMessage] = useState<string | null>(null);
    const returnToTarget = useMemo(() => resolveLoginReturnTarget(location.search), [location.search]);

    const apiBaseError = resolveApiBaseResolutionError();
    const apiBaseUrl = resolveApiBaseUrl();

    useEffect(() => {
        if (typeof window === 'undefined') {
            return;
        }

        const callback = parseAuthCallbackHash(window.location.hash);
        if (callback) {
            storeAuthSession(callback);
            window.history.replaceState({}, document.title, `${window.location.pathname}${window.location.search}`);
            navigate(returnToTarget, { replace: true });
            return;
        }

        if (hasStoredAuthSession() || readStoredAuthSession()) {
            navigate(returnToTarget, { replace: true });
        }
    }, [navigate, returnToTarget]);

    const handleGitHubLogin = async () => {
        setLoading(true);
        setErrorMessage(null);

        try {
            if (apiBaseError) {
                throw new Error(apiBaseError);
            }

            const loginUrl = buildAuthStartUrl(apiBaseUrl, buildLoginStartUrl(returnToTarget), 'github');
            window.location.assign(loginUrl);
        } catch (cause: unknown) {
            setErrorMessage(cause instanceof Error ? cause.message : 'Unable to start Keycloak login.');
        } finally {
            setLoading(false);
        }
    };

    return (
        <main
            style={{
                minHeight: '100dvh',
                margin: 0,
                display: 'grid',
                placeItems: 'center',
                background: '#0d1117',
                color: '#f0f6fc',
                padding: '24px',
                fontFamily: "-apple-system, BlinkMacSystemFont, 'Segoe UI', Helvetica, Arial, sans-serif",
            }}
        >
            <section style={{ width: '100%', maxWidth: 340 }}>
                <div style={{ display: 'grid', justifyItems: 'center', marginBottom: 16 }}>
                    <div
                        aria-hidden="true"
                        style={{
                            width: 48,
                            height: 48,
                            borderRadius: 999,
                            background: '#f0f6fc',
                            color: '#0d1117',
                            display: 'grid',
                            placeItems: 'center',
                            fontWeight: 700,
                            fontSize: 22,
                        }}
                    >
                        B
                    </div>
                    <h1 style={{ margin: '16px 0 0', fontSize: 24, fontWeight: 300 }}>
                        Sign in to Banana
                    </h1>
                </div>

                <div
                    style={{
                        border: '1px solid #30363d',
                        borderRadius: 6,
                        background: '#161b22',
                        padding: 16,
                    }}
                >
                    <button
                        type="button"
                        onClick={handleGitHubLogin}
                        disabled={loading}
                        style={{
                            width: '100%',
                            border: '1px solid rgba(240, 246, 252, 0.1)',
                            borderRadius: 6,
                            background: loading ? '#30363d' : '#238636',
                            color: '#ffffff',
                            fontSize: 14,
                            fontWeight: 600,
                            padding: '10px 14px',
                            cursor: loading ? 'not-allowed' : 'pointer',
                        }}
                    >
                        {loading ? 'Redirecting to GitHub...' : 'Continue with GitHub'}
                    </button>

                    {errorMessage ? (
                        <p style={{ margin: '12px 0 0', color: '#ff7b72', fontSize: 13 }}>{errorMessage}</p>
                    ) : null}
                </div>
            </section>
        </main>
    );
}
