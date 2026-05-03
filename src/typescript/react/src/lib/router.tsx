/**
 * router.tsx — application route tree (feature 052).
 *
 * All routes are code-split via React.lazy + Suspense. The root "/" redirects
 * to "/classify" so existing deep-links keep working.
 */
import { lazy, Suspense } from "react";
import { createBrowserRouter, Navigate, Outlet } from "react-router-dom";
import { WorkspaceShell } from "../components/WorkspaceShell";

const ClassifyPage = lazy(() =>
  import("../pages/ClassifyPage").then((m) => ({ default: m.ClassifyPage }))
);
const OperatorPage = lazy(() =>
  import("../pages/OperatorPage").then((m) => ({ default: m.OperatorPage }))
);
const NotFoundPage = lazy(() =>
  import("../pages/NotFoundPage").then((m) => ({ default: m.NotFoundPage }))
);
const WorkspacePage = lazy(() =>
  import("../pages/WorkspacePage").then((m) => ({ default: m.WorkspacePage }))
);
const KnowledgePage = lazy(() =>
  import("../pages/KnowledgePage").then((m) => ({ default: m.KnowledgePage }))
);
const FunctionsPage = lazy(() =>
  import("../pages/FunctionsPage").then((m) => ({ default: m.FunctionsPage }))
);
const BananaAIPage = lazy(() =>
  import("../pages/BananaAIPage").then((m) => ({ default: m.BananaAIPage }))
);
const ReviewSpikesPage = lazy(() =>
  import("../pages/ReviewSpikesPage").then((m) => ({ default: m.ReviewSpikesPage }))
);

function PageShell() {
  return (
    <Suspense
      fallback={
        <div className="flex h-screen items-center justify-center text-sm text-muted-foreground">
          Loading…
        </div>
      }
    >
      <Outlet />
    </Suspense>
  );
}

export const router = createBrowserRouter([
  {
    element: <PageShell />,
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
