import { useEffect, useMemo, useState } from "react";

export type WasmLifecycle = "booting" | "ready" | "degraded";

export type WasmViewportProps = {
  onLifecycle?: (state: WasmLifecycle) => void;
  onApiFallback?: () => void;
  maxRetries?: number;
};

export function WasmViewport({ onLifecycle, onApiFallback, maxRetries = 3 }: WasmViewportProps) {
  const [lifecycle, setLifecycle] = useState<WasmLifecycle>("booting");
  const [errorMessage, setErrorMessage] = useState<string>("");
  const [attempt, setAttempt] = useState(0);

  useEffect(() => {
    onLifecycle?.(lifecycle);
  }, [lifecycle, onLifecycle]);

  useEffect(() => {
    let cancelled = false;

    const load = async () => {
      try {
        const response = await fetch("/wasm/engine.wasm");
        if (!response.ok) throw new Error(`WASM fetch failed with status ${response.status}`);
        if (!cancelled) {
          setErrorMessage("");
          setLifecycle("ready");
        }
      } catch (err) {
        if (cancelled) return;
        const message = err instanceof Error ? err.message : "Failed to load WASM engine.";
        setErrorMessage(message);
        setLifecycle("degraded");
      }
    };

    void load();

    return () => {
      cancelled = true;
    };
  }, [attempt]);

  const canRetry = useMemo(
    () => lifecycle === "degraded" && attempt < maxRetries,
    [lifecycle, attempt, maxRetries]
  );

  return (
    <section className="relative">
      <canvas
        data-testid="wasm-canvas"
        data-lifecycle={lifecycle}
        style={{ opacity: lifecycle === "degraded" ? "0.3" : "1" }}
      />

      {lifecycle === "degraded" ? (
        <div data-testid="wasm-error-banner" role="alert">
          <p data-testid="wasm-error-message">{errorMessage}</p>

          {canRetry ? (
            <button data-testid="wasm-retry-btn" onClick={() => setAttempt((value) => value + 1)}>
              Retry
            </button>
          ) : null}

          {onApiFallback ? (
            <button data-testid="wasm-fallback-btn" onClick={onApiFallback}>
              Use API fallback
            </button>
          ) : null}
        </div>
      ) : null}
    </section>
  );
}
