import { track } from "@vercel/analytics";

const ANALYTICS_ENABLED =
    import.meta.env.PROD || import.meta.env.VITE_ENABLE_ANALYTICS === "true";

export function isAnalyticsEnabled(): boolean {
    return ANALYTICS_ENABLED;
}

export function trackEvent(name: string, properties: Record<string, string | number | boolean> = {}): void {
    if (!ANALYTICS_ENABLED || typeof window === "undefined") {
        return;
    }

    track(name, properties);
}

export function trackPageView(pathname: string, navigationType: string): void {
    trackEvent("banana_page_view", {
        pathname,
        navigationType,
    });
}
