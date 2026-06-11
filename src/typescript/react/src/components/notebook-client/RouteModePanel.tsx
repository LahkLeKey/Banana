import { PanelBase } from '@banana/ui';
import { routeModeDescriptors, type NotebookRouteMode } from '../../lib/notebook-client/routeModeConfig';

export function RouteModePanel({ mode }: { mode: NotebookRouteMode; }) {
    const descriptor = routeModeDescriptors[mode];

    return (
        <PanelBase title={descriptor.title} variant="default">
            <div style={{ fontSize: 14, color: '#e2e8f0', lineHeight: 1.6, marginBottom: 14 }}>
                {descriptor.description}
            </div>
            <div style={{ display: 'flex', gap: 8, flexWrap: 'wrap' }}>
                {descriptor.actions.map((action) => (
                    <a
                        key={action.href}
                        href={action.href}
                        style={{
                            borderRadius: 8,
                            border: `1px solid ${action.primary ? 'rgba(45, 212, 191, 0.45)' : 'rgba(148, 163, 184, 0.2)'}`,
                            background: action.primary ? 'rgba(45, 212, 191, 0.12)' : 'rgba(15, 23, 42, 0.6)',
                            color: action.primary ? '#5eead4' : '#cbd5e1',
                            padding: '8px 12px',
                            fontSize: 11,
                            fontWeight: action.primary ? 700 : 500,
                            textTransform: 'uppercase',
                            letterSpacing: '0.03em',
                            textDecoration: 'none',
                            display: 'inline-block',
                            transition: 'all 0.2s ease',
                            cursor: 'pointer',
                        }}
                        onMouseEnter={(e) => {
                            e.currentTarget.style.background = action.primary ? 'rgba(45, 212, 191, 0.2)' : 'rgba(45, 212, 191, 0.08)';
                            e.currentTarget.style.borderColor = action.primary ? 'rgba(45, 212, 191, 0.6)' : 'rgba(45, 212, 191, 0.3)';
                        }}
                        onMouseLeave={(e) => {
                            e.currentTarget.style.background = action.primary ? 'rgba(45, 212, 191, 0.12)' : 'rgba(15, 23, 42, 0.6)';
                            e.currentTarget.style.borderColor = action.primary ? 'rgba(45, 212, 191, 0.45)' : 'rgba(148, 163, 184, 0.2)';
                        }}
                    >
                        {action.label}
                    </a>
                ))}
            </div>
        </PanelBase>
    );
}
