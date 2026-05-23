import { createBrowserRouter, Navigate } from "react-router-dom";
import { GameEnginePage } from "@banana/ui";

export const router = createBrowserRouter([
  { path: "/", element: <Navigate to="/game-engine" replace /> },
  { path: "/session-room", element: <Navigate to="/game-engine" replace /> },
  { path: "/game-engine", element: <GameEnginePage /> },
  { path: "*", element: <Navigate to="/game-engine" replace /> },
]);
