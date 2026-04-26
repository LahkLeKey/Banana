// Slice 030 -- main-process storage shim for window state.
// Persists `{x, y, width, height}` to a JSON file under the Electron
// `userData` path. Built-in fs only (no electron-store dep) so this
// stays compose/CI-friendly.
//
// Pure factory `createWindowStateStore({readFile, writeFile, mkdir, filePath})`
// is exported so the smoke harness can exercise the read/write contract
// without touching the real filesystem.

const path = require('node:path');
const fs = require('node:fs');

const DEFAULT_STATE = Object.freeze({
  x: undefined,
  y: undefined,
  width: 1024,
  height: 720,
});

function sanitize(state) {
  if (!state || typeof state !== 'object') return {...DEFAULT_STATE};
  const out = {...DEFAULT_STATE};
  for (const key of ['x', 'y', 'width', 'height']) {
    const value = state[key];
    if (typeof value === 'number' && Number.isFinite(value)) {
      out[key] = value;
    }
  }
  if (!out.width || out.width < 320) out.width = DEFAULT_STATE.width;
  if (!out.height || out.height < 240) out.height = DEFAULT_STATE.height;
  return out;
}

/**
 * Build a window-state store. Defaults to the real fs but accepts
 * injected primitives for testing.
 *
 * @param {object} [opts]
 * @param {string} [opts.filePath] -- JSON file path; defaults to
 *   `<userData>/window-state.json` when getUserDataPath is provided.
 * @param {() => string} [opts.getUserDataPath]
 * @param {typeof fs.promises.readFile} [opts.readFile]
 * @param {typeof fs.promises.writeFile} [opts.writeFile]
 * @param {typeof fs.promises.mkdir} [opts.mkdir]
 */
function createWindowStateStore(opts = {}) {
  const readFile = opts.readFile || fs.promises.readFile;
  const writeFile = opts.writeFile || fs.promises.writeFile;
  const mkdir = opts.mkdir || fs.promises.mkdir;
  let resolved = opts.filePath;
  function getFilePath() {
    if (resolved) return resolved;
    if (typeof opts.getUserDataPath === 'function') {
      resolved = path.join(opts.getUserDataPath(), 'window-state.json');
      return resolved;
    }
    throw new Error('createWindowStateStore: filePath or getUserDataPath required');
  }
  return {
    async load() {
      try {
        const raw = await readFile(getFilePath(), 'utf8');
        return sanitize(JSON.parse(raw));
      } catch {
        return sanitize(null);
      }
    },
    async save(state) {
      const filePath = getFilePath();
      try {
        await mkdir(path.dirname(filePath), {recursive: true});
        await writeFile(filePath, JSON.stringify(sanitize(state)), 'utf8');
        return true;
      } catch {
        return false;
      }
    },
  };
}

module.exports = {createWindowStateStore, sanitize, DEFAULT_STATE};
