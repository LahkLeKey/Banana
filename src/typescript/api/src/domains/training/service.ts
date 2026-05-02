import { spawn } from "node:child_process";
import { randomUUID } from "node:crypto";

export type TrainingLane = "left-brain" | "right-brain";

export type StartTrainingInput = {
  lane: TrainingLane;
  corpus_path?: string;
  training_profile: "ci" | "local" | "overnight" | "auto";
  session_mode: "single" | "incremental";
  max_sessions: number;
  operator_id: string;
  notes?: string;
};

export type TrainingRunRecord = {
  run_id: string;
  lane: TrainingLane;
  status: "running" | "passed" | "failed";
  started_at: string;
  finished_at: string;
  output_dir: string;
  metrics: Record<string, unknown>;
  run_fingerprint?: string;
  persisted_session?: Record<string, unknown>;
  history_drift?: Record<string, unknown>;
  recovery_guidance?: string[];
  threshold_passed: boolean;
  operator_id: string;
  notes?: string;
  error?: string;
};

export type TrainingAuditEvent = {
  id: string;
  run_id: string;
  lane: TrainingLane;
  operator_id: string;
  action: "run_submitted" | "run_completed" | "promote_candidate" | "promote_stable";
  created_at: string;
  detail?: string;
};

const laneDefaults: Record<
  TrainingLane,
  {
    script: string;
    corpus: string;
    outputPrefix: string;
  }
> = {
  "left-brain": {
    script: "scripts/train-ripeness-model.py",
    corpus: "data/ripeness/corpus.json",
    outputPrefix: "artifacts/training/ripeness/workbench",
  },
  "right-brain": {
    script: "scripts/train-not-banana-model.py",
    corpus: "data/not-banana/corpus.json",
    outputPrefix: "artifacts/training/not-banana/workbench",
  },
};

const runs = new Map<string, TrainingRunRecord>();
const auditEvents: TrainingAuditEvent[] = [];

function buildRecoveryGuidance(
  lane: TrainingLane,
  errorText?: string,
  thresholdPassed = true
): string[] {
  const guidance: string[] = [];
  if (!thresholdPassed) {
    guidance.push(
      "Threshold gate failed: expand corpus coverage and rerun in incremental mode for comparison."
    );
    guidance.push(
      "Review history_drift + metrics output and reduce ambiguous sample noise before stable promotion."
    );
  }

  const lowered = (errorText || "").toLowerCase();
  if (lowered.includes("file not found") || lowered.includes("no such file")) {
    guidance.push("Verify corpus and output paths are valid workspace-relative locations.");
  }
  if (lowered.includes("schema")) {
    guidance.push("Run schema validators locally and repair required fields before retrying.");
  }
  if (lowered.includes("threshold")) {
    guidance.push(
      "Tune profile/session settings or improve corpus quality to raise threshold pass rates."
    );
  }

  if (guidance.length === 0) {
    guidance.push(
      `Review ${lane} trainer stderr and retry with session_mode=single for a faster repro.`
    );
  }
  return guidance;
}

function nowIso(): string {
  return new Date().toISOString();
}

function parseLastJsonObject(stdout: string): Record<string, unknown> {
  const lines = stdout.trim().split(/\r?\n/).filter(Boolean).reverse();
  for (const line of lines) {
    try {
      const parsed = JSON.parse(line) as Record<string, unknown>;
      if (parsed && typeof parsed === "object") return parsed;
    } catch {
      // keep scanning
    }
  }
  throw new Error("training output did not contain a valid JSON result");
}

function runCommand(
  command: string,
  args: string[]
): Promise<{ stdout: string; stderr: string; code: number }> {
  return new Promise((resolve) => {
    const child = spawn(command, args, { stdio: ["ignore", "pipe", "pipe"] });
    let stdout = "";
    let stderr = "";

    child.stdout.on("data", (chunk) => {
      stdout += chunk.toString();
    });
    child.stderr.on("data", (chunk) => {
      stderr += chunk.toString();
    });

    child.on("close", (code) => {
      resolve({ stdout, stderr, code: code ?? 1 });
    });
  });
}

function pushAudit(event: Omit<TrainingAuditEvent, "id" | "created_at">) {
  auditEvents.push({
    id: `audit_${randomUUID()}`,
    created_at: nowIso(),
    ...event,
  });
  if (auditEvents.length > 500) auditEvents.shift();
}

export async function startTrainingRun(input: StartTrainingInput): Promise<TrainingRunRecord> {
  const laneConfig = laneDefaults[input.lane];
  const started_at = nowIso();
  const run_id = `run_${randomUUID()}`;
  const output_dir = `${laneConfig.outputPrefix}-${Date.now()}`;

  pushAudit({
    run_id,
    lane: input.lane,
    operator_id: input.operator_id,
    action: "run_submitted",
    detail: input.notes,
  });

  const pythonCommand = process.env.BANANA_PYTHON_BIN || "python";
  const args = [
    laneConfig.script,
    "--corpus",
    input.corpus_path || laneConfig.corpus,
    "--output",
    output_dir,
    "--training-profile",
    input.training_profile,
    "--session-mode",
    input.session_mode,
    "--max-sessions",
    String(input.max_sessions),
  ];

  const executed = await runCommand(pythonCommand, args);
  const finished_at = nowIso();

  if (executed.code !== 0 && executed.code !== 2) {
    const failed: TrainingRunRecord = {
      run_id,
      lane: input.lane,
      status: "failed",
      started_at,
      finished_at,
      output_dir,
      metrics: {},
      threshold_passed: false,
      operator_id: input.operator_id,
      notes: input.notes,
      error: executed.stderr.trim() || `trainer exited with code ${executed.code}`,
      recovery_guidance: buildRecoveryGuidance(
        input.lane,
        executed.stderr.trim() || `trainer exited with code ${executed.code}`,
        false
      ),
    };
    runs.set(run_id, failed);
    pushAudit({
      run_id,
      lane: input.lane,
      operator_id: input.operator_id,
      action: "run_completed",
      detail: failed.error,
    });
    return failed;
  }

  const payload = parseLastJsonObject(executed.stdout);
  const metrics =
    payload.metrics && typeof payload.metrics === "object"
      ? (payload.metrics as Record<string, unknown>)
      : {};
  const threshold_passed = metrics.meets_thresholds === true;

  const record: TrainingRunRecord = {
    run_id,
    lane: input.lane,
    status: threshold_passed ? "passed" : "failed",
    started_at,
    finished_at,
    output_dir,
    metrics,
    run_fingerprint:
      typeof payload.run_fingerprint === "string" ? payload.run_fingerprint : undefined,
    persisted_session:
      payload.persisted_session && typeof payload.persisted_session === "object"
        ? (payload.persisted_session as Record<string, unknown>)
        : undefined,
    history_drift:
      payload.history_drift && typeof payload.history_drift === "object"
        ? (payload.history_drift as Record<string, unknown>)
        : undefined,
    recovery_guidance: threshold_passed
      ? undefined
      : buildRecoveryGuidance(input.lane, executed.stderr, threshold_passed),
    threshold_passed,
    operator_id: input.operator_id,
    notes: input.notes,
  };

  runs.set(run_id, record);
  pushAudit({
    run_id,
    lane: input.lane,
    operator_id: input.operator_id,
    action: "run_completed",
    detail: record.status,
  });
  return record;
}

export function getRun(runId: string): TrainingRunRecord | undefined {
  return runs.get(runId);
}

export function listRunHistory(lane?: TrainingLane): TrainingRunRecord[] {
  const all = Array.from(runs.values());
  const filtered = lane ? all.filter((row) => row.lane === lane) : all;
  return filtered.sort((a, b) => b.started_at.localeCompare(a.started_at));
}

export function listAuditTrail(runId?: string): TrainingAuditEvent[] {
  const filtered = runId ? auditEvents.filter((item) => item.run_id === runId) : auditEvents;
  return [...filtered].sort((a, b) => b.created_at.localeCompare(a.created_at));
}

export function recordPromotionAction(
  run: TrainingRunRecord,
  operatorId: string,
  target: "candidate" | "stable",
  reason?: string
): TrainingAuditEvent {
  const action = target === "stable" ? "promote_stable" : "promote_candidate";
  const event: TrainingAuditEvent = {
    id: `audit_${randomUUID()}`,
    run_id: run.run_id,
    lane: run.lane,
    operator_id: operatorId,
    action,
    created_at: nowIso(),
    detail: reason,
  };
  auditEvents.push(event);
  if (auditEvents.length > 500) auditEvents.shift();
  return event;
}
