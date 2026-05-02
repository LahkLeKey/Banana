import crypto from 'node:crypto';

export type AbGroup = 'A' | 'B';

interface AbStats {
  count: number;
  scoreSum: number;
}

const groupStats: Record<AbGroup, AbStats> = {
  A: { count: 0, scoreSum: 0 },
  B: { count: 0, scoreSum: 0 },
};

export function getAbGroup(sessionId: string): AbGroup {
  const hash = crypto.createHash('sha256').update(sessionId).digest('hex');
  const bucket = parseInt(hash.slice(0, 8), 16) % 100;
  return bucket < 50 ? 'A' : 'B';
}

export function recordAbOutcome(group: AbGroup, score: number): void {
  groupStats[group].count += 1;
  groupStats[group].scoreSum += score;
}

export function getAbStats(): {
  groupA: { count: number; meanScore: number };
  groupB: { count: number; meanScore: number };
} {
  const meanA = groupStats.A.count > 0 ? groupStats.A.scoreSum / groupStats.A.count : 0;
  const meanB = groupStats.B.count > 0 ? groupStats.B.scoreSum / groupStats.B.count : 0;
  return {
    groupA: { count: groupStats.A.count, meanScore: meanA },
    groupB: { count: groupStats.B.count, meanScore: meanB },
  };
}
