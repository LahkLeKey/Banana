export type LocationSnapshot = {
  origin: string; pathname: string; search: string; hash: string;
};

export type BlankPageClassification = 'stable'|'navigation';

export function snapshotLocation(
    source: Pick<Location, 'origin'|'pathname'|'search'|'hash'>|null =
        typeof window !== 'undefined' ? window.location : null,
    ): LocationSnapshot {
  if (!source) {
    return {
      origin: '',
      pathname: '',
      search: '',
      hash: '',
    };
  }

  return {
    origin: source.origin,
    pathname: source.pathname,
    search: source.search,
    hash: source.hash,
  };
}

export function classifyPotentialBlankPage(
    before: LocationSnapshot,
    after: LocationSnapshot,
    ): BlankPageClassification {
  if (before.origin !== after.origin || before.pathname !== after.pathname ||
      before.search !== after.search) {
    return 'navigation';
  }

  return 'stable';
}
