import { RouteActionButton, RouteActionsRow, RouteBody, RouteEyebrow, RoutePanel, RouteShell, RouteTitle } from "@banana/ui";
import { useNavigate } from "react-router-dom";

export function SplashPage() {
    const navigate = useNavigate();

    return (
        <RouteShell background="radial-gradient(circle at 20% 20%, rgba(16, 185, 129, 0.24), transparent 35%), linear-gradient(155deg, #03060f, #09162a 60%, #0c2136)">
            <RoutePanel width="min(960px, 100%)">
                <RouteEyebrow color="#5eead4">Boot Sequence</RouteEyebrow>
                <RouteTitle>Banana Engine</RouteTitle>
                <RouteBody maxWidth={780}>
                    Welcome to the notebook-rendered gameplay client. This staged entry flow now mirrors a coherent game startup path before entering live notebooks.
                </RouteBody>
                <RouteActionsRow marginTop={20}>
                    <RouteActionButton onClick={() => navigate("/banana-engine")} tone="primary">
                        Enter Banana Engine
                    </RouteActionButton>
                </RouteActionsRow>
            </RoutePanel>
        </RouteShell>
    );
}
