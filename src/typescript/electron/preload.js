const { contextBridge, ipcRenderer } = require("electron");

contextBridge.exposeInMainWorld("bananaQA", {
  getConfig: () => ipcRenderer.invoke("banana:config"),
  checkHealth: (payload) => ipcRenderer.invoke("banana:health", payload),
  runNative: (payload) => ipcRenderer.invoke("banana:native", payload),
  runApi: (payload) => ipcRenderer.invoke("banana:api", payload),
  runCompare: (payload) => ipcRenderer.invoke("banana:compare", payload),
});