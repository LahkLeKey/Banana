export type TrainingLane = "left-brain"|"right-brain";
export type TrainingProfile = "ci"|"local"|"overnight"|"auto";
export type TrainingSessionMode = "single"|"incremental";
export type TrainingRunStatus = "running"|"passed"|"failed";

export type TrainingRunRequest = {
    lane: TrainingLane;
    corpus_path?: string; training_profile : TrainingProfile; session_mode : TrainingSessionMode;
    max_sessions : number;
    operator_id : string;
    notes?: string;
};

export type TrainingRunResult = {
    run_id: string; lane : TrainingLane; status : TrainingRunStatus; started_at : string;
    finished_at : string;
    output_dir : string;
    metrics?:
        {accuracy?: number; f1?: number; centroid_embedding?: number[]; [key: string] : unknown;};
    run_fingerprint?: string;
    persisted_session?: Record<string, unknown>;
    history_drift?: Record<string, unknown>;
    recovery_guidance?: string[]; threshold_passed : boolean; operator_id : string;
    notes?: string;
    error?: string;
};

export type PromotionAuditEntry = {
    run_id: string; promoted_at : string; lane : TrainingLane;
};

export type EmbeddingDriftSummary = {
    lane: TrainingLane; drift : number | null;
};

export type TrainingAuditEvent = {
    id: string; run_id : string; lane : TrainingLane; operator_id : string;
    action : "run_submitted" | "run_completed" | "promote_candidate" | "promote_stable";
    created_at : string;
    detail?: string;
};
