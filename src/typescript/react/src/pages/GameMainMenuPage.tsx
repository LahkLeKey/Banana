import { RouteActionButton, RouteActionsRow, RouteBody, RouteEyebrow, RouteMetaText, RoutePanel, RouteShell, RouteTitle, clearStoredAuthSession, readStoredAuthSession } from "@banana/ui";
import { useNavigate } from "react-router-dom";
import { clearSelectedCharacter } from "../lib/gameClientFlow";

export function GameMainMenuPage() {
    const navigate = useNavigate();
    const session = readStoredAuthSession();

    const handleSignOut = () => {
        clearStoredAuthSession();
        clearSelectedCharacter();
        navigate("/login", { replace: true });
    };

    return (
        <RouteShell background="radial-gradient(circle at 15% 14%, rgba(20, 184, 166, 0.2), transparent 30%), linear-gradient(155deg, #030712, #0a1628 55%, #0f243c)">
            <RoutePanel width="min(1020px, 100%)">
                <RouteEyebrow color="#5eead4">Game Main Menu</RouteEyebrow>
                <RouteTitle>Operation Notebook Tide</RouteTitle>
                <RouteBody>
                    Active operator token detected. Proceed to character selection before entering notebook sectors.
                </RouteBody>
                <RouteMetaText>Session: {session?.subject || "guest"}</RouteMetaText>

                <RouteActionsRow marginTop={20}>
                    <RouteActionButton onClick={() => navigate("/select-character")} tone="primary">
                        Select Character
                    </RouteActionButton>
                    <RouteActionButton onClick={() => navigate("/notebooks")} tone="neutral">
                        Enter Notebooks
                    </RouteActionButton>
                    <RouteActionButton onClick={handleSignOut} tone="danger">
                        Sign Out
                    </RouteActionButton>
                </RouteActionsRow>
            </RoutePanel>
        </RouteShell>
    );
}
