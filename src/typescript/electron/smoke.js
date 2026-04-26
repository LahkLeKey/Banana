// Slice 024 -- smoke harness.
//
// Runs as `node smoke.js` (no Electron runtime). Asserts the require()
// graph + bridge contract + IPC channel constants without instantiating
// Electron objects. Validation lane consumer:
//   docker compose --profile electron run --rm electron-example npm run smoke

const assert = require('node:assert/strict');

let exitCode = 0;
const pending = [];
function check(label, fn) {
  try {
    const out = fn();
    if (out && typeof out.then === 'function') {
      pending.push(out.then(
        () => console.log(`[banana-electron-smoke] ok  -- ${label}`),
        (err) => {
          console.error(`[banana-electron-smoke] FAIL -- ${label}: ${err && err.message ? err.message : err}`);
          exitCode = 1;
        },
      ));
    } else {
      console.log(`[banana-electron-smoke] ok  -- ${label}`);
    }
  } catch (err) {
    console.error(`[banana-electron-smoke] FAIL -- ${label}: ${err && err.message ? err.message : err}`);
    exitCode = 1;
  }
}

check('ipc.js exports IPC_CHANNELS + setupIpc + defaultClassify', () => {
  const ipc = require('./ipc');
  assert.equal(typeof ipc.setupIpc, 'function');
  assert.equal(typeof ipc.defaultClassify, 'function');
  assert.equal(ipc.IPC_CHANNELS.classifyClipboard, 'banana:classify-clipboard');
  assert.equal(ipc.IPC_CHANNELS.verdict, 'banana:verdict');
  assert.ok(Object.isFrozen(ipc.IPC_CHANNELS));
});

check('preload.js exports a pure buildBridge factory', () => {
  const {IPC_CHANNELS, buildBridge} = require('./preload');
  assert.equal(typeof buildBridge, 'function');
  const calls = [];
  const ipcRendererStub = {
    invoke: (channel) => { calls.push(['invoke', channel]); return Promise.resolve('ok'); },
    on: (channel, handler) => { calls.push(['on', channel, handler]); },
    removeListener: (channel, handler) => { calls.push(['removeListener', channel, handler]); },
  };
  const bridge = buildBridge(ipcRendererStub, IPC_CHANNELS);
  assert.equal(typeof bridge.apiBaseUrl, 'string');
  assert.equal(typeof bridge.platform, 'string');
  assert.equal(typeof bridge.classifyClipboard, 'function');
  assert.equal(typeof bridge.onVerdict, 'function');
  void bridge.classifyClipboard();
  assert.deepEqual(calls[0], ['invoke', 'banana:classify-clipboard']);
  const handler = () => {};
  const off = bridge.onVerdict(handler);
  assert.equal(calls[1][0], 'on');
  assert.equal(calls[1][1], 'banana:verdict');
  assert.equal(typeof off, 'function');
  off();
  assert.equal(calls[2][0], 'removeListener');
});

check('notifications.js maps verdicts to baseline copy', () => {
  const {verdictBody, VERDICT_NOTIFICATION_COPY} = require('./notifications');
  assert.equal(verdictBody({label: 'banana', did_escalate: false}), 'Banana.');
  assert.equal(verdictBody({label: 'banana', did_escalate: true}), 'Banana \u2014 needs a closer look.');
  assert.equal(verdictBody({label: 'not_banana', did_escalate: false}), 'Not a banana.');
  assert.equal(verdictBody({label: 'not_banana', did_escalate: true}), 'Not a banana \u2014 needs a closer look.');
  assert.equal(verdictBody(null), 'Send a sample to get a verdict.');
  assert.ok(Object.isFrozen(VERDICT_NOTIFICATION_COPY));
});

check('tray.js exports createTray + TRAY_MENU_LABELS with baseline copy', () => {
  const {createTray, TRAY_MENU_LABELS} = require('./tray');
  assert.equal(typeof createTray, 'function');
  assert.equal(TRAY_MENU_LABELS.classifyClipboard, 'Classify clipboard');
  assert.equal(TRAY_MENU_LABELS.showLastVerdict, 'Show last verdict');
});

check('defaultClassify hits /ml/ensemble/embedding with the slice 017 envelope', async () => {
  const {defaultClassify} = require('./ipc');
  const calls = [];
  global.fetch = async (url, init) => {
    calls.push([url, init]);
    return {
      ok: true,
      status: 200,
      json: async () => ({verdict: {label: 'banana', score: 0.9, did_escalate: false, calibration_magnitude: 0.5, status: 'ok'}, embedding: null}),
    };
  };
  const result = await defaultClassify('ripe banana', 'http://api.example');
  assert.equal(calls[0][0], 'http://api.example/ml/ensemble/embedding');
  assert.equal(calls[0][1].method, 'POST');
  const body = JSON.parse(calls[0][1].body);
  const inner = JSON.parse(body.inputJson);
  assert.equal(inner.text, 'ripe banana');
  assert.equal(result.verdict.label, 'banana');
});

(async () => {
  await Promise.all(pending);
  if (exitCode === 0) {
    console.log('[banana-electron-smoke] all checks passed');
  }
  process.exit(exitCode);
})();
