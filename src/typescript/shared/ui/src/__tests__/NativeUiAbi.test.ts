// @ts-nocheck -- bun:test types are not part of app tsconfig.

import {describe, expect, test} from 'bun:test';

import {marshalNativeUiFrame} from '../native/NativeUiAbi';

describe('NativeUiAbi — host-neutral marshaling', () => {
  test('maps shared game-engine state into a native UI frame', () => {
    const frame = marshalNativeUiFrame({
      host: 'electron',
      state: {
        engineStatus: 'running',
        error: null,
        movementInput: {
          source: 'keyboard',
          moveX: 1,
          moveZ: 0,
        },
        viewportSize: {
          cssWidth: 1280,
          cssHeight: 720,
          pixelWidth: 2560,
          pixelHeight: 1440,
          devicePixelRatio: 2,
        },
        interactionMessage: 'ready',
        timestamp: 123456789,
      },
    });

    expect(frame.host).toBe('electron');
    expect(frame.surface).toBe('game-engine');
    expect(frame.engineStatus).toBe('running');
    expect(frame.movementSource).toBe('keyboard');
    expect(frame.movement.moveX).toBe(1);
    expect(frame.viewport.pixelWidth).toBe(2560);
    expect(frame.interactionMessage).toBe('ready');
    expect(frame.timestamp).toBe(123456789);
  });

  test('allows the host surface to be overridden explicitly', () => {
    const frame = marshalNativeUiFrame({
      host: 'native',
      surface: 'game-engine',
      state: {
        engineStatus: 'idle',
        error: null,
        movementInput: {
          source: 'none',
          moveX: 0,
          moveZ: 0,
        },
        viewportSize: {
          cssWidth: 1,
          cssHeight: 1,
          pixelWidth: 1,
          pixelHeight: 1,
          devicePixelRatio: 1,
        },
        interactionMessage: null,
        timestamp: 1,
      },
    });

    expect(frame.surface).toBe('game-engine');
    expect(frame.host).toBe('native');
  });
});