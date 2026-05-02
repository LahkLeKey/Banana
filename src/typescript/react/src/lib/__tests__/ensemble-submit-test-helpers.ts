export function getLocationFingerprint(
  location: Pick<Location, "origin" | "pathname" | "search"> = window.location
): string {
  return `${location.origin}${location.pathname}${location.search}`;
}

export function didLocationChange(before: string, after: string): boolean {
  return before !== after;
}
