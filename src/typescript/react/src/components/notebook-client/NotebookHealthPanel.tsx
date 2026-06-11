import { EnhancedPanel } from '@banana/ui';

type NotebookHealthPanelProps = {
    readonly loading: boolean;
    readonly manifestError: string | null;
    readonly notebookError: string | null;
    readonly manifestSource: string;
    readonly notebookSource: string;
    readonly embedded?: boolean;
};

export function NotebookHealthPanel({ loading, manifestError, notebookError, manifestSource, notebookSource, embedded = false }: NotebookHealthPanelProps) {
    const status = loading ? 'loading' : (manifestError || notebookError) ? 'error' : 'success';
    const statusMessage = loading
        ? 'Syncing manifest and notebook payload...'
        : (manifestError || notebookError)
            ? (manifestError || notebookError)
            : 'Transport healthy';

    const sourceInfo = manifestSource.length > 0 || notebookSource.length > 0
        ? [
            manifestSource.length > 0 ? `Manifest: ${manifestSource}` : null,
            notebookSource.length > 0 ? `Notebook: ${notebookSource}` : null,
        ].filter(Boolean).join(' • ')
        : '';

    const sections = [
        {
            title: 'Status',
            content: (
                <div style={{ fontSize: '12px', lineHeight: '1.6', color: 'rgba(226, 232, 240, 0.85)' }}>
                    {loading ? 'Syncing manifest and notebook payload...' : null}
                    {!loading && !manifestError && !notebookError ? (
                        'Transport healthy. Mission dataset is available and API-first fallback routing is operational.'
                    ) : null}
                </div>
            ),
        },
        ...(sourceInfo.length > 0 ? [{
            title: 'Source',
            content: (
                <div style={{ fontSize: '11px', color: 'rgba(148, 197, 253, 0.8)', wordBreak: 'break-all' }}>
                    {sourceInfo}
                </div>
            ),
        }] : []),
    ];

    if (embedded) {
        return (
            <div style={{ fontSize: '12px', lineHeight: '1.6', color: 'rgba(226, 232, 240, 0.85)' }}>
                {sections.map((section, idx) => (
                    <div key={idx} style={{ marginBottom: '8px' }}>
                        <div style={{ fontSize: '10px', fontWeight: 600, textTransform: 'uppercase', color: 'rgba(226, 232, 240, 0.6)' }}>
                            {section.title}
                        </div>
                        {section.content}
                    </div>
                ))}
            </div>
        );
    }

    return (
        <EnhancedPanel
            title="ROUTE REGISTRY"
            status={status}
            statusMessage={statusMessage || undefined}
            sections={sections}
            padding="8px"
            gap="6px"
        />
    );
}
