export type MissionDomain = 'AI'|'Physics'|'Render'|'World'|'Runtime'|'Other';

export type MissionMeta = {
  readonly title: string; readonly domain: MissionDomain; readonly code: string;
};

export function deriveMissionMeta(file: string): MissionMeta {
  if (!file) {
    return {title: 'Unassigned Sector', domain: 'Other', code: 'OT-000'};
  }

  const normalized = file.replace(/\\/g, '/');
  const lane = normalized.split('/').filter(Boolean).pop() ?? normalized;
  const title = lane.replace(/\.[^.]+$/, '')
                    .replace(/[_-]+/g, ' ')
                    .replace(/\b\w/g, (match) => match.toUpperCase());

  let domain: MissionDomain = 'Other';
  if (normalized.includes('/ai/')) {
    domain = 'AI';
  } else if (normalized.includes('/physics/')) {
    domain = 'Physics';
  } else if (normalized.includes('/render/')) {
    domain = 'Render';
  } else if (normalized.includes('/world/')) {
    domain = 'World';
  } else if (normalized.includes('/runtime/')) {
    domain = 'Runtime';
  }

  const checksum = Math.abs(normalized.split('').reduce(
                       (acc, char) => acc + char.charCodeAt(0), 0)) %
      1000;
  return {
    title,
    domain,
    code: `${domain.slice(0, 2).toUpperCase()}-${
        String(checksum).padStart(3, '0')}`,
  };
}

export function normalizeDependencyLabel(rawTarget: string): string {
  const cleaned = rawTarget.replace(/["<>]/g, '').split('/').pop() ?? rawTarget;
  return cleaned.replace(/\.[^.]+$/, '')
      .replace(/[_-]+/g, ' ')
      .replace(/\b\w/g, (match) => match.toUpperCase());
}
