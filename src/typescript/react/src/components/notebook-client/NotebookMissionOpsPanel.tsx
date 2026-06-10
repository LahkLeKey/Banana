import { SurfaceCard } from './SurfacePrimitives';

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
        <SurfaceCard
            title="Mission Ops"
            description="Overlay controls and runtime posture for the currently selected sector."
        >
            <div style={{
                display: 'grid',
                gap: 8,
                gridTemplateColumns: 'repeat(2, minmax(120px, 1fr))',
            }}>
                <div style={{
                    borderRadius: 10,
                    border: '1px solid rgba(148, 163, 184, 0.25)',
                    background: 'rgba(8, 13, 28, 0.62)',
                    padding: 10,
                }}>
                    <div style={{ fontSize: 11, color: '#93c5fd', textTransform: 'uppercase' }}>Threat</div>
                    <div style={{ marginTop: 4, fontWeight: 700 }}>{threatIndex}</div>
                </div>
                <div style={{
                    borderRadius: 10,
                    border: '1px solid rgba(148, 163, 184, 0.25)',
                    background: 'rgba(8, 13, 28, 0.62)',
                    padding: 10,
                }}>
                    <div style={{ fontSize: 11, color: '#93c5fd', textTransform: 'uppercase' }}>Sectors</div>
                    <div style={{ marginTop: 4, fontWeight: 700 }}>{indexedFileCount}</div>
                </div>
                <div style={{
                    borderRadius: 10,
                    border: '1px solid rgba(148, 163, 184, 0.25)',
                    background: 'rgba(8, 13, 28, 0.62)',
                    padding: 10,
                }}>
                    <div style={{ fontSize: 11, color: '#93c5fd', textTransform: 'uppercase' }}>Cells</div>
                    <div style={{ marginTop: 4, fontWeight: 700 }}>{notebookCellCount}</div>
                </div>
                <div style={{
                    borderRadius: 10,
                    border: '1px solid rgba(148, 163, 184, 0.25)',
                    background: 'rgba(8, 13, 28, 0.62)',
                    padding: 10,
                }}>
                    <div style={{ fontSize: 11, color: '#93c5fd', textTransform: 'uppercase' }}>Overlay</div>
                    <div style={{ marginTop: 4, fontWeight: 700 }}>Enabled</div>
                </div>
            </div>

            <div style={{
                marginTop: 10,
                borderRadius: 10,
                border: '1px solid rgba(45, 212, 191, 0.2)',
                background: 'rgba(2, 8, 18, 0.58)',
                padding: 10,
            }}>
                <div style={{ fontSize: 11, color: '#67e8f9', textTransform: 'uppercase' }}>Sector Focus</div>
                <div style={{
                    marginTop: 6,
                    fontSize: 12,
                    color: '#cbd5e1',
                    overflowWrap: 'anywhere',
                    wordBreak: 'break-word',
                    minHeight: 34,
                }}>
                    {selectedFile || 'No sector selected'}
                </div>
            </div>
        </SurfaceCard>
    );
}
