import { readStoredAuthSession, storeAuthSession } from "@banana/ui";
import { useEffect, useState, type CSSProperties } from "react";
import { useNavigate } from "react-router-dom";
import { resolveApiBaseResolutionError, resolveApiBaseUrl, startGuestAuthSession } from "../lib/api";

const shellStyle: CSSProperties = {
    minHeight: "100dvh",
    display: "grid",
    placeItems: "center",
    background: "radial-gradient(circle at 18% 18%, rgba(20, 184, 166, 0.2), transparent 34%), linear-gradient(155deg, #020711, #081629 56%, #0f233b)",
    color: "#e2e8f0",
    fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
    padding: 24,
};

const panelStyle: CSSProperties = {
    width: "min(920px, 100%)",
    borderRadius: 24,
    border: "1px solid rgba(45, 212, 191, 0.35)",
    background: "linear-gradient(170deg, rgba(7, 22, 37, 0.9), rgba(4, 10, 19, 0.96))",
    padding: "30px clamp(18px, 3vw, 36px)",
};

function createLocalGuestSession(alias: string): { token: string; steamId: string } {
    const suffix = Math.random().toString(16).slice(2, 10);
    const guestId = `local-${suffix}`;
    const normalizedAlias = alias.trim().length > 0 ? alias.trim() : "Guest";

    return {
        token: `guest-local-token-${guestId}`,
        steamId: `guest:${normalizedAlias}:${guestId}`,
    };
}

export function LoginPage() {
    const navigate = useNavigate();
    const [alias, setAlias] = useState("Guest");
    const [loading, setLoading] = useState(false);
    const [errorMessage, setErrorMessage] = useState<string | null>(null);

    const apiBaseError = resolveApiBaseResolutionError();
    const apiBaseUrl = resolveApiBaseUrl();

    useEffect(() => {
        if (readStoredAuthSession()) {
            navigate("/game-main-menu", { replace: true });
        }
    }, [navigate]);

    const handleGuestLogin = async () => {
        setLoading(true);
        setErrorMessage(null);

        const requestedAlias = alias.trim() || "Guest";

        try {
            if (apiBaseUrl.trim().length > 0) {
                const session = await startGuestAuthSession(apiBaseUrl, requestedAlias);
                storeAuthSession({
                    token: session.token,
                    steamId: `guest:${session.guestId}`,
                });
            } else {
                storeAuthSession(createLocalGuestSession(requestedAlias));
            }

            navigate("/game-main-menu", { replace: true });
        } catch {
            storeAuthSession(createLocalGuestSession(requestedAlias));
            navigate("/game-main-menu", { replace: true });
        } finally {
            setLoading(false);
        }
    };

    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <p style={{ margin: 0, letterSpacing: "0.12em", textTransform: "uppercase", color: "#5eead4", fontWeight: 700 }}>Authentication Relay</p>
                <h1 style={{ margin: "10px 0 12px", fontSize: "clamp(2rem, 5vw, 3.2rem)", lineHeight: 1.1 }}>Guest Login Enabled</h1>
                <p style={{ margin: 0, color: "#cbd5e1", lineHeight: 1.7 }}>
                    Steam sign-in is not required in this phase. Enter as guest and continue through main menu and character selection.
                </p>

                <label style={{ display: "block", marginTop: 18, color: "#cbd5e1", fontWeight: 700 }}>
                    Operator Alias
                </label>
                <input
                    type="text"
                    value={alias}
                    onChange={(event) => setAlias(event.target.value)}
                    maxLength={24}
                    style={{
                        marginTop: 8,
                        width: "min(420px, 100%)",
                        borderRadius: 10,
                        border: "1px solid rgba(45, 212, 191, 0.35)",
                        background: "rgba(6, 15, 26, 0.9)",
                        color: "#e2e8f0",
                        padding: "11px 12px",
                    }}
                />

                {errorMessage ? <p style={{ margin: "12px 0 0", color: "#fca5a5" }}>{errorMessage}</p> : null}

                <div style={{ marginTop: 18, display: "flex", flexWrap: "wrap", gap: 12 }}>
                    <button
                        type="button"
                        onClick={handleGuestLogin}
                        disabled={loading}
                        style={{
                            borderRadius: 12,
                            border: "1px solid rgba(20, 184, 166, 0.7)",
                            background: "linear-gradient(135deg, #0f766e, #0e7490)",
                            color: "#f8fafc",
                            fontWeight: 700,
                            padding: "12px 18px",
                            cursor: loading ? "progress" : "pointer",
                            opacity: loading ? 0.8 : 1,
                        }}
                    >
                        {loading ? "Starting guest session..." : "Enter as Guest"}
                    </button>
                </div>
            </section>
        </main>
    );
}
