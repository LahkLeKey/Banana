import type {DancingBananaParticle} from './types';

export function buildDancingBananaParticles(
    startId: number, count = 28): DancingBananaParticle[] {
  const glyphs = ['🍌', '🕺', '✨', '🎶'];
  return Array.from(
      {length: count},
      (_, index) => ({
        id: startId + index,
        leftPct: 38 + Math.random() * 24,
        topPct: 42 + Math.random() * 20,
        durationMs: 1200 + Math.floor(Math.random() * 2200),
        delayMs: Math.floor(Math.random() * 380),
        sizeRem: 1 + Math.random() * 1.2,
        glyph: glyphs[Math.floor(Math.random() * glyphs.length)] ?? '🍌',
      }));
}