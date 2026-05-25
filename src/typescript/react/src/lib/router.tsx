import { createBrowserRouter, Navigate } from "react-router-dom";
import { hasStoredAuthSession } from "@banana/ui";
import { LoginPage } from "../pages/LoginPage";
import { MarketingPage } from "../pages/MarketingPage";
import { PlayerPortalPage } from "../pages/PlayerPortalPage";
import { SessionRoomPage } from "../pages/SessionRoomPage";

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
    element: hasStoredAuthSession() ? <SessionRoomPage /> : <Navigate to="/login" replace />,
  },
  { path: "*", element: <Navigate to="/" replace /> },
]);
