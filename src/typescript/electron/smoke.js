// Slice 024 -- smoke harness.
//
// Runs as `node smoke.js` (no Electron runtime). Asserts the require()
// graph + bridge contract + IPC channel constants without instantiating
// Electron objects. Validation lane consumer:
//   docker compose --profile electron run --rm electron-example npm run smoke

const assert = require("node:assert/strict");

let exitCode = 0;
const pending = [];
function check(label, fn) {
  try {
    const out = fn();
    if (out && typeof out.then === "function") {
      pending.push(
        out.then(
          () => console.log(`[banana-electron-smoke] ok  -- ${label}`),
          (err) => {
            console.error(
              `[banana-electron-smoke] FAIL -- ${label}: ${err && err.message ? err.message : err}`
            );
            exitCode = 1;
          }
        )
      );
    } else {
      console.log(`[banana-electron-smoke] ok  -- ${label}`);
    }
  } catch (err) {
    console.error(
      `[banana-electron-smoke] FAIL -- ${label}: ${err && err.message ? err.message : err}`
    );
    exitCode = 1;
  }
}

check("ipc.js exports IPC_CHANNELS + setupIpc + defaultClassify", () => {
  const ipc = require("./ipc");
  assert.equal(typeof ipc.setupIpc, "function");
  assert.equal(typeof ipc.defaultClassify, "function");
  assert.equal(ipc.IPC_CHANNELS.classifyClipboard, "banana:classify-clipboard");
  assert.equal(ipc.IPC_CHANNELS.verdict, "banana:verdict");
  assert.ok(Object.isFrozen(ipc.IPC_CHANNELS));
});

check("preload.js exports a pure buildBridge factory", () => {
  const { IPC_CHANNELS, buildBridge } = require("./preload");
  assert.equal(typeof buildBridge, "function");
  const calls = [];
  const ipcRendererStub = {
    invoke: (channel) => {
      calls.push(["invoke", channel]);
      return Promise.resolve("ok");
    },
    on: (channel, handler) => {
      calls.push(["on", channel, handler]);
    },
    removeListener: (channel, handler) => {
      calls.push(["removeListener", channel, handler]);
    },
  };
  const bridge = buildBridge(ipcRendererStub, IPC_CHANNELS);
  assert.equal(typeof bridge.apiBaseUrl, "string");
  assert.equal(typeof bridge.platform, "string");
  assert.equal(typeof bridge.classifyClipboard, "function");
  assert.equal(typeof bridge.onVerdict, "function");
  void bridge.classifyClipboard();
  assert.deepEqual(calls[0], ["invoke", "banana:classify-clipboard"]);
  const handler = () => {};
  const off = bridge.onVerdict(handler);
  assert.equal(calls[1][0], "on");
  assert.equal(calls[1][1], "banana:verdict");
  assert.equal(typeof off, "function");
  off();
  assert.equal(calls[2][0], "removeListener");
});

check("notifications.js maps verdicts to baseline copy", () => {
  const { verdictBody, VERDICT_NOTIFICATION_COPY } = require("./notifications");
  assert.equal(verdictBody({ label: "banana", did_escalate: false }), "Banana.");
  assert.equal(
    verdictBody({ label: "banana", did_escalate: true }),
    "Banana \u2014 needs a closer look."
  );
  assert.equal(verdictBody({ label: "not_banana", did_escalate: false }), "Not a banana.");
  assert.equal(
    verdictBody({ label: "not_banana", did_escalate: true }),
    "Not a banana \u2014 needs a closer look."
  );
  assert.equal(verdictBody(null), "Send a sample to get a verdict.");
  assert.ok(Object.isFrozen(VERDICT_NOTIFICATION_COPY));
});

check("tray.js exports createTray + TRAY_MENU_LABELS with baseline copy", () => {
  const { createTray, TRAY_MENU_LABELS } = require("./tray");
  assert.equal(typeof createTray, "function");
  assert.equal(TRAY_MENU_LABELS.classifyClipboard, "Classify clipboard");
  assert.equal(TRAY_MENU_LABELS.showLastVerdict, "Show last verdict");
});

// Slice 030 -- bridge gains `history.publish` + `notifyDrainSuccess`.
check("preload.js bridge exposes slice 030 history + drain channels", () => {
  const { IPC_CHANNELS, buildBridge } = require("./preload");
  assert.equal(IPC_CHANNELS.historyUpdate, "banana:history-update");
  assert.equal(IPC_CHANNELS.drainSuccess, "banana:drain-success");
  const sent = [];
  const ipcRendererStub = {
    invoke: () => Promise.resolve("ok"),
    on: () => {},
    removeListener: () => {},
    send: (channel, payload) => {
      sent.push([channel, payload]);
    },
  };
  const bridge = buildBridge(ipcRendererStub, IPC_CHANNELS);
  assert.equal(typeof bridge.history.publish, "function");
  assert.equal(typeof bridge.notifyDrainSuccess, "function");
  bridge.history.publish([{ id: "v1" }]);
  assert.deepEqual(sent[0], ["banana:history-update", [{ id: "v1" }]]);
  bridge.notifyDrainSuccess({ verdict: { label: "banana" } });
  assert.deepEqual(sent[1], ["banana:drain-success", { verdict: { label: "banana" } }]);
  // Non-array payloads to publish coerce to []
  bridge.history.publish(null);
  assert.deepEqual(sent[2], ["banana:history-update", []]);
});

// Slice 030 -- ipc.js exposes setupHistoryIpc + setupDrainIpc helpers.
check("ipc.js exposes slice 030 setupHistoryIpc + setupDrainIpc helpers", () => {
  const { IPC_CHANNELS, setupHistoryIpc, setupDrainIpc } = require("./ipc");
  assert.equal(typeof setupHistoryIpc, "function");
  assert.equal(typeof setupDrainIpc, "function");
  const handlers = {};
  const ipcMainStub = {
    on: (channel, handler) => {
      handlers[channel] = handler;
    },
    handle: () => {},
  };
  let history = null;
  let drained = null;
  setupHistoryIpc({
    ipcMain: ipcMainStub,
    onHistoryUpdate: (list) => {
      history = list;
    },
  });
  setupDrainIpc({
    ipcMain: ipcMainStub,
    onDrainSuccess: (payload) => {
      drained = payload;
    },
  });
  handlers[IPC_CHANNELS.historyUpdate](null, [{ id: "a" }]);
  assert.deepEqual(history, [{ id: "a" }]);
  // Non-array payload is dropped.
  handlers[IPC_CHANNELS.historyUpdate](null, "not-an-array");
  assert.deepEqual(history, [{ id: "a" }]);
  handlers[IPC_CHANNELS.drainSuccess](null, { verdict: { label: "banana" } });
  assert.deepEqual(drained, { verdict: { label: "banana" } });
});

// Slice 030 -- notifications.js gains drainSuccessBody + notifyDrainSuccess.
check("notifications.js exposes drain-success path with baseline copy", () => {
  const { drainSuccessBody, notifyDrainSuccess } = require("./notifications");
  assert.equal(typeof notifyDrainSuccess, "function");
  assert.equal(drainSuccessBody({ label: "banana", did_escalate: false }), "Banana.");
  assert.equal(
    drainSuccessBody({ label: "banana", did_escalate: true }),
    "Banana \u2014 needs a closer look."
  );
});

// Slice 030 -- storage.js window-state shim round-trips via injected fs.
check("storage.js round-trips window state via injected fs primitives", async () => {
  const { createWindowStateStore, sanitize, DEFAULT_STATE } = require("./storage");
  assert.equal(DEFAULT_STATE.width, 1024);
  assert.equal(DEFAULT_STATE.height, 720);
  // sanitize discards bogus values + clamps undersized ones.
  const cleaned = sanitize({ x: 10, y: 20, width: 100, height: 50 });
  assert.equal(cleaned.x, 10);
  assert.equal(cleaned.width, 1024);
  assert.equal(cleaned.height, 720);
  let written = null;
  const fakeStore = { data: null };
  const store = createWindowStateStore({
    filePath: "/tmp/banana-window-state.json",
    readFile: async () => {
      if (fakeStore.data === null) throw new Error("no file");
      return fakeStore.data;
    },
    writeFile: async (_p, contents) => {
      written = contents;
      fakeStore.data = contents;
    },
    mkdir: async () => {},
  });
  // First load -> defaults.
  let state = await store.load();
  assert.equal(state.width, 1024);
  // Save -> persists.
  const ok = await store.save({ x: 100, y: 200, width: 1280, height: 800 });
  assert.equal(ok, true);
  assert.ok(written.includes("1280"));
  // Reload -> persisted values.
  state = await store.load();
  assert.equal(state.x, 100);
  assert.equal(state.width, 1280);
});

check("defaultClassify hits /ml/ensemble/embedding with the slice 017 envelope", async () => {
  const { defaultClassify } = require("./ipc");
  const calls = [];
  global.fetch = async (url, init) => {
    calls.push([url, init]);
    return {
      ok: true,
      status: 200,
      json: async () => ({
        verdict: {
          label: "banana",
          score: 0.9,
          did_escalate: false,
          calibration_magnitude: 0.5,
          status: "ok",
        },
        embedding: null,
      }),
    };
  };
  const result = await defaultClassify("ripe banana", "http://api.example");
  assert.equal(calls[0][0], "http://api.example/ml/ensemble/embedding");
  assert.equal(calls[0][1].method, "POST");
  const body = JSON.parse(calls[0][1].body);
  const inner = JSON.parse(body.inputJson);
  assert.equal(inner.text, "ripe banana");
  assert.equal(result.verdict.label, "banana");
});

(async () => {
  await Promise.all(pending);
  if (exitCode === 0) {
    console.log("[banana-electron-smoke] all checks passed");
  }
  process.exit(exitCode);
})();
