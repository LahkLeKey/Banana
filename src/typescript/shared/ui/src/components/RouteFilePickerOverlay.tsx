import { useState, type CSSProperties } from 'react';

type RouteFilePickerOverlayProps = {
    readonly files: string[];
    readonly recentFiles: string[];
    readonly selected: string;
    readonly title?: string;
    readonly placeholder?: string;
    readonly emptyLabel?: string;
    readonly allItemsLabel?: string;
    readonly onSelect: (file: string) => void;
    readonly onClose: () => void;
};

export function RouteFilePickerOverlay(
    {
        files,
        recentFiles,
        selected,
        title = 'Sector Select',
        placeholder = 'Filter sectors...',
        emptyLabel = 'No sectors match',
        allItemsLabel = 'All Sectors',
        onSelect,
        onClose,
    }: RouteFilePickerOverlayProps,
) {
    const [query, setQuery] = useState('');
    const normalizedQuery = query.trim().toLowerCase();
    const matchedFiles = normalizedQuery
        ? files.filter((file) => file.toLowerCase().includes(normalizedQuery))
        : files;
    const showRecent = recentFiles.length > 0 && !normalizedQuery;

    const rowStyle = (isSelected: boolean): CSSProperties => ({
        padding: '6px 10px',
        borderRadius: 8,
        cursor: 'pointer',
        fontSize: 12,
        fontFamily: '"IBM Plex Mono", monospace',
        color: isSelected ? '#67e8f9' : '#cbd5e1',
        background: isSelected ? 'rgba(45, 212, 191, 0.14)' : 'transparent',
        border: isSelected ? '1px solid rgba(45, 212, 191, 0.3)' : '1px solid transparent',
        whiteSpace: 'nowrap',
        overflow: 'hidden',
        textOverflow: 'ellipsis',
        display: 'block',
        width: '100%',
        textAlign: 'left',
    });

    return (
        <div style={{
            position: 'absolute',
            bottom: 72,
            left: '50%',
            transform: 'translateX(-50%)',
            width: 'min(620px, calc(100vw - 32px))',
            maxHeight: 'calc(54dvh - 80px)',
            zIndex: 20,
            borderRadius: 18,
            border: '1px solid rgba(45, 212, 191, 0.38)',
            background: 'linear-gradient(170deg, rgba(2, 10, 22, 0.97), rgba(1, 7, 16, 0.98))',
            boxShadow: '0 -16px 48px rgba(2, 6, 23, 0.7)',
            display: 'flex',
            flexDirection: 'column',
            overflow: 'hidden',
        }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: 8, padding: '10px 12px 8px', borderBottom: '1px solid rgba(45, 212, 191, 0.18)', flexShrink: 0 }}>
                <span style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.1em', color: '#67e8f9', flexShrink: 0 }}>{title}</span>
                <input
                    type="text"
                    value={query}
                    onChange={(event) => setQuery(event.target.value)}
                    placeholder={placeholder}
                    autoFocus
                    style={{
                        flex: 1,
                        borderRadius: 8,
                        border: '1px solid rgba(45, 212, 191, 0.3)',
                        background: 'rgba(6, 15, 26, 0.9)',
                        color: '#e2e8f0',
                        padding: '5px 9px',
                        fontSize: 12,
                        fontFamily: '"IBM Plex Mono", monospace',
                        outline: 'none',
                    }}
                />
                <button type="button" onClick={onClose} style={{ borderRadius: 999, border: '1px solid rgba(148, 163, 184, 0.3)', background: 'rgba(8, 13, 28, 0.5)', color: '#94a3b8', padding: '4px 9px', cursor: 'pointer', fontSize: 11, flexShrink: 0 }}>✕</button>
            </div>
            <div style={{ flex: 1, overflow: 'auto', padding: '6px 8px' }}>
                {showRecent ? (
                    <>
                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#5eead4', padding: '4px 4px 6px', opacity: 0.8 }}>Recent</div>
                        {recentFiles.map((file) => (
                            <button key={`r-${file}`} type="button" style={rowStyle(file === selected)} onClick={() => onSelect(file)}>
                                ◈ {file}
                            </button>
                        ))}
                        <div style={{ margin: '6px 0', borderTop: '1px solid rgba(45, 212, 191, 0.12)' }} />
                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#94a3b8', padding: '2px 4px 6px', opacity: 0.7 }}>
                            {allItemsLabel} ({files.length})
                        </div>
                    </>
                ) : null}
                {matchedFiles.map((file) => (
                    <button key={file} type="button" style={rowStyle(file === selected)} onClick={() => onSelect(file)}>
                        {file}
                    </button>
                ))}
                {matchedFiles.length === 0 ? (
                    <div style={{ padding: '16px 10px', textAlign: 'center', color: '#64748b', fontSize: 12 }}>{emptyLabel}</div>
                ) : null}
            </div>
        </div>
    );
}
