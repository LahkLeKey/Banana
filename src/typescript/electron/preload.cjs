const { contextBridge } = require("electron");

let nativeUiFrame = null;

const bananaBridge = {
  apiBaseUrl: process.env.BANANA_API_BASE_URL ?? "http://localhost:8080",
  platform: process.platform,
  chatApiBaseUrl: process.env.BANANA_CHAT_API_BASE_URL ?? undefined,
  setNativeUiFrame(frame) {
    nativeUiFrame = frame ?? null;
  },
  getNativeUiFrame() {
    return nativeUiFrame;
  },
};

contextBridge.exposeInMainWorld("banana", bananaBridge);