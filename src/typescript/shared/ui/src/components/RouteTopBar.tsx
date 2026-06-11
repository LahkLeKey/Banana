import { useEffect, useMemo, useRef, useState, type ReactNode } from 'react';

type RouteTopBarProps = {
    readonly routeLabel: string;
    readonly showExplorer?: boolean;
    readonly onToggleExplorer?: () => void;
    readonly showExplorerButton?: boolean;
    readonly nodeBrowserFiles?: readonly string[];
    readonly selectedNodeFile?: string;
    readonly onSelectNodeFile?: (file: string) => void;
    readonly showNodeBrowserButton?: boolean;
    readonly onResetSectors: () => void;
    readonly children?: ReactNode;
};

export function RouteTopBar({
    routeLabel,
    showExplorer = false,
    onToggleExplorer,
    showExplorerButton = true,
    nodeBrowserFiles = [],
    selectedNodeFile = '',
    onSelectNodeFile,
    showNodeBrowserButton = true,
    onResetSectors,
    children,
}: RouteTopBarProps) {
    const [nodeBrowserOpen, setNodeBrowserOpen] = useState(false);
    const [nodeBrowserQuery, setNodeBrowserQuery] = useState('');
    const nodeBrowserRef = useRef<HTMLDivElement | null>(null);

    const filteredNodeFiles = useMemo(() => {
        const query = nodeBrowserQuery.trim().toLowerCase();
        if (!query) {
            return nodeBrowserFiles;
        }
        return nodeBrowserFiles.filter((file) => file.toLowerCase().includes(query));
    }, [nodeBrowserFiles, nodeBrowserQuery]);

    useEffect(() => {
        if (!nodeBrowserOpen) {
            return;
        }

        const handlePointerDown = (event: MouseEvent) => {
            if (!nodeBrowserRef.current) {
                return;
            }
            const target = event.target;
            if (target instanceof Node && !nodeBrowserRef.current.contains(target)) {
                setNodeBrowserOpen(false);
            }
        };

        const handleEscape = (event: KeyboardEvent) => {
            if (event.key === 'Escape') {
                setNodeBrowserOpen(false);
            }
        };

        window.addEventListener('mousedown', handlePointerDown);
        window.addEventListener('keydown', handleEscape);

        return () => {
            window.removeEventListener('mousedown', handlePointerDown);
            window.removeEventListener('keydown', handleEscape);
        };
    }, [nodeBrowserOpen]);

    const handleSelectNodeFile = (file: string) => {
        onSelectNodeFile?.(file);
        setNodeBrowserOpen(false);
    };

    return (
        <div style={{
            position: 'absolute',
            top: 0,
            left: 0,
            right: 0,
            height: 40,
            zIndex: 6,
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'space-between',
            padding: '0 12px',
            gap: 10,
            background: 'linear-gradient(180deg, rgba(2, 8, 20, 0.88) 0%, rgba(2, 10, 24, 0.72) 100%)',
            borderBottom: '1px solid rgba(45, 212, 191, 0.14)',
            backdropFilter: 'blur(8px)',
            pointerEvents: 'auto',
        }}>
            {/* Left: route label */}
            <div style={{
                color: '#93c5fd',
                fontSize: 10,
                fontWeight: 700,
                letterSpacing: '0.08em',
                textTransform: 'uppercase',
                whiteSpace: 'nowrap',
                opacity: 0.7,
            }}>
                {routeLabel}
            </div>

            {/* Center: Explorer + sector reset */}
            <div ref={nodeBrowserRef} style={{ display: 'flex', alignItems: 'center', gap: 6, position: 'relative' }}>
                {showNodeBrowserButton ? (
                    <button
                        type="button"
                        onClick={() => setNodeBrowserOpen((current) => !current)}
                        style={{
                            display: 'flex',
                            alignItems: 'center',
                            gap: 6,
                            borderRadius: 8,
                            border: nodeBrowserOpen
                                ? '1px solid rgba(56, 189, 248, 0.58)'
                                : '1px solid rgba(148, 163, 184, 0.22)',
                            background: nodeBrowserOpen
                                ? 'rgba(8, 47, 73, 0.35)'
                                : 'rgba(8, 13, 28, 0.35)',
                            color: nodeBrowserOpen ? '#7dd3fc' : '#94a3b8',
                            padding: '4px 11px',
                            cursor: 'pointer',
                            fontSize: 11,
                            fontWeight: 700,
                            letterSpacing: '0.04em',
                            transition: 'background 0.15s, border-color 0.15s, color 0.15s',
                            whiteSpace: 'nowrap',
                        }}
                    >
                        <span style={{ fontSize: 13, lineHeight: 1 }}>🗂</span>
                        Node Browser
                    </button>
                ) : null}

                {showExplorerButton ? (
                    <button
                        type="button"
                        onClick={onToggleExplorer}
                        style={{
                            display: 'flex',
                            alignItems: 'center',
                            gap: 6,
                            borderRadius: 8,
                            border: showExplorer
                                ? '1px solid rgba(45, 212, 191, 0.55)'
                                : '1px solid rgba(148, 163, 184, 0.22)',
                            background: showExplorer
                                ? 'rgba(13, 148, 136, 0.22)'
                                : 'rgba(8, 13, 28, 0.35)',
                            color: showExplorer ? '#5eead4' : '#94a3b8',
                            padding: '4px 11px',
                            cursor: 'pointer',
                            fontSize: 11,
                            fontWeight: 700,
                            letterSpacing: '0.04em',
                            transition: 'background 0.15s, border-color 0.15s, color 0.15s',
                            whiteSpace: 'nowrap',
                        }}
                    >
                        <span style={{ fontSize: 13, lineHeight: 1 }}>📂</span>
                        File Explorer
                    </button>
                ) : null}

                {nodeBrowserOpen ? (
                    <div style={{
                        position: 'absolute',
                        top: 36,
                        left: 0,
                        width: 'min(520px, calc(100vw - 32px))',
                        maxHeight: '62dvh',
                        borderRadius: 10,
                        border: '1px solid rgba(56, 189, 248, 0.4)',
                        background: 'linear-gradient(180deg, rgba(2, 10, 22, 0.96), rgba(3, 12, 25, 0.97))',
                        boxShadow: '0 16px 28px rgba(2, 6, 23, 0.5)',
                        backdropFilter: 'blur(10px)',
                        padding: 8,
                        display: 'grid',
                        gap: 8,
                        zIndex: 12,
                    }}>
                        <input
                            type="text"
                            value={nodeBrowserQuery}
                            onChange={(event) => setNodeBrowserQuery(event.target.value)}
                            placeholder="Filter nodes by path or name..."
                            style={{
                                width: '100%',
                                boxSizing: 'border-box',
                                borderRadius: 8,
                                border: '1px solid rgba(56, 189, 248, 0.35)',
                                background: 'rgba(8, 13, 28, 0.64)',
                                color: '#e2e8f0',
                                padding: '7px 10px',
                                fontSize: 12,
                                fontFamily: '"IBM Plex Mono", monospace',
                                outline: 'none',
                            }}
                        />
                        <div style={{
                            maxHeight: '50dvh',
                            overflowY: 'auto',
                            borderRadius: 8,
                            border: '1px solid rgba(148, 163, 184, 0.2)',
                            background: 'rgba(8, 13, 28, 0.44)',
                        }}>
                            {filteredNodeFiles.map((file) => {
                                const isSelected = file === selectedNodeFile;
                                const fileName = file.split('/').pop() ?? file;
                                return (
                                    <button
                                        key={file}
                                        type="button"
                                        onClick={() => handleSelectNodeFile(file)}
                                        style={{
                                            width: '100%',
                                            textAlign: 'left',
                                            border: 'none',
                                            borderBottom: '1px solid rgba(148, 163, 184, 0.1)',
                                            background: isSelected ? 'linear-gradient(90deg, rgba(14, 116, 144, 0.46), rgba(2, 22, 44, 0.35))' : 'transparent',
                                            color: isSelected ? '#bae6fd' : '#e2e8f0',
                                            padding: '8px 10px',
                                            display: 'grid',
                                            gap: 3,
                                            cursor: 'pointer',
                                        }}
                                    >
                                        <span style={{ fontSize: 12, fontWeight: 700, overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>{fileName}</span>
                                        <span style={{ fontSize: 10, color: '#93c5fd', fontFamily: '"IBM Plex Mono", monospace', overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>{file}</span>
                                    </button>
                                );
                            })}
                            {filteredNodeFiles.length === 0 ? (
                                <div style={{ padding: '10px 12px', fontSize: 12, color: '#64748b' }}>No nodes match your filter.</div>
                            ) : null}
                        </div>
                    </div>
                ) : null}

                <button
                    type="button"
                    onClick={onResetSectors}
                    title="Reset all sectors"
                    aria-label="Reset all sectors"
                    style={{
                        borderRadius: 8,
                        border: '1px solid rgba(248, 113, 113, 0.32)',
                        background: 'rgba(127, 29, 29, 0.2)',
                        color: '#fecaca',
                        width: 30,
                        height: 28,
                        display: 'inline-flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        cursor: 'pointer',
                        fontSize: 14,
                        lineHeight: 1,
                    }}
                >
                    ⌂
                </button>
            </div>

            {/* Right: optional slot */}
            <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
                {children}
            </div>
        </div>
    );
}
