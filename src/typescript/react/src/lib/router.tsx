/**
 * router.tsx — application route tree (feature 052).
 *
 * All routes are code-split via React.lazy + Suspense. The root "/" redirects
 * to "/classify" so existing deep-links keep working.
 */
import { lazy, Suspense } from "react";
import { createBrowserRouter, Navigate, Outlet } from "react-router-dom";

const ClassifyPage = lazy(() =>
  import("../pages/ClassifyPage").then((m) => ({ default: m.ClassifyPage }))
);
const OperatorPage = lazy(() =>
  import("../pages/OperatorPage").then((m) => ({ default: m.OperatorPage }))
);
const NotFoundPage = lazy(() =>
  import("../pages/NotFoundPage").then((m) => ({ default: m.NotFoundPage }))
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
      { index: true, element: <Navigate to="/classify" replace /> },
      { path: "classify", element: <ClassifyPage /> },
      { path: "operator", element: <OperatorPage /> },
      { path: "*", element: <NotFoundPage /> },
    ],
  },
]);
