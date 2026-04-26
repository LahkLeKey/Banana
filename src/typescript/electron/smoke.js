// Headless smoke harness (spec 010). NOT the desktop UI — just verifies the
// process can boot without a display and exit cleanly.
const {app} = require('electron');

app.whenReady().then(() => {
  console.log('[banana-electron-smoke] ready');
  setTimeout(() => app.quit(), 100);
});
