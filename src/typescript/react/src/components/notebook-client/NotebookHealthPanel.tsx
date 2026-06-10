import { RouteSubActionBar } from '@banana/ui';

type NotebookHealthPanelProps = {
    readonly loading: boolean;
    readonly manifestError: string | null;
    readonly notebookError: string | null;
    readonly manifestSource: string;
    readonly notebookSource: string;
    readonly embedded?: boolean;
};

export function NotebookHealthPanel({ loading, manifestError, notebookError, manifestSource, notebookSource, embedded = false }: NotebookHealthPanelProps) {
    const content = (
        <>
            {loading ? <p style={{ margin: 0, color: '#cbd5e1' }}>Syncing manifest and notebook payload...</p> : null}
            {manifestError ? <p style={{ margin: 0, color: '#fca5a5' }}>{manifestError}</p> : null}
            {notebookError ? <p style={{ margin: '8px 0 0', color: '#fca5a5' }}>{notebookError}</p> : null}
            {!loading && !manifestError && !notebookError ? (
                <p style={{ margin: '8px 0 0', color: '#cbd5e1', lineHeight: 1.6 }}>
                    Transport healthy. Mission dataset is available and API-first fallback routing is operational.
                </p>
            ) : null}
            {!loading && (manifestSource.length > 0 || notebookSource.length > 0) ? (
                <p style={{ margin: '8px 0 0', color: '#93c5fd', lineHeight: 1.6 }}>
                    {manifestSource.length > 0 ? `Manifest source: ${manifestSource}. ` : ""}
                    {notebookSource.length > 0 ? `Notebook source: ${notebookSource}.` : ""}
                </p>
            ) : null}
        </>
    );

    if (embedded) {
        return content;
    }

    const statusMeta = loading
        ? 'Syncing…'
        : (manifestError || notebookError)
            ? 'Transport error'
            : 'Transport healthy';

    return (
        <div style={{ display: 'flex', flexDirection: 'column', minHeight: 0 }}>
            <RouteSubActionBar meta={statusMeta} />
            <div style={{ padding: '10px 12px' }}>{content}</div>
        </div>
    );
}
