import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import { RouterProvider } from "react-router-dom";
import { ErrorBoundary } from "./components/ErrorBoundary";
import { QueryProvider } from "./lib/queryClient";
import { router } from "./lib/router";
import { initSentry } from "./lib/sentry";
import { initVitals } from "./lib/vitals";
import "./index.css";

initSentry();
initVitals();

createRoot(document.getElementById("root")!).render(
    <StrictMode>
        <ErrorBoundary>
            <QueryProvider>
                <RouterProvider router={router} />
            </QueryProvider>
        </ErrorBoundary>
    </StrictMode>
);
