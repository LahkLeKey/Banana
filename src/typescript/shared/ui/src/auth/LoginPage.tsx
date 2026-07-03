import { useEffect, useState } from "react";

import { ErrorText } from "../components/ErrorText";
import { tokens } from "../tokens";
import { resolveApiBaseResolutionError, resolveApiBaseUrl } from "../game-engine/api";
import {
    type AuthProvider,
    buildAuthStartUrl,
    clearStoredAuthSession,
    hasStoredAuthSession,
    parseAuthCallbackHash,
    resolveLoginReturnToUrl,
    storeAuthSession,
} from "./session";

const PANEL_MAX_WIDTH = 1120;

type LoginStatus = "idle" | "redirecting" | "authenticated";

export function LoginPage() {
    const [status, setStatus] = useState<LoginStatus>("idle");
    const [error, setError] = useState<string | null>(null);
    const [sessionReady, setSessionReady] = useState<boolean>(false);

    const apiBaseError = resolveApiBaseResolutionError();
    const apiBaseUrl = resolveApiBaseUrl();

    useEffect(() => {
        if (typeof window === "undefined") {
            return;
        }

        const callback = parseAuthCallbackHash(window.location.hash);
        if (callback) {
            storeAuthSession(callback);
            window.history.replaceState({}, document.title, `${window.location.pathname}${window.location.search}`);
            setSessionReady(true);
            setStatus("authenticated");
            window.location.replace("/session-room");
            return;
        }

        if (hasStoredAuthSession()) {
            setSessionReady(true);
            setStatus("authenticated");
        }
    }, []);

    const handleIdentitySignIn = (provider?: AuthProvider) => {
        if (apiBaseError) {
            setError(apiBaseError);
            return;
        }

        if (typeof window === "undefined") {
            return;
        }

        setError(null);
        setStatus("redirecting");
        const loginUrl = buildAuthStartUrl(apiBaseUrl, resolveLoginReturnToUrl(), provider);
        window.location.assign(loginUrl);
    };

    const handleSignOut = () => {
        clearStoredAuthSession();
        setSessionReady(false);
        setStatus("idle");
    };

    const handleOpenSessionRoom = () => {
        if (typeof window === "undefined") {
            return;
        }
        window.location.assign("/session-room");
    };

    return (
        <main
            data-testid="banana-login-page"
            style={{
                position: "relative",
                minHeight: "100dvh",
                overflow: "hidden",
                background:
                    "radial-gradient(circle at top left, rgba(245, 158, 11, 0.24), transparent 28%), radial-gradient(circle at 85% 20%, rgba(16, 185, 129, 0.18), transparent 24%), linear-gradient(135deg, #060816 0%, #0b1324 52%, #111a31 100%)",
                color: "#f8fafc",
            }}
        >
            <style>{`
        @keyframes loginGlowFloat {
          0% { transform: translate3d(0, 0, 0) scale(1); }
          50% { transform: translate3d(0, -12px, 0) scale(1.06); }
          100% { transform: translate3d(0, 0, 0) scale(1); }
        }
        @keyframes loginCardRise {
          from { opacity: 0; transform: translate3d(0, 24px, 0) scale(0.98); }
          to { opacity: 1; transform: translate3d(0, 0, 0) scale(1); }
        }
        @keyframes loginSheen {
          from { transform: translateX(-20%); }
          to { transform: translateX(120%); }
        }
      `}</style>

            <div
                aria-hidden="true"
                style={{
                    position: "absolute",
                    inset: 0,
                    background:
                        "linear-gradient(110deg, rgba(15, 23, 42, 0.12) 0%, rgba(15, 23, 42, 0.03) 30%, rgba(15, 23, 42, 0.12) 60%, rgba(15, 23, 42, 0.03) 100%)",
                    backgroundSize: "220px 220px",
                    maskImage: "radial-gradient(circle at center, black, transparent 75%)",
                    opacity: 0.55,
                }}
            />
            <div
                aria-hidden="true"
                style={{
                    position: "absolute",
                    inset: 0,
                    pointerEvents: "none",
                }}
            >
                <div
                    style={{
                        position: "absolute",
                        top: "10%",
                        left: "4%",
                        width: 240,
                        height: 240,
                        borderRadius: "999px",
                        background: "rgba(245, 158, 11, 0.28)",
                        filter: "blur(34px)",
                        animation: "loginGlowFloat 11s ease-in-out infinite",
                    }}
                />
                <div
                    style={{
                        position: "absolute",
                        top: "42%",
                        right: "6%",
                        width: 280,
                        height: 280,
                        borderRadius: "999px",
                        background: "rgba(16, 185, 129, 0.18)",
                        filter: "blur(44px)",
                        animation: "loginGlowFloat 13s ease-in-out infinite reverse",
                    }}
                />
            </div>

            <div
                style={{
                    position: "relative",
                    zIndex: 1,
                    minHeight: "100dvh",
                    display: "grid",
                    placeItems: "center",
                    padding: "40px 20px",
                }}
            >
                <section
                    style={{
                        width: "min(100%, 1120px)",
                        display: "grid",
                        gridTemplateColumns: "1.15fr 0.85fr",
                        gap: 24,
                        alignItems: "stretch",
                    }}
                >
                    <div
                        style={{
                            border: "1px solid rgba(148, 163, 184, 0.22)",
                            borderRadius: 28,
                            background:
                                "linear-gradient(180deg, rgba(15, 23, 42, 0.72), rgba(15, 23, 42, 0.46))",
                            boxShadow: "0 24px 90px rgba(2, 6, 23, 0.5)",
                            padding: 32,
                            position: "relative",
                            overflow: "hidden",
                            animation: "loginCardRise 420ms ease-out both",
                        }}
                    >
                        <div
                            style={{
                                position: "absolute",
                                inset: 0,
                                background:
                                    "linear-gradient(120deg, transparent 0%, rgba(255, 255, 255, 0.12) 48%, transparent 60%)",
                                opacity: 0.32,
                                transform: "translateX(-20%)",
                                animation: "loginSheen 8s ease-in-out infinite",
                                pointerEvents: "none",
                            }}
                        />

                        <div style={{ position: "relative", maxWidth: PANEL_MAX_WIDTH }}>
                            <div
                                style={{
                                    display: "inline-flex",
                                    alignItems: "center",
                                    gap: 10,
                                    padding: "8px 14px",
                                    borderRadius: 999,
                                    border: "1px solid rgba(34, 197, 94, 0.32)",
                                    background: "rgba(6, 95, 70, 0.24)",
                                    color: "#a7f3d0",
                                    fontSize: tokens.font.size.xs,
                                    fontWeight: tokens.font.weight.semibold,
                                    letterSpacing: "0.12em",
                                    textTransform: "uppercase",
                                }}
                            >
                                Keycloak login
                            </div>

                            <h1
                                style={{
                                    margin: "18px 0 12px",
                                    fontSize: "clamp(2.6rem, 6vw, 5rem)",
                                    lineHeight: 0.96,
                                    letterSpacing: "-0.05em",
                                    maxWidth: 11,
                                }}
                            >
                                Enter the Banana viewport.
                            </h1>

                            <p
                                style={{
                                    margin: 0,
                                    maxWidth: 720,
                                    fontSize: tokens.font.size.lg,
                                    lineHeight: 1.55,
                                    color: tokens.color.text.muted,
                                }}
                            >
                                Sign in with Keycloak and carry a single identity context across the web client and native shell.
                            </p>

                            <div
                                style={{
                                    display: "grid",
                                    gridTemplateColumns: "repeat(3, minmax(0, 1fr))",
                                    gap: 14,
                                    marginTop: 28,
                                }}
                            >
                                {[
                                    ["Shared auth", "One identity authority for desktop and web."],
                                    ["Neon storage", "User state and session rows live in the Neon-backed store."],
                                    ["Fast handoff", "The callback returns a signed JWT for the game viewport."],
                                ].map(([title, body]) => (
                                    <article
                                        key={title}
                                        style={{
                                            borderRadius: 20,
                                            border: "1px solid rgba(148, 163, 184, 0.18)",
                                            background: "rgba(15, 23, 42, 0.42)",
                                            padding: 16,
                                            minHeight: 120,
                                        }}
                                    >
                                        <div
                                            style={{
                                                marginBottom: 10,
                                                color: "#fbbf24",
                                                fontSize: tokens.font.size.sm,
                                                fontWeight: tokens.font.weight.semibold,
                                                letterSpacing: "0.08em",
                                                textTransform: "uppercase",
                                            }}
                                        >
                                            {title}
                                        </div>
                                        <div
                                            style={{
                                                color: "#e2e8f0",
                                                lineHeight: 1.5,
                                                fontSize: tokens.font.size.md,
                                            }}
                                        >
                                            {body}
                                        </div>
                                    </article>
                                ))}
                            </div>
                        </div>
                    </div>

                    <aside
                        style={{
                            border: "1px solid rgba(148, 163, 184, 0.22)",
                            borderRadius: 28,
                            background: "rgba(2, 6, 23, 0.68)",
                            boxShadow: "0 24px 90px rgba(2, 6, 23, 0.38)",
                            padding: 28,
                            display: "flex",
                            flexDirection: "column",
                            gap: 18,
                            justifyContent: "space-between",
                            animation: "loginCardRise 520ms ease-out both",
                        }}
                    >
                        <div>
                            <div
                                style={{
                                    color: tokens.color.text.muted,
                                    fontSize: tokens.font.size.sm,
                                    letterSpacing: "0.14em",
                                    textTransform: "uppercase",
                                }}
                            >
                                Authentication terminal
                            </div>
                            <h2
                                style={{
                                    margin: "12px 0 8px",
                                    fontSize: "2rem",
                                    lineHeight: 1.04,
                                }}
                            >
                                Identity session ready.
                            </h2>
                            <p
                                style={{
                                    margin: 0,
                                    color: tokens.color.text.muted,
                                    lineHeight: 1.6,
                                }}
                            >
                                The login handoff is orchestrated by the API deploy target, so the same entry point can serve the web build and the native shell.
                            </p>
                        </div>

                        {sessionReady ? (
                            <div
                                style={{
                                    borderRadius: 18,
                                    border: "1px solid rgba(34, 197, 94, 0.28)",
                                    background: "rgba(6, 78, 59, 0.3)",
                                    padding: 16,
                                }}
                            >
                                <div style={{ color: "#a7f3d0", fontWeight: tokens.font.weight.semibold }}>
                                    Identity session cached
                                </div>
                                <div style={{ marginTop: 6, color: "#d1fae5", lineHeight: 1.5 }}>
                                    Your signed token is cached in memory for this tab. Launch the game viewport or sign out to swap accounts.
                                </div>
                            </div>
                        ) : (
                            <div
                                style={{
                                    borderRadius: 18,
                                    border: "1px solid rgba(250, 204, 21, 0.22)",
                                    background: "rgba(120, 53, 15, 0.22)",
                                    padding: 16,
                                }}
                            >
                                <div style={{ color: "#fde68a", fontWeight: tokens.font.weight.semibold }}>
                                    Ready to connect
                                </div>
                                <div style={{ marginTop: 6, color: "#fef3c7", lineHeight: 1.5 }}>
                                    Keycloak will hand control back to <span style={{ fontFamily: "monospace" }}>{resolveLoginReturnToUrl()}</span> after verification.
                                </div>
                            </div>
                        )}

                        <div style={{ display: "grid", gap: 12 }}>
                            {sessionReady ? (
                                <button
                                    type="button"
                                    onClick={handleOpenSessionRoom}
                                    style={{
                                        border: "none",
                                        borderRadius: 18,
                                        padding: "16px 18px",
                                        background: "linear-gradient(135deg, #2563eb, #0f766e)",
                                        color: "#ffffff",
                                        fontSize: tokens.font.size.md,
                                        fontWeight: tokens.font.weight.semibold,
                                        cursor: "pointer",
                                        boxShadow: "0 16px 30px rgba(37, 99, 235, 0.28)",
                                    }}
                                >
                                    Open session room
                                </button>
                            ) : null}

                            <button
                                type="button"
                                onClick={() => handleIdentitySignIn("github")}
                                disabled={Boolean(apiBaseError) || status === "redirecting"}
                                style={{
                                    border: "none",
                                    borderRadius: 18,
                                    padding: "16px 18px",
                                    background:
                                        status === "redirecting"
                                            ? "linear-gradient(135deg, #334155, #1f2937)"
                                            : "linear-gradient(135deg, #22c55e, #0f766e)",
                                    color: "#ffffff",
                                    fontSize: tokens.font.size.md,
                                    fontWeight: tokens.font.weight.semibold,
                                    cursor: Boolean(apiBaseError) || status === "redirecting" ? "not-allowed" : "pointer",
                                    boxShadow: "0 16px 30px rgba(15, 118, 110, 0.28)",
                                }}
                            >
                                {status === "redirecting" ? "Redirecting to GitHub..." : "Continue with GitHub"}
                            </button>

                            <button
                                type="button"
                                onClick={() => handleIdentitySignIn("google")}
                                disabled={Boolean(apiBaseError) || status === "redirecting"}
                                style={{
                                    borderRadius: 18,
                                    padding: "14px 18px",
                                    background: "transparent",
                                    color: "#ffffff",
                                    border: "1px solid rgba(148, 163, 184, 0.26)",
                                    cursor: Boolean(apiBaseError) || status === "redirecting" ? "not-allowed" : "pointer",
                                }}
                            >
                                Continue with Google
                            </button>

                            <button
                                type="button"
                                onClick={() => handleIdentitySignIn("linkedin")}
                                disabled={Boolean(apiBaseError) || status === "redirecting"}
                                style={{
                                    borderRadius: 18,
                                    padding: "14px 18px",
                                    background: "transparent",
                                    color: "#ffffff",
                                    border: "1px solid rgba(148, 163, 184, 0.26)",
                                    cursor: Boolean(apiBaseError) || status === "redirecting" ? "not-allowed" : "pointer",
                                }}
                            >
                                Continue with LinkedIn
                            </button>

                            <button
                                type="button"
                                onClick={() => handleIdentitySignIn()}
                                disabled={Boolean(apiBaseError) || status === "redirecting"}
                                style={{
                                    borderRadius: 18,
                                    padding: "14px 18px",
                                    background: "transparent",
                                    color: tokens.color.text.muted,
                                    border: "1px solid rgba(148, 163, 184, 0.26)",
                                    cursor: Boolean(apiBaseError) || status === "redirecting" ? "not-allowed" : "pointer",
                                }}
                            >
                                Continue with Keycloak
                            </button>

                            <button
                                type="button"
                                onClick={handleSignOut}
                                style={{
                                    borderRadius: 18,
                                    padding: "14px 18px",
                                    background: "transparent",
                                    color: tokens.color.text.muted,
                                    border: "1px solid rgba(148, 163, 184, 0.26)",
                                    cursor: "pointer",
                                }}
                            >
                                Sign out locally
                            </button>

                            {apiBaseError && <ErrorText>{apiBaseError}</ErrorText>}
                            {error && <ErrorText>{error}</ErrorText>}
                        </div>

                        <div
                            style={{
                                borderTop: "1px solid rgba(148, 163, 184, 0.16)",
                                paddingTop: 16,
                                color: tokens.color.text.muted,
                                lineHeight: 1.5,
                                fontSize: tokens.font.size.sm,
                            }}
                        >
                            The API deployment owns the auth redirect. Neon stores the player record while the Keycloak callback mints the short-lived game token.
                        </div>
                    </aside>
                </section>
            </div>
        </main>
    );
}
