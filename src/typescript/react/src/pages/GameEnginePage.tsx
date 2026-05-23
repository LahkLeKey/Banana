import { GameEnginePage as SharedGameEnginePage } from "@banana/ui";

/**
 * Compatibility wrapper for the shared @banana/ui engine page.
 * Keeps React route imports stable while centralizing implementation.
 */
export function GameEnginePage() {
  return <SharedGameEnginePage />;
}
