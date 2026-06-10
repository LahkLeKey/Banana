import { RouteActionButton, RouteActionsRow, RouteBody, RouteEyebrow, RouteFieldLabel, RoutePanel, RouteShell, RouteTextInput, RouteTitle, readStoredAuthSession, storeAuthSession } from "@banana/ui";
import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { resolveApiBaseResolutionError, resolveApiBaseUrl, startGuestAuthSession } from "../lib/api";

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
        <RouteShell background="radial-gradient(circle at 18% 18%, rgba(20, 184, 166, 0.2), transparent 34%), linear-gradient(155deg, #020711, #081629 56%, #0f233b)">
            <RoutePanel width="min(920px, 100%)">
                <RouteEyebrow color="#5eead4">Authentication Relay</RouteEyebrow>
                <RouteTitle>Guest Login Enabled</RouteTitle>
                <RouteBody>
                    Steam sign-in is not required in this phase. Enter as guest and continue through main menu and character selection.
                </RouteBody>

                <RouteFieldLabel>Operator Alias</RouteFieldLabel>
                <RouteTextInput value={alias} onChange={setAlias} maxLength={24} />

                {errorMessage ? <p style={{ margin: "12px 0 0", color: "#fca5a5" }}>{errorMessage}</p> : null}

                <RouteActionsRow marginTop={18}>
                    <RouteActionButton onClick={handleGuestLogin} disabled={loading} tone="primary">
                        {loading ? "Starting guest session..." : "Enter as Guest"}
                    </RouteActionButton>
                </RouteActionsRow>
            </RoutePanel>
        </RouteShell>
    );
}
