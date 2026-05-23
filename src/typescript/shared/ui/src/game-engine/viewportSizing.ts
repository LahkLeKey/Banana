export function attachViewportCanvasSizing(
    canvas: HTMLCanvasElement, viewport: HTMLDivElement): () => void {
  const resolveViewportSize = () => {
    const bounds = viewport.getBoundingClientRect();
    const visualViewport = window.visualViewport;

    const cssWidth = Math.max(
        1,
        Math.floor(
            (bounds.width > 1 ? bounds.width : 0) || visualViewport?.width ||
            window.innerWidth || document.documentElement.clientWidth || 1));
    const cssHeight = Math.max(
        1,
        Math.floor(
            (bounds.height > 1 ? bounds.height : 0) || visualViewport?.height ||
            window.innerHeight || document.documentElement.clientHeight || 1));

    return {cssWidth, cssHeight};
  };

  const syncCanvasCssSize = () => {
    const {cssWidth, cssHeight} = resolveViewportSize();
    const dpr = Math.max(1, window.devicePixelRatio || 1);
    const pixelWidth = Math.max(1, Math.floor(cssWidth * dpr));
    const pixelHeight = Math.max(1, Math.floor(cssHeight * dpr));
    canvas.style.width = `${cssWidth}px`;
    canvas.style.height = `${cssHeight}px`;
    if (canvas.width !== pixelWidth) {
      canvas.width = pixelWidth;
    }
    if (canvas.height !== pixelHeight) {
      canvas.height = pixelHeight;
    }
  };

  syncCanvasCssSize();

  const observer = new ResizeObserver(() => {
    syncCanvasCssSize();
  });
  observer.observe(viewport);

  window.addEventListener('resize', syncCanvasCssSize);
  window.visualViewport?.addEventListener('resize', syncCanvasCssSize);

  return () => {
    observer.disconnect();
    window.removeEventListener('resize', syncCanvasCssSize);
    window.visualViewport?.removeEventListener('resize', syncCanvasCssSize);
  };
}