import { Component, type ErrorInfo, type ReactNode } from "react";

import { Alert, AlertDescription, AlertTitle } from "./ui/alert";
import { Button } from "./ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "./ui/card";

export interface ErrorBoundaryProps {
  children: ReactNode;
  fallback?: (error: Error, reset: () => void) => ReactNode;
}

interface ErrorBoundaryState {
  error: Error | null;
}

export class ErrorBoundary extends Component<ErrorBoundaryProps, ErrorBoundaryState> {
  state: ErrorBoundaryState = { error: null };

  static getDerivedStateFromError(error: Error): ErrorBoundaryState {
    return { error };
  }

  componentDidCatch(error: Error, _info: ErrorInfo): void {
    // Errors are visualized via the fallback render; consumers can wire
    // additional reporting via the `fallback` prop if needed.
  }

  private reset = (): void => {
    this.setState({ error: null });
  };

  render(): ReactNode {
    const { error } = this.state;
    if (error !== null) {
      if (this.props.fallback) {
        return this.props.fallback(error, this.reset);
      }
      return (
        <main className="mx-auto max-w-3xl p-6" data-testid="app-error-boundary">
          <Card>
            <CardHeader>
              <CardTitle>Something interrupted the page.</CardTitle>
            </CardHeader>
            <CardContent className="space-y-3">
              <Alert variant="destructive">
                <AlertTitle>Render error</AlertTitle>
                <AlertDescription data-testid="app-error-boundary-message">
                  {error.message}
                </AlertDescription>
              </Alert>
              <Button onClick={this.reset} data-testid="app-error-boundary-retry">
                Try again
              </Button>
            </CardContent>
          </Card>
        </main>
      );
    }
    return this.props.children;
  }
}
