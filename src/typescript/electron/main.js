// Electron main process (spec 010). Owns app lifecycle + window creation.
// Renderer consumes @banana/ui only via the preload bridge in preload.js.
const {app, BrowserWindow} = require('electron');
const path = require('node:path');

const RENDERER_URL = process.env.BANANA_ELECTRON_RENDERER_URL ?? 'http://localhost:5173';

function createWindow() {
  const win = new BrowserWindow({
    width: 1024,
    height: 720,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  });
  win.loadURL(RENDERER_URL);
}

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});
