// Legacy-compatible shared contracts used by React/API integration surfaces.

export interface ChatMessage {
  id: string;
  role: 'user'|'assistant'|'system';
  content: string;
  createdAt?: string;
}

export interface ChatSession {
  id: string;
  createdAt?: string;
  updatedAt?: string;
}

export interface EnsembleVerdict {
  label: string;
  confidence: number;
  model?: string;
}

export interface RipenessResult {
  verdict: string;
  confidence: number;
  notes?: string;
}

export type TrainingLane = 'candidate'|'stable'|'baseline';

export interface TrainingRunRequest {
  lane: TrainingLane;
  prompt?: string;
  dryRun?: boolean;
}

export interface TrainingRunResult {
  runId: string;
  lane: TrainingLane;
  status: 'queued'|'running'|'completed'|'failed';
  startedAt?: string;
  completedAt?: string;
}

export interface TrainingAuditEvent {
  id: string;
  runId: string;
  event: string;
  at: string;
  details?: Record<string, string|number|boolean|null>;
}
