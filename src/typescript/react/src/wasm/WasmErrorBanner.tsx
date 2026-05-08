/**
 * WasmErrorBanner.tsx — Degraded-mode overlay with retry and API fallback actions.
 *
 * Spec 261 T007/T008: Shown when WASM fails to load or enters degraded lifecycle state.
 * Exposes retry action and API fallback option per FR-007/FR-008.
 * Renders at z-index 30 (Modal slot) to block interaction while degraded.
 */

interface WasmErrorBannerProps {
  message: string | null;
  retryCount?: number;
  maxRetries?: number;
  onRetry?: () => void;
  onApiFallback?: () => void;
}

export function WasmErrorBanner({
  message,
  retryCount = 0,
  maxRetries = 3,
  onRetry,
  onApiFallback,
}: WasmErrorBannerProps) {
  if (!message) return null;

  const canRetry = retryCount < maxRetries;

  return (
    <div
      data-overlay="true"
      data-testid="wasm-error-banner"
      style={{
        position: "fixed",
        inset: 0,
        zIndex: 30,
        display: "flex",
        flexDirection: "column",
        alignItems: "center",
        justifyContent: "center",
        backgroundColor: "rgba(0, 0, 0, 0.82)",
        color: "#fff",
        fontFamily: "monospace",
        pointerEvents: "auto",
      }}
    >
      <div
        style={{
          maxWidth: "420px",
          padding: "2rem",
          border: "1px solid #b91c1c",
          borderRadius: "0.5rem",
          backgroundColor: "#1a0a0a",
          textAlign: "center",
        }}
      >
        <p style={{ fontSize: "1.1rem", fontWeight: 700, marginBottom: "0.5rem" }}>
          ⚠ Engine Unavailable
        </p>
        <p
          style={{ fontSize: "0.8rem", opacity: 0.75, marginBottom: "1.5rem", lineHeight: 1.5 }}
          data-testid="wasm-error-message"
        >
          {message}
        </p>

        <div style={{ display: "flex", gap: "0.75rem", justifyContent: "center" }}>
          {canRetry && onRetry ? (
            <button
              type="button"
              data-testid="wasm-retry-btn"
              onClick={onRetry}
              style={{
                padding: "0.5rem 1.25rem",
                backgroundColor: "#1d4ed8",
                color: "#fff",
                border: "none",
                borderRadius: "0.375rem",
                cursor: "pointer",
                fontSize: "0.85rem",
                fontFamily: "monospace",
              }}
            >
              Retry ({retryCount}/{maxRetries})
            </button>
          ) : null}

          {!canRetry ? (
            <span style={{ fontSize: "0.75rem", opacity: 0.6 }}>Max retries reached</span>
          ) : null}

          {onApiFallback ? (
            <button
              type="button"
              data-testid="wasm-fallback-btn"
              onClick={onApiFallback}
              style={{
                padding: "0.5rem 1.25rem",
                backgroundColor: "#374151",
                color: "#fff",
                border: "1px solid #4b5563",
                borderRadius: "0.375rem",
                cursor: "pointer",
                fontSize: "0.85rem",
                fontFamily: "monospace",
              }}
            >
              Use API Mode
            </button>
          ) : null}
        </div>
      </div>
    </div>
  );
}
