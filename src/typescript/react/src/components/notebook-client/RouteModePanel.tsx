import { ActionLink, Pill, SurfaceCard } from './SurfacePrimitives';
import { routeModeDescriptors, type NotebookRouteMode } from '../../lib/notebook-client/routeModeConfig';

export function RouteModePanel({ mode }: { mode: NotebookRouteMode; }) {
    const descriptor = routeModeDescriptors[mode];

    return (
        <SurfaceCard title={descriptor.title} description={descriptor.description}>
            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8, marginBottom: 12 }}>
                <Pill color="#67e8f9" borderColor="rgba(45, 212, 191, 0.4)">{descriptor.badge}</Pill>
                <Pill color="#93c5fd" borderColor="rgba(148, 163, 184, 0.35)">Single-Page Route State</Pill>
            </div>

            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 10 }}>
                {descriptor.actions.map((action) => (
                    <ActionLink key={action.href + action.label} href={action.href} emphasis={action.primary ? 'primary' : 'secondary'}>
                        {action.label}
                    </ActionLink>
                ))}
            </div>
        </SurfaceCard>
    );
}
