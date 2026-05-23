import { app, BrowserWindow, shell } from "electron";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const LOOPBACK_HOSTS = new Set(["localhost", "127.0.0.1", "::1", "[::1]"]);

function isAllowedRendererUrl(rawUrl) {
	if (typeof rawUrl !== "string" || rawUrl.trim().length === 0) {
		return false;
	}

	let parsed;
	try {
		parsed = new URL(rawUrl);
	} catch {
		return false;
	}

	if (parsed.protocol === "file:") {
		return true;
	}

	if (parsed.protocol === "http:" || parsed.protocol === "https:") {
		if (!LOOPBACK_HOSTS.has(parsed.hostname.toLowerCase())) {
			return false;
		}
		if (parsed.username || parsed.password) {
			return false;
		}
		return true;
	}

	return false;
}

function resolveStartUrl() {
	const configured = (process.env.BANANA_DESKTOP_START_URL ?? "").trim();
	if (configured.length > 0 && isAllowedRendererUrl(configured)) {
		return configured;
	}

	if (configured.length > 0) {
		console.error(
			`[security] Rejected BANANA_DESKTOP_START_URL; only file:// or loopback http(s) URLs are allowed. Received: ${configured}`
		);
	}

	return "http://localhost:5173/game-engine";
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
			preload: path.join(__dirname, "preload.cjs"),
			devTools: true,
		},
	});

	const startUrl = resolveStartUrl();
	void window.loadURL(startUrl);

	window.webContents.on("will-navigate", (event, url) => {
		if (!isAllowedRendererUrl(url)) {
			event.preventDefault();
			console.error(`[security] Blocked navigation to disallowed URL: ${url}`);
		}
	});

	window.webContents.setWindowOpenHandler(({ url }) => {
		if (isAllowedRendererUrl(url)) {
			void shell.openExternal(url);
			return { action: "deny" };
		}

		console.error(`[security] Blocked window.open target: ${url}`);
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
