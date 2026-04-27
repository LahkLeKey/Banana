export type EventWithNavigationGuards = {
  preventDefault?: () => void;
  stopPropagation?: () => void;
};

export function preventNativeSubmitNavigation(
    event?: EventWithNavigationGuards): void {
  event?.preventDefault?.();
  event?.stopPropagation?.();
}

export function normalizeEnsembleSample(rawInput: string): string {
  return rawInput.trim();
}

export function canSubmitEnsemble(
    apiBaseUrl: string,
    sample: string,
    isPredictingEnsemble: boolean,
    ): boolean {
  if (isPredictingEnsemble) {
    return false;
  }

  return apiBaseUrl.length > 0 && sample.length > 0;
}
