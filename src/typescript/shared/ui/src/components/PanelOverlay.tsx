import { useEffect, useMemo, type CSSProperties, type ReactNode } from 'react';
import { PanelBase, type PanelBaseProps } from './PanelBase';
import { composePanelStages, type PanelStageStyles } from './PanelPipeline';
import { buildPanelBehaviorPipeline } from './PanelVariantPipeline';

export type PanelOverlayPipelineStage = 'backdrop' | 'window' | 'header' | 'content';

export type PanelOverlayChrome = {
    readonly movable?: boolean;
    readonly resizable?: boolean;
    readonly dockable?: boolean;
};

export type PanelOverlayProps = {
    readonly isOpen: boolean;
    readonly title?: string;
    readonly children: ReactNode;
    readonly onClose: () => void;
    readonly maxWidth?: string;
    readonly maxHeight?: string;
    readonly width?: string;
    readonly height?: string;
    readonly variant?: PanelBaseProps['variant'];
    readonly closeOnBackdropClick?: boolean;
    readonly closeOnEscape?: boolean;
    readonly chrome?: PanelOverlayChrome;
    readonly stageStyles?: PanelStageStyles<PanelOverlayPipelineStage>;
};

export function PanelOverlay({
    isOpen,
    title,
    children,
    onClose,
    maxWidth = '70vw',
    maxHeight = '80dvh',
    width = '90vw',
    height = '90dvh',
    variant = 'default',
    closeOnBackdropClick = true,
    closeOnEscape = true,
    chrome,
    stageStyles,
}: PanelOverlayProps) {
    if (!isOpen) return null;
    const panelTitle = title ?? 'Panel';

    useEffect(() => {
        if (!closeOnEscape) {
            return;
        }

        const onKeyDown = (event: KeyboardEvent) => {
            if (event.key === 'Escape') {
                onClose();
            }
        };

        window.addEventListener('keydown', onKeyDown);
        return () => {
            window.removeEventListener('keydown', onKeyDown);
        };
    }, [closeOnEscape, onClose]);

    const pipelineStyles = useMemo(
        () => composePanelStages<PanelOverlayPipelineStage>(
            {
                backdrop: {
                    position: 'fixed',
                    inset: 0,
                    background: 'rgba(0, 0, 0, 0.6)',
                    backdropFilter: 'blur(4px)',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                    zIndex: 999,
                    animation: 'fadeIn 0.2s ease-out',
                },
                window: {
                    background: 'radial-gradient(circle at 20% 15%, rgba(14, 165, 233, 0.08), transparent 40%), linear-gradient(155deg, rgba(7, 19, 34, 0.95) 0%, rgba(10, 27, 45, 0.95) 100%)',
                    border: '1px solid rgba(20, 184, 166, 0.25)',
                    borderRadius: '8px',
                    boxShadow: '0 20px 60px rgba(0, 0, 0, 0.8), inset 0 1px 0 rgba(20, 184, 166, 0.1)',
                    maxWidth,
                    maxHeight,
                    width,
                    height,
                    display: 'flex',
                    flexDirection: 'column',
                    animation: 'slideUp 0.3s ease-out',
                    overflow: 'hidden',
                },
                header: {
                    display: 'flex',
                    alignItems: 'center',
                    gap: '8px',
                },
                content: {
                    color: 'rgba(226, 232, 240, 0.85)',
                    fontSize: '13px',
                    lineHeight: '1.6',
                },
            },
            stageStyles,
        ),
        [height, maxHeight, maxWidth, stageStyles, width],
    );

    const closeButtonStyle: CSSProperties = {
        background: 'none',
        border: 'none',
        color: 'rgba(226, 232, 240, 0.6)',
        fontSize: '20px',
        cursor: 'pointer',
        padding: '4px 8px',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        transition: 'color 0.2s',
    };

    useEffect(() => {
        if (document.head.querySelector('style[data-panel-overlay]')) {
            return;
        }

        const styleSheet = document.createElement('style');
        styleSheet.setAttribute('data-panel-overlay', '');
        styleSheet.textContent = `
            @keyframes fadeIn {
                from { opacity: 0; }
                to { opacity: 1; }
            }
            @keyframes slideUp {
                from {
                    transform: translateY(20px);
                    opacity: 0;
                }
                to {
                    transform: translateY(0);
                    opacity: 1;
                }
            }
        `;
        document.head.appendChild(styleSheet);
    }, []);

    const behaviorPipeline = useMemo(
        () => buildPanelBehaviorPipeline({
            movable: chrome?.movable,
            resizable: chrome?.resizable,
            dockable: chrome?.dockable,
            modal: true,
        }),
        [chrome?.dockable, chrome?.movable, chrome?.resizable],
    );

    const capabilityTokens = behaviorPipeline.tokens.filter(
        (token: string) => token !== 'base' && token !== 'modal',
    );

    const headerAction = (
        <div style={pipelineStyles.header}>
            {capabilityTokens.map((token: string) => (
                <span
                    key={token}
                    style={{
                        borderRadius: 999,
                        border: '1px solid rgba(148, 163, 184, 0.3)',
                        padding: '2px 6px',
                        fontSize: '9px',
                        letterSpacing: '0.04em',
                        color: '#94a3b8',
                        textTransform: 'uppercase',
                    }}
                >
                    {token}
                </span>
            ))}
            <button
                onClick={onClose}
                style={closeButtonStyle}
                onMouseEnter={(event) => {
                    event.currentTarget.style.color = 'rgba(226, 232, 240, 0.9)';
                }}
                onMouseLeave={(event) => {
                    event.currentTarget.style.color = 'rgba(226, 232, 240, 0.6)';
                }}
                title="Close overlay"
                aria-label="Close overlay"
            >
                x
            </button>
        </div>
    );

    return (
        <div
            style={pipelineStyles.backdrop}
            onClick={closeOnBackdropClick ? onClose : undefined}
        >
            <div
                style={pipelineStyles.window}
                data-panel-behavior-pipeline={behaviorPipeline.layers.join('>')}
                onClick={(event) => event.stopPropagation()}
            >
                <PanelBase
                    title={panelTitle}
                    variant={variant}
                    headerAction={headerAction}
                    isScrollable
                    padding="16px 20px"
                    gap="12px"
                >
                    <div style={pipelineStyles.content}>{children}</div>
                </PanelBase>
            </div>
        </div>
    );
}
