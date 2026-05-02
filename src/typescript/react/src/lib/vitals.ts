import type {Metric} from 'web-vitals';

// Reports Core Web Vitals to a configurable beacon endpoint.
// Uses the web-vitals library (must be installed: bun add web-vitals).
export function reportWebVitals(onPerfEntry?: (metric: Metric) => void): void {
  if (!onPerfEntry) return;
  import('web-vitals').then(({onCLS, onFCP, onINP, onLCP, onTTFB}) => {
    onCLS(onPerfEntry);
    onFCP(onPerfEntry);
    onINP(onPerfEntry);
    onLCP(onPerfEntry);
    onTTFB(onPerfEntry);
  });
}

export function sendToBeacon(metric: Metric): void {
  const endpoint = import.meta.env.VITE_RUM_BEACON_URL;
  if (!endpoint) return;
  const body = JSON.stringify({
    name: metric.name,
    value: metric.value,
    rating: metric.rating,
    delta: metric.delta,
    id: metric.id,
    navigationType: metric.navigationType,
  });
  (navigator.sendBeacon && navigator.sendBeacon(endpoint, body)) ||
    fetch(endpoint, {body, method: 'POST', keepalive: true});
}
