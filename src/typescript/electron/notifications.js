// Slice 024 -- native notification of verdict completion using the
// canonical baseline copy. Mirrors src/typescript/react/src/lib/copy.ts.
//
// Lazy-requires electron so this module is require()-safe from the
// smoke harness.

const VERDICT_NOTIFICATION_COPY = Object.freeze({
  bananaConfident: 'Banana.',
  bananaEscalated: 'Banana \u2014 needs a closer look.',
  notBananaConfident: 'Not a banana.',
  notBananaEscalated: 'Not a banana \u2014 needs a closer look.',
  unknown: 'Send a sample to get a verdict.',
});

function verdictBody(verdict) {
  if (!verdict || typeof verdict !== 'object') {
    return VERDICT_NOTIFICATION_COPY.unknown;
  }
  if (verdict.label === 'banana') {
    return verdict.did_escalate ? VERDICT_NOTIFICATION_COPY.bananaEscalated
                                : VERDICT_NOTIFICATION_COPY.bananaConfident;
  }
  if (verdict.label === 'not_banana') {
    return verdict.did_escalate ? VERDICT_NOTIFICATION_COPY.notBananaEscalated
                                : VERDICT_NOTIFICATION_COPY.notBananaConfident;
  }
  return VERDICT_NOTIFICATION_COPY.unknown;
}

function verdictTitle(verdict) {
  if (!verdict || typeof verdict !== 'object') return 'Banana verdict';
  if (verdict.label === 'banana') return 'Banana verdict';
  if (verdict.label === 'not_banana') return 'Banana verdict';
  return 'Banana verdict';
}

/**
 * Raise a native notification for the given verdict / embedding payload.
 * Accepts either an `EnsembleVerdict` shape or the slice 017
 * `{verdict, embedding}` envelope.
 */
function notifyVerdict(payload) {
  const {Notification} = require('electron');
  const verdict = payload && payload.verdict ? payload.verdict : payload;
  if (!Notification.isSupported()) {
    return null;
  }
  const notification = new Notification({
    title: verdictTitle(verdict),
    body: verdictBody(verdict),
  });
  notification.show();
  return notification;
}

// Slice 030 -- drain-success notification copy. Surfaces when a
// queued-while-offline verdict resolves after the channel comes back
// online. Body re-uses the canonical verdict body so the same
// baseline strings appear everywhere.
const DRAIN_NOTIFICATION_TITLE = 'Banana verdict ready';

function drainSuccessBody(verdict) {
  return verdictBody(verdict);
}

function notifyDrainSuccess(payload) {
  const {Notification} = require('electron');
  const verdict = payload && payload.verdict ? payload.verdict : payload;
  if (!Notification.isSupported()) {
    return null;
  }
  const notification = new Notification({
    title: DRAIN_NOTIFICATION_TITLE,
    body: drainSuccessBody(verdict),
  });
  notification.show();
  return notification;
}

module.exports = {notifyVerdict, notifyDrainSuccess, verdictBody, verdictTitle, drainSuccessBody, VERDICT_NOTIFICATION_COPY};
