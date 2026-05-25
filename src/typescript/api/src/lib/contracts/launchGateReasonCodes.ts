export const LAUNCH_GATE_REASON_CODES = [
  'OK',
  'UNLINKED_ACCOUNT',
  'SUSPENDED_OR_RESTRICTED',
  'STALE_SESSION',
  'STEAM_UNAVAILABLE',
  'API_UNAVAILABLE',
  'OFFLINE_UNVERIFIABLE',
  'UNKNOWN_MODE',
] as const;

export type LaunchGateReasonCode = typeof LAUNCH_GATE_REASON_CODES[number];

export const REMEDIABLE_LAUNCH_GATE_REASON_CODES:
    ReadonlySet<LaunchGateReasonCode> = new Set([
      'UNLINKED_ACCOUNT',
      'STALE_SESSION',
      'STEAM_UNAVAILABLE',
      'API_UNAVAILABLE',
      'OFFLINE_UNVERIFIABLE',
    ]);

export function isLaunchGateReasonCode(value: string):
    value is LaunchGateReasonCode {
  return (LAUNCH_GATE_REASON_CODES as readonly string[]).includes(value);
}

export function isRemediableLaunchGateReason(reasonCode: LaunchGateReasonCode):
    boolean {
  return REMEDIABLE_LAUNCH_GATE_REASON_CODES.has(reasonCode);
}
