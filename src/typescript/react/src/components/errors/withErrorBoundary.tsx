import {
    Component,
    type ComponentType,
    type ErrorInfo,
    type PropsWithChildren,
    type ReactNode,
} from 'react';

import { reportComponentErrorTelemetry, resolveApiBaseUrl } from '../../lib/api';

type BoundaryFallbackRenderContext = {
    componentName: string;
    error: Error;
};

type WithErrorBoundaryOptions<P> = {
    componentName?: string;
    fallback?: ReactNode | ((context: BoundaryFallbackRenderContext) => ReactNode);
    telemetryMetadata?: (props: Readonly<P>) => Record<string, unknown>;
};

type BoundaryState = {
    hasError: boolean;
    error: Error | null;
};

function defaultFallback(context: BoundaryFallbackRenderContext): ReactNode {
    return (
        <section style={{
            borderRadius: 12,
            border: '1px solid rgba(248, 113, 113, 0.45)',
            background: 'rgba(30, 10, 12, 0.88)',
            color: '#fecaca',
            padding: 14,
            fontFamily: '"IBM Plex Sans", "Segoe UI", sans-serif',
        }}>
            <h2 style={{ margin: '0 0 8px', fontSize: 16 }}>Component Boundary Triggered</h2>
            <p style={{ margin: '0 0 8px', fontSize: 13, color: '#fda4af' }}>
                {context.componentName}
            </p>
            <p style={{ margin: 0, fontSize: 12, lineHeight: 1.5 }}>
                {context.error.message}
            </p>
        </section>
    );
}

export function withErrorBoundary<P extends object>(
    WrappedComponent: ComponentType<P>,
    options: WithErrorBoundaryOptions<P> = {}): ComponentType<P> {
    const wrappedName =
        options.componentName ?? WrappedComponent.displayName ??
        WrappedComponent.name ?? 'AnonymousComponent';

    class ErrorBoundaryWrapper extends Component<PropsWithChildren<P>, BoundaryState> {
        state: BoundaryState = {
            hasError: false,
            error: null,
        };

        static getDerivedStateFromError(error: Error): BoundaryState {
            return { hasError: true, error };
        }

        componentDidCatch(error: Error, info: ErrorInfo): void {
            const apiBaseUrl = resolveApiBaseUrl();
            if (apiBaseUrl.trim().length === 0) {
                return;
            }

            const metadata = options.telemetryMetadata ?
                options.telemetryMetadata(this.props) : undefined;

            void reportComponentErrorTelemetry(apiBaseUrl, {
                componentName: wrappedName,
                message: error.message,
                stack: error.stack,
                componentStack: info.componentStack ?? undefined,
                routePath: typeof window !== 'undefined' ? window.location.pathname : '',
                userAgent: typeof navigator !== 'undefined' ? navigator.userAgent : '',
                timestamp: Date.now(),
                metadata,
            }).catch(() => {
                // Boundary telemetry failures must never throw into render flow.
            });
        }

        render(): ReactNode {
            if (this.state.hasError && this.state.error) {
                const fallback = options.fallback ?? defaultFallback;
                return typeof fallback === 'function' ?
                    fallback({ componentName: wrappedName, error: this.state.error }) :
                    fallback;
            }

            return <WrappedComponent {...this.props} />;
        }
    }

    const boundaryComponent = ErrorBoundaryWrapper as unknown as ComponentType<P>;
    (boundaryComponent as { displayName?: string }).displayName =
        `WithErrorBoundary(${wrappedName})`;
    return boundaryComponent;
}