import { createBrowserRouter, Navigate } from "react-router-dom";
import { hasStoredAuthSession } from "@banana/ui";
import type { ReactElement } from "react";
import { hasSelectedCharacter } from "./gameClientFlow";
import { BananaEngineBootPage } from "../pages/BananaEngineBootPage";
import { CharacterSelectPage } from "../pages/CharacterSelectPage";
import { DataSciencePlaygroundPage } from "../pages/DataSciencePlaygroundPage";
import { GameMainMenuPage } from "../pages/GameMainMenuPage";
import { LoginPage } from "../pages/LoginPage";
import { MarketingPage } from "../pages/MarketingPage";
import { PlayerPortalPage } from "../pages/PlayerPortalPage";
import { SplashPage } from "../pages/SplashPage";
import { SessionRoomPage } from "../pages/SessionRoomPage";

function RequireSession({ children }: { children: ReactElement }) {
  return hasStoredAuthSession() ? children : <Navigate to="/login" replace />;
}

function RequireSessionAndCharacter({ children }: { children: ReactElement }) {
  if (!hasStoredAuthSession()) {
    return <Navigate to="/login" replace />;
  }
  return hasSelectedCharacter() ? children : <Navigate to="/select-character" replace />;
}

export const router = createBrowserRouter([
  {
    path: "/",
    element: <SplashPage />,
  },
  { path: "/banana-engine", element: <BananaEngineBootPage /> },
  { path: "/login", element: <LoginPage /> },
  {
    path: "/game-main-menu",
    element: <RequireSession><GameMainMenuPage /></RequireSession>,
  },
  {
    path: "/select-character",
    element: <RequireSession><CharacterSelectPage /></RequireSession>,
  },
  {
    path: "/notebooks",
    element: <RequireSessionAndCharacter><DataSciencePlaygroundPage /></RequireSessionAndCharacter>,
  },
  { path: "/marketing", element: <MarketingPage /> },
  { path: "/download", element: <PlayerPortalPage /> },
  { path: "/account", element: <PlayerPortalPage /> },
  {
    path: "/session-room",
    element: <RequireSession><Navigate to="/game-main-menu" replace /></RequireSession>,
  },
  { path: "*", element: <Navigate to="/" replace /> },
]);
