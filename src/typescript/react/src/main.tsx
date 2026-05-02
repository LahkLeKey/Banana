import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import { App } from "./App";
import { ErrorBoundary } from "./components/ErrorBoundary";
import { QueryProvider } from "./lib/queryClient";
import "./index.css";

createRoot(document.getElementById("root")!).render(
    <StrictMode>
        <ErrorBoundary>
            <QueryProvider>
                <App />
            </QueryProvider>
        </ErrorBoundary>
    </StrictMode>,
);
