// @banana/ui — public surface (spec 009 contract).
// Deep imports are forbidden by lint; everything cross-platform-safe is
// re-exported here.

export type { BananaBadgeProps } from "./components/BananaBadge";
export { BananaBadge } from "./components/BananaBadge";
export { ChatMessageBubble } from "./components/ChatMessageBubble";
export type { ErrorTextProps } from "./components/ErrorText";
export { ErrorText } from "./components/ErrorText";
export type { EmbeddingSummary, EscalationPanelProps } from "./components/EscalationPanel";
export { EscalationPanel } from "./components/EscalationPanel";
export type { RetryButtonProps } from "./components/RetryButton";
export { RETRY_BUTTON_COPY, RetryButton } from "./components/RetryButton";
export { RipenessLabel } from "./components/RipenessLabel";
export { TrainingMetricCard } from "./components/TrainingMetricCard";
export type { Tokens } from "./tokens";
export { tokens } from "./tokens";

export function computeCosineDrift(a: number[], b: number[]): number {
  if (!a?.length || !b?.length || a.length !== b.length) return 0;
  const dot = a.reduce((s, v, i) => s + v * b[i], 0);
  const magA = Math.sqrt(a.reduce((s, v) => s + v * v, 0));
  const magB = Math.sqrt(b.reduce((s, v) => s + v * v, 0));
  if (magA === 0 || magB === 0) return 0;
  return 1 - dot / (magA * magB);
}

export type { ContextMenuProps } from "./components/ContextMenu";
export { ContextMenu } from "./components/ContextMenu";
export type {
  ControlsHintProps,
  EngineVersionBadgeProps,
  ErrorBadgeProps,
  InteractionMessageBadgeProps,
  ViewportErrorOverlayProps,
} from "./components/GameEngineOverlays";
export {
  ControlsHint,
  EngineVersionBadge,
  ErrorBadge,
  InteractionMessageBadge,
  ViewportErrorOverlay,
} from "./components/GameEngineOverlays";
// Game Engine UI — shared components for ARPG viewport
export type {
  ContextMenuAction,
  ContextMenuState,
  EngineStatus,
  RadialControlState,
  RadialDirection,
} from "./components/GameEngineTypes";
export {
  computeContextMenuPosition,
  computeMoveAxes,
  getDirectionFromTouch,
  isMovementKey,
  MOVEMENT_KEYS,
  RADIAL_CONFIG,
} from "./components/GameEngineTypes";
export type { MobileRadialControlProps } from "./components/MobileRadialControl";
export { MobileRadialControl } from "./components/MobileRadialControl";
export type { SplashOverlayProps } from "./components/SplashOverlay";
export { SplashOverlay } from "./components/SplashOverlay";
export * from "./domains/ConnectionManager";
export * from "./domains/GameEngineUIService";
// Domain-Driven Design: Game Engine UI Domains
export * from "./domains/InputDomain";
export * from "./domains/InteractionDomain";
export * from "./domains/NetcodeDomain";
export * from "./domains/PredictionDomain";
export * from "./domains/ReplicationDomain";
export * from "./domains/ViewportDomain";
export type {
  ChatMessage,
  ChatMessageStatus,
  ChatRole,
  ChatSession,
  EmbeddingDriftSummary,
  EnsembleLabel,
  EnsembleStatus,
  EnsembleVerdict,
  PromotionAuditEntry,
  Ripeness,
  RipenessResult,
  TrainingAuditEvent,
  TrainingLane,
  TrainingRunRequest,
  TrainingRunResult,
} from "./types";
