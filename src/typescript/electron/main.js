// Electron main process (specs 010 + 024).
// Owns app lifecycle + window creation + tray + global shortcut + IPC.
const {app, BrowserWindow, clipboard, globalShortcut, ipcMain} = require('electron');
const path = require('node:path');

const {IPC_CHANNELS, setupIpc, defaultClassify} = require('./ipc');
const {createTray} = require('./tray');
const {notifyVerdict} = require('./notifications');

const RENDERER_URL = process.env.BANANA_ELECTRON_RENDERER_URL ?? 'http://localhost:5173';
const SHORTCUT = 'CommandOrControl+Shift+B';

let mainWindow = null;
let tray = null;
let lastVerdict = null;

function getMainWindow() { return mainWindow; }
function getApiBaseUrl() { return process.env.BANANA_API_BASE_URL || ''; }
function getLastVerdict() { return lastVerdict; }

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1024,
    height: 720,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  });
  mainWindow.loadURL(RENDERER_URL);
  mainWindow.on('closed', () => { mainWindow = null; });
}

async function classifyClipboard() {
  const text = (clipboard.readText() || '').trim();
  if (text.length === 0) return null;
  try {
    const payload = await defaultClassify(text, getApiBaseUrl());
    if (mainWindow && !mainWindow.isDestroyed()) {
      mainWindow.webContents.send(IPC_CHANNELS.verdict, payload);
    }
    handleVerdict(payload);
    return payload;
  } catch {
    return null;
  }
}

function handleVerdict(payload) {
  lastVerdict = payload && payload.verdict ? payload.verdict : payload;
  notifyVerdict(payload);
}

app.whenReady().then(() => {
  createWindow();

  setupIpc({
    ipcMain,
    clipboard,
    getMainWindow,
    classify: defaultClassify,
    onVerdict: handleVerdict,
    getApiBaseUrl,
  });

  tray = createTray({
    getMainWindow,
    classifyClipboard,
    getLastVerdict,
  });

  globalShortcut.register(SHORTCUT, () => { void classifyClipboard(); });
});

app.on('will-quit', () => { globalShortcut.unregisterAll(); });

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});
