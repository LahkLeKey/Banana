import {
    RouteActionLink,
    RouteActionsRow,
    RouteBody,
    RouteEyebrow,
    RoutePanel,
    RouteShell,
    RouteTitle,
} from '@banana/ui';

export function MarketingPage() {
    return (
        <RouteShell background="radial-gradient(circle at 10% 10%, rgba(16, 185, 129, 0.16), transparent 32%), radial-gradient(circle at 92% 18%, rgba(245, 158, 11, 0.2), transparent 30%), linear-gradient(140deg, #05070f 0%, #0b1324 55%, #121d37 100%)">
            <RoutePanel>
                <RouteEyebrow>Banana Engineer</RouteEyebrow>
                <RouteTitle>Steam Windows build is the primary deliverable.</RouteTitle>
                <RouteBody>
                    The website is now a marketing channel. Download and launch the desktop client from Steam to access the full DirectX12 runtime. Steam sign-in and account session state are orchestrated through the new Neon-backed auth store.
                </RouteBody>

                <RouteActionsRow>
                    <RouteActionLink href="https://store.steampowered.com/" target="_blank" rel="noreferrer" tone="primary">
                        Download on Steam
                    </RouteActionLink>
                    <RouteActionLink href="/login" tone="neutral">
                        Steam account login
                    </RouteActionLink>
                    <RouteActionLink href="/download" tone="emerald">
                        View account and gameplay stats
                    </RouteActionLink>
                </RouteActionsRow>
            </RoutePanel>
        </RouteShell>
    );
}
