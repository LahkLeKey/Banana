import { createBrowserRouter, Navigate } from "react-router-dom";
import { withErrorBoundary } from "../components/errors/withErrorBoundary";
import { CharacterSelectPage } from "../pages/CharacterSelectPage";
import { DataSciencePlaygroundPage } from "../pages/DataSciencePlaygroundPage";
import { HomePage } from "../pages/HomePage";
import { LegacyNotebookPage } from "../pages/LegacyNotebookPage";
import { LoginPage } from "../pages/LoginPage";
import { ProfilePage } from "../pages/ProfilePage";
import { WorldMapPage } from "../pages/WorldMapPage";

const SafeHomePage = withErrorBoundary(HomePage, {
  componentName: 'HomePage',
});

const SafeCharacterSelectPage = withErrorBoundary(CharacterSelectPage, {
  componentName: 'CharacterSelectPage',
});

const SafeWorldMapPage = withErrorBoundary(WorldMapPage, {
  componentName: 'WorldMapPage',
});

const SafeProfilePage = withErrorBoundary(ProfilePage, {
  componentName: 'ProfilePage',
});

const SafeDataSciencePlaygroundPage = withErrorBoundary(DataSciencePlaygroundPage, {
  componentName: 'DataSciencePlaygroundPage',
});

const SafeLegacyNotebookPage = withErrorBoundary(LegacyNotebookPage, {
  componentName: 'LegacyNotebookPage',
});

const canonicalRedirects = [
  ["/game-main-menu", "/"],
  ["/select-character", "/characters"],
  ["/marketing", "/"],
  ["/download", "/"],
] as const;

export const router = createBrowserRouter([
  {
    path: "/login",
    element: <LoginPage />,
  },
  {
    path: "/",
    element: <SafeHomePage />,
  },
  {
    path: "/characters",
    element: <SafeCharacterSelectPage />,
  },
  {
    path: "/world-map",
    element: <SafeWorldMapPage />,
  },
  {
    path: "/notebooks",
    element: <SafeDataSciencePlaygroundPage />,
  },
  {
    path: "/legacy",
    element: <SafeLegacyNotebookPage />,
  },
  {
    path: "/legacy-notebook",
    element: <SafeDataSciencePlaygroundPage />,
  },
  {
    path: "/banana-engine",
    element: <SafeDataSciencePlaygroundPage />,
  },
  {
    path: "/session-room",
    element: <SafeDataSciencePlaygroundPage />,
  },
  {
    path: "/profile",
    element: <SafeProfilePage />,
  },
  {
    path: "/account",
    element: <SafeProfilePage />,
  },
  ...canonicalRedirects.map(([path, redirectTo]) => ({
    path,
    element: <Navigate to={redirectTo} replace />,
  })),
  { path: "*", element: <Navigate to="/" replace /> },
]);
