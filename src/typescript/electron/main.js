const path = require("path");
const fs = require("fs");
const { app, BrowserWindow, ipcMain } = require("electron");
const { spawn } = require("child_process");

const apiClient = require("./apiClient");

const defaultApiUrl = process.env.BANANA_API_URL || "http://127.0.0.1:8080";
const nodeExecutable = process.env.BANANA_NODE_PATH || "node";
const rendererDevUrl = process.env.BANANA_ELECTRON_RENDERER_URL || "";
const rendererDistPath = path.join(__dirname, "renderer", "dist", "index.html");

function loadRenderer(window) {
  if (rendererDevUrl) {
    return window.loadURL(rendererDevUrl);
  }

  if (fs.existsSync(rendererDistPath)) {
    return window.loadFile(rendererDistPath);
  }

  const missingBuildHtml = encodeURIComponent(`
    <!doctype html>
    <html>
      <head>
        <meta charset="UTF-8" />
        <title>Banana Electron Renderer Missing</title>
      </head>
      <body style="font-family: system-ui, sans-serif; margin: 24px; line-height: 1.4;">
        <h2>Renderer build not found</h2>
        <p>Build the Bun React renderer before launching Electron:</p>
        <pre>bun install --cwd src/typescript/electron/renderer
      bun run --cwd src/typescript/electron/renderer build</pre>
      </body>
    </html>
  `);

  return window.loadURL(`data:text/html;charset=utf-8,${missingBuildHtml}`);
}

function createWindow() {
  const window = new BrowserWindow({
    width: 1120,
    height: 760,
    minWidth: 900,
    minHeight: 640,
    backgroundColor: "#f5ecd9",
    webPreferences: {
      contextIsolation: true,
      nodeIntegration: false,
      preload: path.join(__dirname, "preload.js"),
    },
  });

  loadRenderer(window).catch((error) => {
    console.error("failed to load electron renderer", error);
  });
}

function runNativeWorker(request) {
  return new Promise((resolve, reject) => {
    const worker = spawn(nodeExecutable, [path.join(__dirname, "nativeCli.js"), JSON.stringify(request)], {
      env: process.env,
      stdio: ["ignore", "pipe", "pipe"],
    });

    let stdout = "";
    let stderr = "";

    worker.stdout.on("data", (chunk) => {
      stdout += chunk;
    });

    worker.stderr.on("data", (chunk) => {
      stderr += chunk;
    });

    worker.on("error", reject);
    worker.on("close", (code) => {
      if (!stdout.trim()) {
        reject(new Error(stderr.trim() || `native worker exited with code ${code}`));
        return;
      }

      const message = JSON.parse(stdout);
      if (message.ok) {
        resolve(message.result);
        return;
      }

      const error = new Error(message.error.message);
      error.status = message.error.status;
      reject(error);
    });
  });
}

ipcMain.handle("banana:config", async () => ({
  apiUrl: defaultApiUrl,
  nativePath: process.env.BANANA_NATIVE_PATH || null,
}));

ipcMain.handle("banana:health", async (_event, payload) => apiClient.fetchHealth(payload.apiUrl || defaultApiUrl));

ipcMain.handle("banana:native", async (_event, payload) => runNativeWorker({
  action: "scenario",
  purchases: payload.purchases,
  multiplier: payload.multiplier,
  includeDatabase: Boolean(payload.includeDatabase),
}));

ipcMain.handle("banana:api", async (_event, payload) => apiClient.fetchBanana(
  payload.apiUrl || defaultApiUrl,
  payload.purchases,
  payload.multiplier,
));

ipcMain.handle("banana:compare", async (_event, payload) => {
  const apiUrl = payload.apiUrl || defaultApiUrl;
  const [nativeResult, apiResult] = await Promise.all([
    runNativeWorker({
      action: "scenario",
      purchases: payload.purchases,
      multiplier: payload.multiplier,
      includeDatabase: Boolean(payload.includeDatabase),
    }),
    apiClient.fetchBanana(apiUrl, payload.purchases, payload.multiplier),
  ]);

  return {
    native: nativeResult,
    api: apiResult,
    matches: nativeResult.calculation === apiResult.payload.banana,
  };
});

app.whenReady().then(() => {
  createWindow();

  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow();
    }
  });
});

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});