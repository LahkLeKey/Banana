import { createBrowserRouter, Navigate } from "react-router-dom";
import { DataSciencePlaygroundPage } from "../pages/DataSciencePlaygroundPage";

const notebookAliases = [
  "/",
  "/notebooks",
  "/banana-engine",
  "/login",
  "/game-main-menu",
  "/select-character",
  "/marketing",
  "/download",
  "/account",
  "/session-room",
] as const;

export const router = createBrowserRouter([
  ...notebookAliases.map((path) => ({
    path,
    element: <DataSciencePlaygroundPage />,
  })),
  { path: "*", element: <Navigate to="/" replace /> },
]);
