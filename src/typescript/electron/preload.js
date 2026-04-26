// Electron preload (specs 010 + 024). Exposes a typed, narrow bridge
// to the renderer. No direct ipc/node access leaks past this seam.
//
// Pure factory `buildBridge(ipcRenderer, channels)` is exported for the
// smoke harness so the contract can be asserted without an Electron
// runtime; the real preload path wires it via `contextBridge`.

const IPC_CHANNELS = Object.freeze({
  classifyClipboard: 'banana:classify-clipboard',
  verdict: 'banana:verdict',
});

function buildBridge(ipcRenderer, channels) {
  return {
    apiBaseUrl: process.env.BANANA_API_BASE_URL ?? '',
    platform: process.platform,
    classifyClipboard: () => ipcRenderer.invoke(channels.classifyClipboard),
    onVerdict: (handler) => {
      const wrapped = (_event, payload) => {
        try { handler(payload); } catch { /* swallow handler errors */ }
      };
      ipcRenderer.on(channels.verdict, wrapped);
      return () => ipcRenderer.removeListener(channels.verdict, wrapped);
    },
  };
}

if (process.versions && process.versions.electron) {
  // Real Electron runtime path.
  const {contextBridge, ipcRenderer} = require('electron');
  contextBridge.exposeInMainWorld('banana', buildBridge(ipcRenderer, IPC_CHANNELS));
}

module.exports = {IPC_CHANNELS, buildBridge};
