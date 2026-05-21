import type { FastifyInstance } from "fastify";

// OpenFeature-compatible flag store backed by env vars (v1).
// Swap to LaunchDarkly / Flagsmith by changing the provider.
export type FlagKey =
  | "streaming-verdict-enabled"
  | "ab-harness-enabled"
  | "drift-detection-enabled";

export function isEnabled(flag: FlagKey): boolean {
  const envKey = `BANANA_FLAG_${flag.toUpperCase().replace(/-/g, "_")}`;
  const val = process.env[envKey];
  if (val === undefined) return false;
  return val === "1" || val.toLowerCase() === "true";
}

export async function registerFeatureFlagsPlugin(app: FastifyInstance): Promise<void> {
  app.decorate("flags", { isEnabled });
  // GET /operator/flags - returns current flag state
  app.get("/operator/flags", async () => {
    const flags: Record<FlagKey, boolean> = {
      "streaming-verdict-enabled": isEnabled("streaming-verdict-enabled"),
      "ab-harness-enabled": isEnabled("ab-harness-enabled"),
      "drift-detection-enabled": isEnabled("drift-detection-enabled"),
    };
    return { flags };
  });
}
