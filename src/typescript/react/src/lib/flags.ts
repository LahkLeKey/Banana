// Client-side feature flag helper.
// Reads from VITE_* env vars for compile-time flags,
// or fetches from /operator/flags for runtime flags.
export type FlagKey = 'streaming-verdict-enabled' | 'ab-harness-enabled';

export function isFlagEnabled(flag: FlagKey): boolean {
  const envKey = `VITE_FLAG_${flag.toUpperCase().replace(/-/g, '_')}`;
  return import.meta.env[envKey] === 'true' || import.meta.env[envKey] === '1';
}
