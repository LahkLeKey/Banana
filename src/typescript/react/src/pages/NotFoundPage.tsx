/**
 * NotFoundPage.tsx — 404 fallback route (feature 052).
 */
import { Link } from "react-router-dom";
import { Button } from "../components/ui/button";

export function NotFoundPage() {
  return (
    <div className="flex h-screen flex-col items-center justify-center gap-4 p-8 text-center">
      <h1 className="text-4xl font-bold">404</h1>
      <p className="text-sm text-muted-foreground">Page not found.</p>
      <Button asChild variant="outline" size="sm">
        <Link to="/classify">Back to classify</Link>
      </Button>
    </div>
  );
}
