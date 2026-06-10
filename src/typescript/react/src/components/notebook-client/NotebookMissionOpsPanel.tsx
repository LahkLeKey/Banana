import { RouteStatTile, RouteSubActionBar } from '@banana/ui';

type NotebookMissionOpsPanelProps = {
    selectedFile: string;
    indexedFileCount: number;
    notebookCellCount: number;
};

function deriveThreatIndex(selectedFile: string): string {
    if (!selectedFile) {
        return 'Low';
    }

    const normalized = selectedFile.toLowerCase();
    if (normalized.includes('/runtime/') || normalized.includes('/world/')) {
        return 'High';
    }

    if (normalized.includes('/render/') || normalized.includes('/physics/')) {
        return 'Medium';
    }

    return 'Low';
}

export function NotebookMissionOpsPanel(
    { selectedFile, indexedFileCount, notebookCellCount }: NotebookMissionOpsPanelProps) {
    const threatIndex = deriveThreatIndex(selectedFile);

    return (
        <div style={{ display: 'flex', flexDirection: 'column', gap: 0, minHeight: 0 }}>
            <RouteSubActionBar
                meta={`Risk ${threatIndex} · ${indexedFileCount} sectors · ${notebookCellCount} cells`}
            />
            <div style={{ padding: '8px 10px', display: 'grid', gap: 8 }}>
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(2, 1fr)', gap: 8 }}>
                    <RouteStatTile label="Threat" value={threatIndex} accent={threatIndex === 'High' ? '#fca5a5' : threatIndex === 'Medium' ? '#fcd34d' : '#86efac'} />
                    <RouteStatTile label="Sectors" value={indexedFileCount} />
                    <RouteStatTile label="Cells" value={notebookCellCount} />
                    <RouteStatTile label="Overlay" value="Active" accent="#5eead4" />
                </div>
                <div style={{ fontSize: 11, color: '#67e8f9', textTransform: 'uppercase', letterSpacing: '0.06em', marginTop: 2 }}>Sector Focus</div>
                <div style={{ fontSize: 12, color: '#cbd5e1', overflowWrap: 'anywhere', wordBreak: 'break-word' }}>
                    {selectedFile || 'No sector selected'}
                </div>
            </div>
        </div>
    );
}
