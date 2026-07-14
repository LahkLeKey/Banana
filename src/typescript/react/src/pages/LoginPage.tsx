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
    const [loadingTarget, setLoadingTarget] = useState<null | 'github' | 'local-signin' | 'local-signup'>(null);
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

    const beginLoginRedirect = async (target: 'github' | 'local-signin' | 'local-signup') => {
        setLoadingTarget(target);
        setErrorMessage(null);

        try {
            if (apiBaseError) {
                throw new Error(apiBaseError);
            }

            const provider = target === 'github' ? 'github' : undefined;
            const action = target === 'local-signup' ? 'register' : undefined;
            const loginUrl = buildAuthStartUrl(apiBaseUrl, buildLoginStartUrl(returnToTarget), provider, action);
            window.location.assign(loginUrl);
        } catch (cause: unknown) {
            setErrorMessage(cause instanceof Error ? cause.message : 'Unable to start Keycloak login.');
        } finally {
            setLoadingTarget(null);
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
                        onClick={() => {
                            void beginLoginRedirect('github');
                        }}
                        disabled={loadingTarget !== null}
                        style={{
                            width: '100%',
                            border: '1px solid rgba(240, 246, 252, 0.1)',
                            borderRadius: 6,
                            background: loadingTarget !== null ? '#30363d' : '#238636',
                            color: '#ffffff',
                            fontSize: 14,
                            fontWeight: 600,
                            padding: '10px 14px',
                            cursor: loadingTarget !== null ? 'not-allowed' : 'pointer',
                        }}
                    >
                        {loadingTarget === 'github' ? 'Redirecting to GitHub...' : 'Continue with GitHub'}
                    </button>

                    <button
                        type="button"
                        onClick={() => {
                            void beginLoginRedirect('local-signin');
                        }}
                        disabled={loadingTarget !== null}
                        style={{
                            width: '100%',
                            marginTop: 10,
                            border: '1px solid rgba(240, 246, 252, 0.22)',
                            borderRadius: 6,
                            background: 'transparent',
                            color: '#f0f6fc',
                            fontSize: 14,
                            fontWeight: 600,
                            padding: '10px 14px',
                            cursor: loadingTarget !== null ? 'not-allowed' : 'pointer',
                        }}
                    >
                        {loadingTarget === 'local-signin' ? 'Redirecting...' : 'Continue with Email and Password'}
                    </button>

                    <button
                        type="button"
                        onClick={() => {
                            void beginLoginRedirect('local-signup');
                        }}
                        disabled={loadingTarget !== null}
                        style={{
                            width: '100%',
                            marginTop: 10,
                            border: '1px dashed rgba(240, 246, 252, 0.35)',
                            borderRadius: 6,
                            background: 'transparent',
                            color: '#f0f6fc',
                            fontSize: 14,
                            fontWeight: 600,
                            padding: '10px 14px',
                            cursor: loadingTarget !== null ? 'not-allowed' : 'pointer',
                        }}
                    >
                        {loadingTarget === 'local-signup' ? 'Redirecting to Sign Up...' : 'Create Banana Account'}
                    </button>

                    {errorMessage ? (
                        <p style={{ margin: '12px 0 0', color: '#ff7b72', fontSize: 13 }}>{errorMessage}</p>
                    ) : null}
                </div>
            </section>
        </main>
    );
}
