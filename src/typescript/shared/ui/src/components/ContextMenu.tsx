import type { ContextMenuState, ContextMenuAction } from "./GameEngineTypes";

export interface ContextMenuProps {
    state: ContextMenuState;
    actions: ContextMenuAction[];
    onAction: (actionId: string) => void;
    onClose: () => void;
}

/**
 * Context menu for gameplay actions (Interact, Harvest, etc.)
 * Positioned near right-click / long-press location
 * Used by both desktop and mobile platforms
 */
export function ContextMenu({ state, actions, onAction, onClose }: ContextMenuProps) {
    if (!state.visible) {
        return null;
    }

    return (
        <>
            {/* Invisible overlay to dismiss on click outside menu */}
            <button
                type="button"
                aria-label="Close action menu"
                onMouseDown={onClose}
                style={{
                    position: "absolute",
                    inset: 0,
                    border: "none",
                    padding: 0,
                    margin: 0,
                    background: "transparent",
                    zIndex: 1100,
                    cursor: "default",
                }}
            />

            {/* Menu container */}
            <div
                style={{
                    position: "absolute",
                    top: state.y,
                    left: state.x,
                    width: 220,
                    padding: 8,
                    backgroundColor: "rgba(18, 18, 22, 0.96)",
                    border: "1px solid rgba(255, 255, 255, 0.2)",
                    borderRadius: 8,
                    boxShadow: "0 12px 28px rgba(0, 0, 0, 0.45)",
                    color: "rgba(255, 255, 255, 0.94)",
                    fontSize: 13,
                    fontFamily: "monospace",
                    zIndex: 1200,
                }}
            >
                <div style={{ padding: "4px 6px", opacity: 0.85, fontWeight: 700 }}>Actions</div>

                {actions.map((action) => (
                    <button
                        key={action.actionId}
                        type="button"
                        onClick={() => onAction(action.actionId)}
                        style={{
                            width: "100%",
                            textAlign: "left",
                            border: "none",
                            background: "transparent",
                            color: "rgba(255, 255, 255, 0.9)",
                            padding: "8px 6px",
                            fontFamily: "monospace",
                            cursor: "pointer",
                        }}
                    >
                        {action.label}
                    </button>
                ))}

                <button
                    type="button"
                    onClick={onClose}
                    style={{
                        width: "100%",
                        textAlign: "left",
                        border: "none",
                        background: "transparent",
                        color: "rgba(255, 255, 255, 0.9)",
                        padding: "8px 6px",
                        fontFamily: "monospace",
                        cursor: "pointer",
                    }}
                >
                    Close
                </button>
            </div>
        </>
    );
}
