// React Native share-sheet handler (slice 025).
// Registers a listener for inbound share intents (Android ACTION_SEND
// text/plain + iOS share extension) and routes the payload to the
// CaptureScreen via the supplied callback. The actual native share
// extension wiring is declared in app.json (T010); this module is the
// JS-side seam so unit-style code can drive the same flow with a
// stub `getInitialUrl` / `addEventListener` implementation.

export type ShareEvent = {
  text: string
};
export type ShareListener = (event: ShareEvent) => void;

export type LinkingLike = {
  getInitialURL: () => Promise<string|null>;
  addEventListener: (
      type: 'url',
      handler: (event: {url: string}) => void,
      ) => {
    remove: () => void
  };
};

/**
 * Parses a share intent URL of the form `banana://share?text=...`
 * (used by both the Android intent-filter routed through expo-linking
 * and the iOS share-extension `URLScheme` callback) and returns the
 * extracted text payload.
 */
export function parseShareUrl(url: string|null): ShareEvent|null {
  if (!url) return null;
  try {
    const parsed = new URL(url);
    const text = parsed.searchParams.get('text');
    if (text && text.length > 0) return {text};
  } catch {
    // Fallback: best-effort split for runtimes without URL parsing.
    const idx = url.indexOf('text=');
    if (idx >= 0) {
      const text = decodeURIComponent(url.slice(idx + 'text='.length));
      if (text.length > 0) return {text};
    }
  }
  return null;
}

/**
 * Subscribes to share-sheet payloads. Calls `listener` immediately with
 * any cold-start payload, then again for each subsequent share intent.
 * Returns an unsubscribe handle.
 */
export function registerShareListener(
    linking: LinkingLike, listener: ShareListener): () => void {
  void linking.getInitialURL().then((url) => {
    const event = parseShareUrl(url);
    if (event) listener(event);
  });
  const sub = linking.addEventListener('url', ({url}) => {
    const event = parseShareUrl(url);
    if (event) listener(event);
  });
  return () => {
    sub.remove();
  };
}
