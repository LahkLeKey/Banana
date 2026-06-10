import {describe, expect, it} from 'bun:test';

import {resolveRouteHudPreset} from './routeModeConfig';

describe('resolveRouteHudPreset', () => {
  it('maps known aliases to expected route modes', () => {
    expect(resolveRouteHudPreset('/notebooks').mode).toBe('runtime');
    expect(resolveRouteHudPreset('/login').mode).toBe('auth');
    expect(resolveRouteHudPreset('/marketing').mode).toBe('marketing');
    expect(resolveRouteHudPreset('/account').mode).toBe('portal');
    expect(resolveRouteHudPreset('/download').mode).toBe('portal');
    expect(resolveRouteHudPreset('/session-room').mode).toBe('session');
    expect(resolveRouteHudPreset('/banana-engine').mode).toBe('engine');
    expect(resolveRouteHudPreset('/game-main-menu').mode).toBe('menu');
    expect(resolveRouteHudPreset('/select-character').mode).toBe('character');
  });

  it('falls back to runtime mode for unknown paths', () => {
    const preset = resolveRouteHudPreset('/unknown-surface');

    expect(preset.mode).toBe('runtime');
    expect(preset.explorer).toBe(true);
    expect(preset.menu).toBe(true);
    expect(preset.status).toBe(true);
    expect(preset.operations).toBe(true);
  });
});
