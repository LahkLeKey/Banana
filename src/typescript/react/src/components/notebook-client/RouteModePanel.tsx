import { RouteModeDescriptorCard } from '@banana/ui';
import { routeModeDescriptors, type NotebookRouteMode } from '../../lib/notebook-client/routeModeConfig';

export function RouteModePanel({ mode }: { mode: NotebookRouteMode; }) {
    const descriptor = routeModeDescriptors[mode];

    return (
        <RouteModeDescriptorCard
            title={descriptor.title}
            description={descriptor.description}
            badge={descriptor.badge}
            actions={descriptor.actions}
            wrapper={(children) => (
                <article style={{
                    borderRadius: 18,
                    border: '1px solid rgba(45, 212, 191, 0.26)',
                    background: 'linear-gradient(170deg, rgba(8, 21, 33, 0.92), rgba(4, 11, 22, 0.96))',
                    padding: 18,
                    minWidth: 0,
                    boxShadow: 'inset 0 1px 0 rgba(148, 163, 184, 0.08)',
                }}>
                    <h2 style={{ margin: '0 0 8px', fontSize: 18, letterSpacing: '0.04em', textTransform: 'uppercase' }}>
                        {descriptor.title}
                    </h2>
                    {children}
                </article>
            )}
        />
    );
}
