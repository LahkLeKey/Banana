/**
 * HudOverlay.tsx — HUD overlay skeleton (z-index 10).
 *
 * Spec 261 T005: HUD overlay component skeleton.
 * Renders in the HUD slot (z-index 10) for in-game status elements.
 *
 * Set data-overlay="true" on interactive children to capture pointer events.
 */

export function HudOverlay() {
  return (
    <div
      data-overlay="true"
      style={{
        position: "absolute",
        top: "1rem",
        left: "1rem",
        color: "#fff",
        fontFamily: "monospace",
        fontSize: "0.8rem",
        pointerEvents: "none",
        userSelect: "none",
      }}
    >
      {/* HUD elements rendered here */}
    </div>
  );
}
