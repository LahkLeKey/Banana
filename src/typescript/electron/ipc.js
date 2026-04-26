// Slice 024 -- IPC channel registry + main-process wiring for the
// customer-facing verdict surface.
//
// Channels are exported as plain string constants so the preload bridge
// (preload.js) and smoke harness (smoke.js) can assert contract shape
// without instantiating the Electron runtime.
//
// classifyClipboard: renderer/main -> main: trigger a paste-classify
//   pass against the configured API base URL. Main reads the OS
//   clipboard text in a single place and emits a `verdict` event.
// verdict: main -> renderer: broadcast the resulting EnsembleVerdict
//   so the React surface can re-render and so notifications.js can
//   raise a native notification.

const IPC_CHANNELS = Object.freeze({
  classifyClipboard: 'banana:classify-clipboard',
  verdict: 'banana:verdict',
});

/**
 * Wire the IPC channels in the main process.
 *
 * @param {object} deps
 * @param {() => Electron.BrowserWindow|null} deps.getMainWindow
 * @param {{readText: () => string}} deps.clipboard
 * @param {Electron.IpcMain} deps.ipcMain
 * @param {(text: string, baseUrl: string) => Promise<object>} deps.classify
 *   Performs the actual API call. Injected so smoke + tests can stub.
 * @param {(verdict: object) => void} [deps.onVerdict]
 *   Optional side-effect (notifications.js wires this).
 * @param {() => string} [deps.getApiBaseUrl]
 */
function setupIpc(deps) {
  const {ipcMain, clipboard, getMainWindow, classify, onVerdict, getApiBaseUrl} = deps;
  ipcMain.handle(IPC_CHANNELS.classifyClipboard, async () => {
    const text = (clipboard.readText() || '').trim();
    if (text.length === 0) {
      return {error: 'clipboard is empty'};
    }
    const baseUrl = getApiBaseUrl ? getApiBaseUrl() : (process.env.BANANA_API_BASE_URL || '');
    if (!baseUrl) {
      return {error: 'BANANA_API_BASE_URL is not set'};
    }
    try {
      const verdict = await classify(text, baseUrl);
      const win = getMainWindow();
      if (win && !win.isDestroyed()) {
        win.webContents.send(IPC_CHANNELS.verdict, verdict);
      }
      if (onVerdict) onVerdict(verdict);
      return {verdict};
    } catch (err) {
      return {error: err && err.message ? err.message : String(err)};
    }
  });
}

/**
 * Default classify implementation -- POSTs to /ml/ensemble/embedding
 * and returns the slice 017 payload (verdict + embedding).
 */
async function defaultClassify(text, baseUrl) {
  const response = await fetch(`${baseUrl}/ml/ensemble/embedding`, {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify({inputJson: JSON.stringify({text})}),
  });
  if (!response.ok) {
    throw new Error(`request failed (${response.status})`);
  }
  return response.json();
}

module.exports = {IPC_CHANNELS, setupIpc, defaultClassify};
