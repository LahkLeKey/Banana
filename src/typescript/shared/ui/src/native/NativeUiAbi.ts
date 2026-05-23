import type {EngineStatus} from '../components/GameEngineTypes';
import type {GameEngineUIState} from '../domains/GameEngineUIService';
import type {ViewportSize} from '../domains/ViewportDomain';

export type NativeUiHostKind = 'web'|'electron'|'react-native'|'native';

export type NativeUiSurface = 'game-engine';

export type NativeUiFrame = {
  host: NativeUiHostKind; surface: NativeUiSurface; engineStatus: EngineStatus;
  error: string | null;
  movementSource: GameEngineUIState['movementInput']['source'];
  movement: {moveX: number; moveZ: number;};
  viewport: ViewportSize;
  interactionMessage: string | null;
  timestamp: number;
};

export type NativeUiMarshalInput = {
  host: NativeUiHostKind; state: Pick<
                              GameEngineUIState,
                              |'engineStatus'|'error'|'movementInput'|
                              'viewportSize'|'interactionMessage'|'timestamp'>;
  surface?: NativeUiSurface;
};

/**
 * Marshal shared game-engine UI state into a host-neutral ABI frame.
 * UI shells can forward the frame directly to native viewports, Electron,
 * web, or mobile adapters without owning the state transformation rules.
 */
export function marshalNativeUiFrame(
    input: NativeUiMarshalInput,
    ): NativeUiFrame {
  return {
    host: input.host,
    surface: input.surface ?? 'game-engine',
    engineStatus: input.state.engineStatus,
    error: input.state.error,
    movementSource: input.state.movementInput.source,
    movement: {
      moveX: input.state.movementInput.moveX,
      moveZ: input.state.movementInput.moveZ,
    },
    viewport: input.state.viewportSize,
    interactionMessage: input.state.interactionMessage,
    timestamp: input.state.timestamp,
  };
}