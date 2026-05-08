/**
 * WasmErrorBanner.tsx — Graceful degradation banner.
 *
 * Spec 261 T012: Shown when WASM fails to load or is unavailable.
 * Renders at overlay z-index 40 (Debug slot) so it sits above all game content.
 */

interface WasmErrorBannerProps {
  message: string | null;
}

export function WasmErrorBanner({ message }: WasmErrorBannerProps) {
  if (!message) return null;

  return (
    <div
      data-overlay="true"
      style={{
        position: "fixed",
        top: 0,
        left: 0,
        right: 0,
        zIndex: 40,
        backgroundColor: "rgba(185, 28, 28, 0.92)",
        color: "#fff",
        padding: "0.75rem 1.25rem",
        fontFamily: "monospace",
        fontSize: "0.875rem",
        display: "flex",
        alignItems: "center",
        gap: "0.75rem",
      }}
    >
      <span style={{ fontWeight: 700 }}>⚠ WASM engine unavailable</span>
      <span style={{ opacity: 0.85 }}>{message}</span>
    </div>
  );
}
