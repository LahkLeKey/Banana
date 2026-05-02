/**
 * vitals.ts — Core Web Vitals collection + reporting (feature 061).
 *
 * Reports LCP, INP, CLS, FCP, TTFB to the configured beacon endpoint.
 * Falls back to console.debug in development so we never lose the signal.
 */
import { onCLS, onFCP, onINP, onLCP, onTTFB, type Metric } from "web-vitals";

const BEACON_URL = import.meta.env.VITE_VITALS_BEACON_URL ?? "";
const DEV = import.meta.env.DEV;

function send(metric: Metric): void {
  if (DEV) {
    // eslint-disable-next-line no-console
    console.debug(`[vitals] ${metric.name}`, metric.value, metric);
    return;
  }
  if (!BEACON_URL) return;
  const body = JSON.stringify({
    name: metric.name,
    value: metric.value,
    rating: metric.rating,
    id: metric.id,
    navigationType: metric.navigationType,
    url: window.location.href,
    ts: Date.now(),
  });
  if (navigator.sendBeacon) {
    navigator.sendBeacon(BEACON_URL, body);
  } else {
    fetch(BEACON_URL, { method: "POST", body, keepalive: true }).catch(() => {
      /* best-effort */
    });
  }
}

export function initVitals(): void {
  onCLS(send);
  onFCP(send);
  onINP(send);
  onLCP(send);
  onTTFB(send);
}
