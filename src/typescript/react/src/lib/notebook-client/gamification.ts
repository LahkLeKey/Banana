export type SectorDomain =
    |'ai'|'physics'|'render'|'world'|'runtime'|'ui'|'tools'|'other';

export type SectorRarity = {
  label: 'Common'|'Rare'|'Epic'|'Legendary'; multiplier: number; accent: string;
};

export type QuestProgress = {
  id: string; title: string; progress: number; target: number;
  completed: boolean;
};

function clamp(value: number, min: number, max: number): number {
  return Math.max(min, Math.min(max, value));
}

export function deriveSectorDomain(filePath: string): SectorDomain {
  const normalized = filePath.toLowerCase();
  if (normalized.includes('/ai/')) {
    return 'ai';
  }
  if (normalized.includes('/physics/')) {
    return 'physics';
  }
  if (normalized.includes('/render/')) {
    return 'render';
  }
  if (normalized.includes('/world/')) {
    return 'world';
  }
  if (normalized.includes('/runtime/')) {
    return 'runtime';
  }
  if (normalized.includes('/ui/')) {
    return 'ui';
  }
  if (normalized.includes('/tools/')) {
    return 'tools';
  }
  return 'other';
}

export function deriveSectorRarity(
    filePath: string, lineCount: number): SectorRarity {
  const normalized = filePath.toLowerCase();

  if (normalized.includes('/runtime/engine/') ||
      normalized.includes('/world/') || lineCount >= 280) {
    return {label: 'Legendary', multiplier: 2.3, accent: '#f59e0b'};
  }

  if (normalized.includes('/render/') || normalized.includes('/physics/') ||
      lineCount >= 180) {
    return {label: 'Epic', multiplier: 1.7, accent: '#a78bfa'};
  }

  if (normalized.includes('/ai/') || normalized.includes('/runtime/') ||
      lineCount >= 100) {
    return {label: 'Rare', multiplier: 1.35, accent: '#22d3ee'};
  }

  return {label: 'Common', multiplier: 1, accent: '#86efac'};
}

export function calculateSectorXp(
    lineCount: number,
    rarityMultiplier: number,
    comboStreak: number,
    firstCapture: boolean,
    ): number {
  const baseXp = 30 + Math.floor(lineCount / 5);
  const rarityXp = Math.round(baseXp * rarityMultiplier);
  const comboBonus = Math.max(0, comboStreak - 1) * 16;
  const noveltyBonus = firstCapture ? 45 : 12;
  return clamp(rarityXp + comboBonus + noveltyBonus, 24, 680);
}

export function calculateLevel(totalXp: number): number {
  return Math.max(1, Math.floor(Math.sqrt(totalXp / 120)) + 1);
}

export function buildQuestProgress(capturedFiles: string[]): QuestProgress[] {
  const domains = new Set<SectorDomain>();
  for (const file of capturedFiles) {
    domains.add(deriveSectorDomain(file));
  }

  const coreTargets: SectorDomain[] = ['ai', 'physics', 'render', 'world'];
  const coveredCore =
      coreTargets.filter((domain) => domains.has(domain)).length;
  const runtimeCovered = domains.has('runtime') ? 1 : 0;

  return [
    {
      id: 'core-systems',
      title: 'Scout all core systems',
      progress: coveredCore,
      target: coreTargets.length,
      completed: coveredCore >= coreTargets.length,
    },
    {
      id: 'runtime-bridge',
      title: 'Secure runtime bridge',
      progress: runtimeCovered,
      target: 1,
      completed: runtimeCovered >= 1,
    },
    {
      id: 'cartographer',
      title: 'Capture 12 sectors',
      progress: Math.min(capturedFiles.length, 12),
      target: 12,
      completed: capturedFiles.length >= 12,
    },
  ];
}

export function deriveNextCoreDomainTarget(capturedFiles: string[]):
    SectorDomain {
  const domains = new Set<SectorDomain>();
  for (const file of capturedFiles) {
    domains.add(deriveSectorDomain(file));
  }

  const orderedTargets: SectorDomain[] =
      ['ai', 'physics', 'render', 'world', 'runtime'];
  const next = orderedTargets.find((domain) => !domains.has(domain));
  return next ?? 'runtime';
}