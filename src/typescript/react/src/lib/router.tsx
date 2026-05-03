/**
 * router.tsx — application route tree (feature 052).
 *
 * Pages are loaded eagerly to avoid stale dynamic chunk URLs causing
 * client-side route failures after production deploys.
 */
import { createBrowserRouter, isRouteErrorResponse, Navigate, Outlet, useRouteError } from "react-router-dom";
import { Button } from "../components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { WorkspaceShell } from "../components/WorkspaceShell";
import { BananaAIPage } from "../pages/BananaAIPage";
import { ClassifyPage } from "../pages/ClassifyPage";
import { FunctionsPage } from "../pages/FunctionsPage";
import { KnowledgePage } from "../pages/KnowledgePage";
import { NotFoundPage } from "../pages/NotFoundPage";
import { OperatorPage } from "../pages/OperatorPage";
import { ReviewSpikesPage } from "../pages/ReviewSpikesPage";
import { WorkspacePage } from "../pages/WorkspacePage";

function PageShell() {
  return <Outlet />;
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
      {
        element: <WorkspaceShell />,
        children: [
          { index: true, element: <Navigate to="/workspace" replace /> },
          { path: "workspace", element: <WorkspacePage /> },
          { path: "knowledge", element: <KnowledgePage /> },
          { path: "functions", element: <FunctionsPage /> },
          { path: "banana-ai", element: <BananaAIPage /> },
          { path: "review-spikes", element: <ReviewSpikesPage /> },
          { path: "classify", element: <ClassifyPage /> },
          { path: "operator", element: <OperatorPage /> },
        ],
      },
      { path: "*", element: <NotFoundPage /> },
    ],
  },
]);
