import { createBrowserRouter, Navigate } from "react-router-dom";
import { withErrorBoundary } from "../components/errors/withErrorBoundary";
import { DataSciencePlaygroundPage } from "../pages/DataSciencePlaygroundPage";

const SafeDataSciencePlaygroundPage = withErrorBoundary(
  DataSciencePlaygroundPage,
  {
    componentName: 'DataSciencePlaygroundPage',
  },
);

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
    element: <SafeDataSciencePlaygroundPage />,
  })),
  { path: "*", element: <Navigate to="/" replace /> },
]);
