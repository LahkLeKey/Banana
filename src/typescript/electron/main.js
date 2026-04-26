// Electron main process (specs 010 + 024 + 030).
// Owns app lifecycle + window creation + tray + global shortcut + IPC.
const {app, BrowserWindow, clipboard, globalShortcut, ipcMain} = require('electron');
const path = require('node:path');

const {IPC_CHANNELS, setupIpc, defaultClassify, setupHistoryIpc, setupDrainIpc} = require('./ipc');
const {createTray} = require('./tray');
const {notifyVerdict, notifyDrainSuccess} = require('./notifications');
const {createWindowStateStore} = require('./storage');

const RENDERER_URL = process.env.BANANA_ELECTRON_RENDERER_URL ?? 'http://localhost:5173';
const SHORTCUT = 'CommandOrControl+Shift+B';

let mainWindow = null;
let tray = null;
let lastVerdict = null;
// Slice 030 -- main-process cache of the renderer's VerdictHistory
// snapshot so the tray can read it without round-tripping.
let cachedHistory = [];
let windowStateStore = null;

function getMainWindow() { return mainWindow; }
function getApiBaseUrl() { return process.env.BANANA_API_BASE_URL || ''; }
function getLastVerdict() { return lastVerdict; }
function getCachedHistory() { return cachedHistory; }

async function createWindow() {
  // Slice 030 -- restore last position + size.
  const persisted = windowStateStore ? await windowStateStore.load() : {width: 1024, height: 720};
  mainWindow = new BrowserWindow({
    x: persisted.x,
    y: persisted.y,
    width: persisted.width,
    height: persisted.height,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  });
  mainWindow.loadURL(RENDERER_URL);

  // Slice 030 -- persist position + size on lifecycle events.
  const persistState = () => {
    if (!mainWindow || mainWindow.isDestroyed() || !windowStateStore) return;
    const bounds = mainWindow.getBounds();
    void windowStateStore.save(bounds);
  };
  mainWindow.on('resize', persistState);
  mainWindow.on('move', persistState);
  mainWindow.on('close', persistState);
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

app.whenReady().then(async () => {
  windowStateStore = createWindowStateStore({
    getUserDataPath: () => app.getPath('userData'),
  });
  await createWindow();

  setupIpc({
    ipcMain,
    clipboard,
    getMainWindow,
    classify: defaultClassify,
    onVerdict: handleVerdict,
    getApiBaseUrl,
  });

  // Slice 030 -- listen for renderer history snapshots + drain signals.
  setupHistoryIpc({
    ipcMain,
    onHistoryUpdate: (list) => { cachedHistory = list; },
  });
  setupDrainIpc({
    ipcMain,
    onDrainSuccess: (payload) => { notifyDrainSuccess(payload); },
  });

  tray = createTray({
    getMainWindow,
    classifyClipboard,
    getLastVerdict,
    getCachedHistory,
  });

  globalShortcut.register(SHORTCUT, () => { void classifyClipboard(); });
});

app.on('will-quit', () => { globalShortcut.unregisterAll(); });

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});
