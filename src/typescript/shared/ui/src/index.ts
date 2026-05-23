// @banana/ui — public surface (spec 009 contract).
// Deep imports are forbidden by lint; everything cross-platform-safe is
// re-exported here.

export type {ErrorTextProps} from './components/ErrorText';
export {ErrorText} from './components/ErrorText';
// Game Engine UI — shared components for ARPG viewport
export type {EngineStatus, RadialDirection,} from './components/GameEngineTypes';
export {computeMoveAxes, getDirectionFromTouch, isMovementKey, MOVEMENT_KEYS, RADIAL_CONFIG,} from './components/GameEngineTypes';
export type {RetryButtonProps} from './components/RetryButton';
export {RETRY_BUTTON_COPY, RetryButton} from './components/RetryButton';
export type {NativeUiFrame, NativeUiHostKind, NativeUiMarshalInput, NativeUiSurface,} from './native/NativeUiAbi';
export {marshalNativeUiFrame} from './native/NativeUiAbi';
export type {Tokens} from './tokens';
export {tokens} from './tokens';
export type {ChatMessage, ChatSession, EnsembleVerdict, RipenessResult, TrainingAuditEvent, TrainingLane, TrainingRunRequest, TrainingRunResult,} from './types';

export * from './domains/ConnectionManager';
export * from './domains/GameEngineUIService';
// Domain-Driven Design: Game Engine UI Domains
export * from './domains/InputDomain';
export * from './domains/NetcodeDomain';
export * from './domains/PredictionDomain';
export * from './domains/ReplicationDomain';
export * from './domains/ViewportDomain';
export {LoginPage} from './auth/LoginPage';
export {BANANA_AUTH_STEAM_ID_STORAGE_KEY, BANANA_AUTH_TOKEN_STORAGE_KEY, buildSteamAuthStartUrl, clearStoredAuthSession, hasStoredAuthSession, parseAuthCallbackHash, readStoredAuthSession, resolveLoginReturnToUrl, storeAuthSession,} from './auth/session';
export {GameEnginePage} from './game-engine/GameEnginePage';
