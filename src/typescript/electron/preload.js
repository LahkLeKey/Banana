// Electron preload (spec 010). Exposes a typed, narrow bridge to renderer.
// No direct ipc/node access leaks past this seam.
const {contextBridge} = require('electron');

contextBridge.exposeInMainWorld('banana', {
  apiBaseUrl: process.env.BANANA_API_BASE_URL ?? '',
  platform: process.platform,
});
