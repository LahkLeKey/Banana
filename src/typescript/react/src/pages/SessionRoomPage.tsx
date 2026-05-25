import { SessionRoomPage as SharedSessionRoomPage } from "@banana/ui";
import type { LaunchGateMode } from "../lib/launchGateTypes";

type SessionRoomPageProps = {
    launchModeOverride?: LaunchGateMode;
    renderSharedPage?: boolean;
};

function resolveLaunchMode(): LaunchGateMode {
    const importMeta = import.meta as {
        env?: { VITE_BANANA_LAUNCH_MODE?: string };
    };
    const raw = importMeta.env?.VITE_BANANA_LAUNCH_MODE?.trim().toLowerCase() ?? "";
    if (raw === "development" || raw === "staging-uat" || raw === "production-steam-package") {
        return raw;
    }
    return "production-steam-package";
}

/**
 * Compatibility wrapper for the shared local-client session room menu.
 */
export function SessionRoomPage({
    launchModeOverride,
    renderSharedPage = true,
}: SessionRoomPageProps) {
    const mode = launchModeOverride ?? resolveLaunchMode();
    const showDisclosure = mode !== "production-steam-package";

    return (
        <>
            {showDisclosure ? (
                <aside
                    role="status"
                    aria-label="Launch mode disclosure"
                    className="border-b border-amber-400/40 bg-amber-100 px-4 py-2 text-xs text-amber-900"
                >
                    Launch mode: {mode}. Production Steam launch gating is not fully enforced in this mode.
                </aside>
            ) : null}
            {renderSharedPage ? <SharedSessionRoomPage /> : null}
        </>
    );
}
