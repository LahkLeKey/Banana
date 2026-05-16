/**
 * Domain-Driven Design: Viewport Domain
 *
 * Manages responsive viewport sizing, device pixel ratio, and frame rate adaptation.
 * Follows Dependency Inversion: accepts viewport element as dependency, returns pure sizing
 * calculations. Follows Single Responsibility: only manages viewport geometry and sizing logic.
 */

/**
 * Value Object: Represents viewport dimensions in both CSS and device pixel space
 * Immutable and comparable
 */
export interface ViewportSize {
    readonly cssWidth: number;
    readonly cssHeight: number;
    readonly pixelWidth: number;
    readonly pixelHeight: number;
    readonly devicePixelRatio: number;
}

/**
 * Entity: Manages viewport sizing with fallback chains for mobile robustness
 * Resolves browser inconsistencies (chrome transitions, visualViewport API, etc.)
 */
export class ViewportSizer
{
    private element: HTMLElement|null = null;
    private lastSize: ViewportSize|null = null;

    /**
     * Attach to a viewport container element
     * @param element - DOM element to measure
     */
    attachTo(element: HTMLElement): void
    {
        this.element = element;
    }

    /**
     * Compute viewport size with mobile-safe fallback chain
     * 1. Try element bounding rect
     * 2. Fall back to visualViewport API (mobile chrome transitions)
     * 3. Fall back to window.innerWidth/innerHeight
     * 4. Fall back to documentElement.clientWidth/clientHeight
     * 5. Minimum 1x1 to prevent canvas collapse
     */
    computeSize(): ViewportSize
    {
        const dpr = Math.max(1, window.devicePixelRatio || 1);

        // Try element bounding rect first
        if (this.element)
        {
            const bounds = this.element.getBoundingClientRect();
            if (bounds.width > 1 && bounds.height > 1)
            {
                const cssWidth = Math.floor(bounds.width);
                const cssHeight = Math.floor(bounds.height);
                const pixelWidth = Math.max(1, Math.floor(cssWidth * dpr));
                const pixelHeight = Math.max(1, Math.floor(cssHeight * dpr));
                return {
                    cssWidth,
                    cssHeight,
                    pixelWidth,
                    pixelHeight,
                    devicePixelRatio : dpr,
                };
            }
        }

        // Fall back to visualViewport (mobile chrome URL bar transitions)
        const visualViewport = window.visualViewport;
        if (visualViewport && visualViewport.width > 1 && visualViewport.height > 1)
        {
            const cssWidth = Math.floor(visualViewport.width);
            const cssHeight = Math.floor(visualViewport.height);
            const pixelWidth = Math.max(1, Math.floor(cssWidth * dpr));
            const pixelHeight = Math.max(1, Math.floor(cssHeight * dpr));
            return {
                cssWidth,
                cssHeight,
                pixelWidth,
                pixelHeight,
                devicePixelRatio : dpr,
            };
        }

        // Fall back to window dimensions
        if (window.innerWidth > 1 && window.innerHeight > 1)
        {
            const cssWidth = Math.floor(window.innerWidth);
            const cssHeight = Math.floor(window.innerHeight);
            const pixelWidth = Math.max(1, Math.floor(cssWidth * dpr));
            const pixelHeight = Math.max(1, Math.floor(cssHeight * dpr));
            return {
                cssWidth,
                cssHeight,
                pixelWidth,
                pixelHeight,
                devicePixelRatio : dpr,
            };
        }

        // Final fallback to documentElement
        const clientWidth = document.documentElement.clientWidth || 1;
        const clientHeight = document.documentElement.clientHeight || 1;
        const cssWidth = Math.max(1, clientWidth);
        const cssHeight = Math.max(1, clientHeight);
        const pixelWidth = Math.max(1, Math.floor(cssWidth * dpr));
        const pixelHeight = Math.max(1, Math.floor(cssHeight * dpr));

        return {
            cssWidth,
            cssHeight,
            pixelWidth,
            pixelHeight,
            devicePixelRatio : dpr,
        };
    }

    /**
     * Check if size has changed significantly
     * @param threshold - Pixel threshold to consider a real change (default 2px)
     * @returns true if size changed by more than threshold
     */
    hasSizeChanged(threshold = 2): boolean
    {
        const current = this.computeSize();

        if (!this.lastSize)
        {
            this.lastSize = current;
            return true;
        }

        const pixelDeltaX = Math.abs(current.pixelWidth - this.lastSize.pixelWidth);
        const pixelDeltaY = Math.abs(current.pixelHeight - this.lastSize.pixelHeight);

        const changed = pixelDeltaX > threshold || pixelDeltaY > threshold;

        if (changed)
        {
            this.lastSize = current;
        }

        return changed;
    }

    /**
     * Get the last computed size
     */
    getLastSize(): ViewportSize|null
    {
        return this.lastSize;
    }

    /**
     * Normalize a client-space coordinate to viewport-relative [0, 1]
     */
    normalizeCoordinate(clientX: number,
                        clientY: number): {normalizedX: number; normalizedY : number}
    {
        const size = this.computeSize();
        return {
            normalizedX : size.cssWidth > 0 ? clientX / size.cssWidth : 0,
            normalizedY : size.cssHeight > 0 ? clientY / size.cssHeight : 0,
        };
    }
}

/**
 * Factory for creating a viewport sizer attached to an element
 */
export function createViewportSizer(element: HTMLElement): ViewportSizer
{
    const sizer = new ViewportSizer();
    sizer.attachTo(element);
    return sizer;
}
