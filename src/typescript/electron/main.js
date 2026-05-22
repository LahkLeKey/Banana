import { app, BrowserWindow, shell } from "electron";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

function resolveStartUrl() {
	const configured = (process.env.BANANA_DESKTOP_START_URL ?? "").trim();
	if (configured.length > 0) {
		return configured;
	}
	return "http://localhost:5173/session-room";
}

function createWindow() {
	const window = new BrowserWindow({
		width: 1440,
		height: 900,
		minWidth: 1024,
		minHeight: 640,
		backgroundColor: "#0b1020",
		title: "Banana Engineer",
		webPreferences: {
			contextIsolation: true,
			nodeIntegration: false,
			sandbox: true,
			devTools: true,
		},
	});

	const startUrl = resolveStartUrl();
	void window.loadURL(startUrl);

	window.webContents.setWindowOpenHandler(({ url }) => {
		void shell.openExternal(url);
		return { action: "deny" };
	});

	return window;
}

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

export function bootElectronRuntime() {
	return {
		ready: app.isReady(),
		channel: "electron",
		entry: path.join(__dirname, "main.js"),
	};
}
