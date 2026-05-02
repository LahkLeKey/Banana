/**
 * ProtectedRoute.tsx — Role-gated route wrapper (feature 063).
 *
 * Usage in router:
 *   { path: '/operator', element: <ProtectedRoute minRole="operator"><OperatorPage /></ProtectedRoute> }
 */
import type { ReactNode } from "react";
import { Navigate } from "react-router-dom";
import { useAuth, type BananaRole } from "../lib/hooks/useAuth";

interface ProtectedRouteProps {
  children: ReactNode;
  minRole?: BananaRole;
}

const ROLE_WEIGHT: Record<BananaRole, number> = {
  admin: 30,
  operator: 20,
  viewer: 10,
};

export function ProtectedRoute({ children, minRole = "viewer" }: ProtectedRouteProps) {
  const { token, role } = useAuth();

  if (!token) {
    return <Navigate to="/login" replace />;
  }

  const userWeight = role ? (ROLE_WEIGHT[role] ?? 0) : 0;
  if (userWeight < ROLE_WEIGHT[minRole]) {
    return <Navigate to="/unauthorized" replace />;
  }

  return <>{children}</>;
}
