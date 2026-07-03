import { createBrowserRouter, Navigate } from "react-router-dom";
import { withErrorBoundary } from "../components/errors/withErrorBoundary";
import { DataSciencePlaygroundPage } from "../pages/DataSciencePlaygroundPage";
import { LoginPage } from "../pages/LoginPage";
import { ProfilePage } from "../pages/ProfilePage";

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
  "/game-main-menu",
  "/select-character",
  "/marketing",
  "/download",
  "/session-room",
] as const;

export const router = createBrowserRouter([
  {
    path: "/login",
    element: <LoginPage />,
  },
  {
    path: "/profile",
    element: <ProfilePage />,
  },
  {
    path: "/account",
    element: <ProfilePage />,
  },
  ...notebookAliases.map((path) => ({
    path,
    element: <SafeDataSciencePlaygroundPage />,
  })),
  { path: "*", element: <Navigate to="/" replace /> },
]);
