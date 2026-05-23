import { createBrowserRouter, Navigate } from "react-router-dom";
import { hasStoredAuthSession } from "@banana/ui";
import { LoginPage } from "../pages/LoginPage";
import { MarketingPage } from "../pages/MarketingPage";

export const router = createBrowserRouter([
  {
    path: "/",
    element: <MarketingPage />,
  },
  { path: "/download", element: <MarketingPage /> },
  { path: "/login", element: <LoginPage /> },
  {
    path: "/session-room",
    element: <Navigate to={hasStoredAuthSession() ? "/download" : "/login"} replace />,
  },
  {
    path: "/game-engine",
    element: <Navigate to="/download" replace />,
  },
  { path: "*", element: <Navigate to="/" replace /> },
]);
