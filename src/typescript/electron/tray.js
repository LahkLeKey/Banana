// Slice 024 -- tray icon + menu using baseline copy.
//
// Lazy-requires electron inside createTray so this module stays
// require()-safe from the smoke harness running under plain node.

const path = require('node:path');

const {verdictBody} = require('./notifications');

const TRAY_MENU_LABELS = Object.freeze({
  classifyClipboard: 'Classify clipboard',
  showLastVerdict: 'Show last verdict',
  quit: 'Quit Banana',
});

/**
 * @param {object} deps
 * @param {() => Electron.BrowserWindow|null} deps.getMainWindow
 * @param {() => Promise<void>} deps.classifyClipboard
 *   Triggers the same flow as the renderer + global shortcut.
 * @param {() => object|null} deps.getLastVerdict
 * @param {() => object[]} [deps.getCachedHistory]
 *   Slice 030 -- returns the latest renderer-published history list;
 *   tray "Show last verdict" surfaces the newest entry as a native
 *   notification.
 */
function createTray(deps) {
  // Lazy require so this module loads under `node smoke.js`.
  const {Tray, Menu, Notification, nativeImage} = require('electron');
  const iconPath = path.join(__dirname, 'assets', 'tray-icon.png');
  const icon = nativeImage.createFromPath(iconPath);
  const tray = new Tray(icon.isEmpty() ? nativeImage.createEmpty() : icon);
  tray.setToolTip('Banana');

  const menu = Menu.buildFromTemplate([
    {
      label: TRAY_MENU_LABELS.classifyClipboard,
      click: () => { void deps.classifyClipboard(); },
    },
    {
      label: TRAY_MENU_LABELS.showLastVerdict,
      click: () => {
        // Slice 030 -- prefer the renderer-published history snapshot.
        const cached = typeof deps.getCachedHistory === 'function' ? deps.getCachedHistory() : [];
        const newest = cached && cached.length > 0 ? cached[0] : null;
        const verdict = newest && newest.verdict
          ? newest.verdict
          : (typeof deps.getLastVerdict === 'function' ? deps.getLastVerdict() : null);
        if (Notification.isSupported()) {
          new Notification({
            title: 'Banana verdict',
            body: verdictBody(verdict),
          }).show();
        }
        const win = deps.getMainWindow();
        if (win && !win.isDestroyed()) {
          win.show();
          win.focus();
        }
      },
    },
    {type: 'separator'},
    {label: TRAY_MENU_LABELS.quit, role: 'quit'},
  ]);
  tray.setContextMenu(menu);
  return tray;
}

module.exports = {createTray, TRAY_MENU_LABELS};
