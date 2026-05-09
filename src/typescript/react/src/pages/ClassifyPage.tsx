/**
 * ClassifyPage.tsx — primary classify route (feature 052).
 *
 * Renders the existing App component under the "/classify" path so all
 * existing functionality is preserved with zero behaviour change.
 */
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { App } from "../App";

export function ClassifyPage() {
  return (
    <div className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle className="game-display text-lg">Classification Bay</CardTitle>
          <CardDescription>
            Run live sample scans, review confidence signals, and keep operator triage in the same
            mission-control surface.
          </CardDescription>
        </CardHeader>
        <CardContent className="pt-0 text-xs uppercase tracking-[0.16em] text-muted-foreground">
          mode: live-classifier
        </CardContent>
      </Card>

      <App />
    </div>
  );
}
