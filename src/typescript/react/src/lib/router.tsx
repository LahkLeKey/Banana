/**
 * router.tsx — application route tree (feature 052).
 *
 * Pages are loaded eagerly to avoid stale dynamic chunk URLs causing
 * client-side route failures after production deploys.
 */
import {
  createBrowserRouter,
  isRouteErrorResponse,
  Navigate,
  Outlet,
  useLocation,
  useNavigationType,
  useRouteError,
} from "react-router-dom";
import { Analytics } from "@vercel/analytics/react";
import { SpeedInsights } from "@vercel/speed-insights/react";
import { useEffect } from "react";
import { Button } from "../components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { isAnalyticsEnabled, trackPageView } from "./analytics";
import { GameEnginePage } from "../pages/GameEnginePage";
import { SessionRoomPage } from "../pages/SessionRoomPage";

const legacyRoutes = [
  "landing",
  "workspace",
  "knowledge",
  "functions",
  "api-docs",
  "data-science",
  "banana-ai",
  "quiz-qa",
  "telemetry",
  "review-spikes",
  "classify",
  "operator",
];

function PageShell() {
  const location = useLocation();
  const navigationType = useNavigationType();

  useEffect(() => {
    trackPageView(location.pathname, navigationType);
  }, [location.pathname, navigationType]);

  return (
    <>
      <Outlet />
      {isAnalyticsEnabled() ? <Analytics /> : null}
      {isAnalyticsEnabled() ? <SpeedInsights /> : null}
    </>
  );
}

function RouteErrorBoundary() {
  const error = useRouteError();
  const message = isRouteErrorResponse(error)
    ? `${error.status} ${error.statusText}`
    : error instanceof Error
      ? error.message
      : "An unexpected route error occurred.";

  return (
    <main className="mx-auto max-w-3xl p-6" data-testid="route-error-boundary">
      <Card>
        <CardHeader>
          <CardTitle>Page load interrupted</CardTitle>
          <CardDescription>
            A stale app chunk or transient network failure prevented this route from loading.
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-3">
          <p className="text-sm text-muted-foreground" data-testid="route-error-boundary-message">
            {message}
          </p>
          <Button onClick={() => window.location.reload()}>Reload app</Button>
        </CardContent>
      </Card>
    </main>
  );
}

export const router = createBrowserRouter([
  {
    element: <PageShell />,
    errorElement: <RouteErrorBoundary />,
    children: [
      { index: true, element: <Navigate to="/session-room" replace /> },
      { path: "session-room", element: <SessionRoomPage /> },
      { path: "game-engine", element: <GameEnginePage /> },
      ...legacyRoutes.map((path) => ({ path, element: <Navigate to="/" replace /> })),
      { path: "*", element: <Navigate to="/" replace /> },
    ],
  },
]);
