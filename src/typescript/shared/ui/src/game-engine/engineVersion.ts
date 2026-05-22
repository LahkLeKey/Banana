export function resolveFallbackEngineVersion(): string {
  const env = (import.meta as {env?: Record<string, string|undefined>}).env;
  const configured = env?.VITE_ENGINE_ASSET_VERSION;
  if (configured) return configured;
  if (env?.DEV) return `dev-${Date.now()}`;
  return 'unknown';
}

export function isWebGL2Available(): boolean {
  try {
    const probeCanvas = document.createElement('canvas');
    const context = probeCanvas.getContext('webgl2');
    return context !== null;
  } catch {
    return false;
  }
}

export async function loadEngineAssetVersion(): Promise<string> {
  if (window.location.hostname === 'app.example') {
    return resolveFallbackEngineVersion();
  }

  try {
    const response =
        await fetch(`/wasm/engine.version.json?meta=${Date.now()}`, {
          cache: 'no-store',
        });
    if (!response.ok) {
      return resolveFallbackEngineVersion();
    }
    const payload = (await response.json()) as {engineAssetVersion?: string};
    return payload.engineAssetVersion || resolveFallbackEngineVersion();
  } catch {
    return resolveFallbackEngineVersion();
  }
}

export function startDevEngineVersionWatcher(onVersionChanged: () => void):
    (() => void)|null {
  const env = (import.meta as {env?: Record<string, unknown>}).env;
  const isDev = Boolean(env?.DEV);
  if (!isDev) return null;
  if (window.location.hostname === 'app.example') return null;

  let lastVersion: string|null = null;
  const pollForWasmChanges = async () => {
    try {
      const response =
          await fetch(`/wasm/engine.version.json?dev-watch=${Date.now()}`, {
            cache: 'no-store',
          });
      if (!response.ok) return;

      const payload = (await response.json()) as {engineAssetVersion?: string};
      const currentVersion = payload.engineAssetVersion;
      if (!currentVersion) return;

      if (lastVersion === null) {
        lastVersion = currentVersion;
        return;
      }

      if (currentVersion !== lastVersion) {
        onVersionChanged();
      }
    } catch {
      // Best-effort dev polling; ignore transient fetch errors.
    }
  };

  const watcher = window.setInterval(() => {
    void pollForWasmChanges();
  }, 3000);

  return () => {
    window.clearInterval(watcher);
  };
}