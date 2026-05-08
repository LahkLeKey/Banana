/**
 * MenuOverlay.tsx — Menu overlay skeleton (z-index 20).
 *
 * Spec 261 T006: Menu overlay component skeleton.
 * Renders in the Menu slot (z-index 20) for pause/main menus.
 *
 * Hidden by default — parent passes `open` prop to show it.
 * Uses data-overlay="true" so pointer events are captured by the overlay.
 */

interface MenuOverlayProps {
  open?: boolean;
}

export function MenuOverlay({ open = false }: MenuOverlayProps) {
  if (!open) return null;

  return (
    <div
      data-overlay="true"
      style={{
        position: "absolute",
        inset: 0,
        display: "flex",
        alignItems: "center",
        justifyContent: "center",
        backgroundColor: "rgba(0, 0, 0, 0.6)",
        pointerEvents: "auto",
      }}
    >
      <div
        style={{
          backgroundColor: "#1a1a2e",
          border: "1px solid #333",
          borderRadius: "0.5rem",
          padding: "2rem",
          color: "#fff",
          fontFamily: "monospace",
          minWidth: "240px",
          textAlign: "center",
        }}
      >
        <p style={{ margin: 0, opacity: 0.7 }}>Menu</p>
      </div>
    </div>
  );
}
