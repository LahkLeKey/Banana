/**
 * Shared types for game engine UI components
 * Used by GameEnginePage and frontend consumers.
 */

export type EngineStatus = 'idle'|'loading'|'running'|'error'|'unavailable';

export type RadialDirection = 'up'|'down'|'left'|'right'|null;

/** Radial control dimensions and thresholds */
export const RADIAL_CONFIG = {
  RADIUS: 60,
  INNER_RADIUS: 20,
  DIRECTION_THRESHOLD: 0.35,
  HOLD_REVEAL_DELAY_MS: 150,
};

export const MOVEMENT_KEYS = new Set([
  'w',
  'a',
  's',
  'd',
  'arrowup',
  'arrowleft',
  'arrowdown',
  'arrowright',
]);

export function isMovementKey(key: string): boolean {
  return MOVEMENT_KEYS.has(key.toLowerCase());
}

export function computeMoveAxes(keys: ReadonlySet<string>):
    {moveX: number; moveZ: number} {
  const moveX = (keys.has('d') || keys.has('arrowright') ? 1 : 0) +
      (keys.has('a') || keys.has('arrowleft') ? -1 : 0);
  const moveZ = (keys.has('w') || keys.has('arrowup') ? 1 : 0) +
      (keys.has('s') || keys.has('arrowdown') ? -1 : 0);
  return {moveX, moveZ};
}

export function getDirectionFromTouch(
    touchX: number, touchY: number, radialCenterX: number,
    radialCenterY: number): RadialDirection {
  const deltaX = touchX - radialCenterX;
  const deltaY = touchY - radialCenterY;
  const distance = Math.sqrt(deltaX * deltaX + deltaY * deltaY);

  if (distance < RADIAL_CONFIG.INNER_RADIUS) {
    return null;
  }

  if (distance / RADIAL_CONFIG.RADIUS < RADIAL_CONFIG.DIRECTION_THRESHOLD) {
    return null;
  }

  const angle = Math.atan2(deltaY, deltaX);
  const normalizedAngle = ((angle * 180) / Math.PI + 360) % 360;

  if (normalizedAngle < 45 || normalizedAngle >= 315) return 'right';
  if (normalizedAngle < 135) return 'down';
  if (normalizedAngle < 225) return 'left';
  return 'up';
}
