import type { ReactNode } from 'react';

import { RouteActionLink, RouteActionsRow, RouteBody, RouteEyebrow, RoutePanel, RouteShell } from './RouteScaffold';

export type RouteDescriptorAction = {
    readonly href: string;
    readonly label: string;
    readonly primary?: boolean;
};

type RouteModeDescriptorCardProps = {
    readonly title: string;
    readonly description: string;
    readonly badge: string;
    readonly actions: RouteDescriptorAction[];
    readonly secondaryBadge?: string;
    readonly wrapper?: (children: ReactNode) => ReactNode;
};

export function RouteModeDescriptorCard({
    title,
    description,
    badge,
    actions,
    secondaryBadge = 'Single-Page Route State',
    wrapper,
}: RouteModeDescriptorCardProps) {
    const content = (
        <>
            <RouteEyebrow color="#67e8f9">{badge}</RouteEyebrow>
            <RouteBody>{description}</RouteBody>
            <div style={{ marginTop: 8 }}>
                <RouteEyebrow color="#93c5fd">{secondaryBadge}</RouteEyebrow>
            </div>
            <RouteActionsRow marginTop={12}>
                {actions.map((action) => (
                    <RouteActionLink key={`${action.href}:${action.label}`} href={action.href} tone={action.primary ? 'primary' : 'neutral'}>
                        {action.label}
                    </RouteActionLink>
                ))}
            </RouteActionsRow>
        </>
    );

    if (wrapper) {
        return <>{wrapper(content)}</>;
    }

    return (
        <RouteShell background="transparent">
            <RoutePanel width="min(100%, 720px)">
                <h2 style={{ margin: '0 0 8px', fontSize: 18, letterSpacing: '0.04em', textTransform: 'uppercase' }}>{title}</h2>
                {content}
            </RoutePanel>
        </RouteShell>
    );
}
