export type LaunchMode = 'development'|'staging-uat'|'production-steam-package';

export type EnvironmentModePolicy = {
  mode: LaunchMode; strictnessLevel: number; allowNonSteamStartup: boolean;
  allowCachedVerification: boolean;
  allowOverrideContext: boolean;
};

export const ENVIRONMENT_MODE_POLICIES:
    Record<LaunchMode, EnvironmentModePolicy> = {
      development: {
        mode: 'development',
        strictnessLevel: 1,
        allowNonSteamStartup: true,
        allowCachedVerification: true,
        allowOverrideContext: true,
      },
      'staging-uat': {
        mode: 'staging-uat',
        strictnessLevel: 2,
        allowNonSteamStartup: false,
        allowCachedVerification: true,
        allowOverrideContext: true,
      },
      'production-steam-package': {
        mode: 'production-steam-package',
        strictnessLevel: 3,
        allowNonSteamStartup: false,
        allowCachedVerification: false,
        allowOverrideContext: false,
      },
    };

export function resolveEnvironmentModePolicy(modeRaw: string):
    EnvironmentModePolicy|null {
  const mode = modeRaw.trim() as LaunchMode;
  if (!(mode in ENVIRONMENT_MODE_POLICIES)) {
    return null;
  }
  return ENVIRONMENT_MODE_POLICIES[mode];
}
