// Electron preload (specs 010 + 024). Exposes a typed, narrow bridge
// to the renderer. No direct ipc/node access leaks past this seam.
//
// Pure factory `buildBridge(ipcRenderer, channels)` is exported for the
// smoke harness so the contract can be asserted without an Electron
// runtime; the real preload path wires it via `contextBridge`.

const IPC_CHANNELS = Object.freeze({
  classifyClipboard: "banana:classify-clipboard",
  verdict: "banana:verdict",
  // Slice 030
  historyUpdate: "banana:history-update",
  drainSuccess: "banana:drain-success",
});

function buildBridge(ipcRenderer, channels) {
  return {
    apiBaseUrl: process.env.BANANA_API_BASE_URL ?? "",
    platform: process.platform,
    classifyClipboard: () => ipcRenderer.invoke(channels.classifyClipboard),
    onVerdict: (handler) => {
      const wrapped = (_event, payload) => {
        try {
          handler(payload);
        } catch {
          /* swallow handler errors */
        }
      };
      ipcRenderer.on(channels.verdict, wrapped);
      return () => ipcRenderer.removeListener(channels.verdict, wrapped);
    },
    // Slice 030 -- renderer publishes its current VerdictHistory snapshot
    // to main so the tray "Show last verdict" entry can read it.
    history: {
      publish: (list) => ipcRenderer.send(channels.historyUpdate, Array.isArray(list) ? list : []),
    },
    // Slice 030 -- renderer signals when a queued submission drained
    // successfully so main raises a native notification.
    notifyDrainSuccess: (payload) => ipcRenderer.send(channels.drainSuccess, payload),
  };
}

if (process.versions && process.versions.electron) {
  // Real Electron runtime path.
  const { contextBridge, ipcRenderer } = require("electron");
  contextBridge.exposeInMainWorld("banana", buildBridge(ipcRenderer, IPC_CHANNELS));
}

module.exports = { IPC_CHANNELS, buildBridge };
