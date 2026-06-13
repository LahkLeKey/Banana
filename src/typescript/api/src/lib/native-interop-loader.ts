import {dlopen} from 'bun:ffi';

import {resolveBananaNativeLibraryCandidates, type NativeLibraryCandidateOptions,} from './native-library-candidates';

type DlopenSymbolDefinition = Parameters<typeof dlopen>[1];

type LoadNativeSymbolsOptions = NativeLibraryCandidateOptions;

export function loadBananaNativeSymbols<T>(
    symbolDefinitions: DlopenSymbolDefinition,
    featureLabel: string,
    options: LoadNativeSymbolsOptions = {},
): T {
  const candidates = resolveBananaNativeLibraryCandidates(options);
  let lastError: unknown = null;

  for (const candidate of candidates) {
    try {
      const library = dlopen(candidate, symbolDefinitions);
      return library.symbols as unknown as T;
    } catch (error) {
      lastError = error;
    }
  }

  throw new Error(
      `Unable to load Banana native library for ${featureLabel}. Candidates: ${
          candidates.join(', ')}. Last error: ${String(lastError)}`,
  );
}
