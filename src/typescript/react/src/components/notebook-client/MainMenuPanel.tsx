import type { CSSProperties } from 'react';

import { ActionLink, StatTile, SurfaceCard } from './SurfacePrimitives';

type MainMenuPanelProps = {
    readonly indexedFileCount: number;
    readonly notebookCellCount: number;
    readonly manifestAvailable: boolean;
};

const headingStyle: CSSProperties = {
    margin: '8px 0 12px',
    fontSize: 'clamp(2rem, 4.6vw, 3.4rem)',
    lineHeight: 1,
    fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
};

export function MainMenuPanel({ indexedFileCount, notebookCellCount, manifestAvailable }: MainMenuPanelProps) {
    return (
        <SurfaceCard
            title="Banana Command Uplink"
            description="Notebook-driven world simulation client. Source Explorer remains the primary workspace for navigating runtime sectors."
        >
            <h1 style={headingStyle}>Overworld Runtime Bridge</h1>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(170px, 1fr))', gap: 12 }}>
                <StatTile label="Map Sectors" value={indexedFileCount} />
                <StatTile label="Runtime Cells" value={notebookCellCount} />
                <StatTile
                    label="Connection"
                    value={manifestAvailable ? 'Synchronized' : 'Awaiting Sync'}
                    accent={manifestAvailable ? '#86efac' : '#fca5a5'}
                />
            </div>

            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 10 }}>
                <ActionLink href="/notebooks/native-c-catalog.ipynb" emphasis="primary">
                    Download Notebook Payload
                </ActionLink>
                <ActionLink href="/notebooks/catalog-index.json">Open Notebook Manifest</ActionLink>
                <ActionLink href="/marketing">Legacy Marketing</ActionLink>
                <ActionLink href="/login">Account Login</ActionLink>
            </div>
        </SurfaceCard>
    );
}
