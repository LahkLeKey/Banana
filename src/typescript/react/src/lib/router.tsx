import { createBrowserRouter, Navigate } from "react-router-dom";
import { hasStoredAuthSession } from "@banana/ui";
import { LoginPage } from "../pages/LoginPage";
import { MarketingPage } from "../pages/MarketingPage";
import { PlayerPortalPage } from "../pages/PlayerPortalPage";

export const router = createBrowserRouter([
  {
    path: "/",
    element: <MarketingPage />,
  },
  { path: "/download", element: <PlayerPortalPage /> },
  { path: "/account", element: <PlayerPortalPage /> },
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
