export type Ripeness = "ripe"|"unripe"|"overripe";

export type ChatRole = "user"|"assistant"|"system";
export type ChatMessageStatus = "accepted"|"complete";

export type ChatMessage = {
    id: string; session_id : string; role : ChatRole; content : string; created_at : string;
    status : ChatMessageStatus;
};

export type ChatSession = {
    id: string; platform : string; created_at : string; updated_at : string; message_count : number;
};

/**
 * Slice 015 -- mirrors the TypeScript API ensemble verdict JSON contract.
 * Field order locked: label, score, did_escalate, calibration_magnitude,
 * status. Any drift here vs the API contract MUST fail the snapshot test in
 * `react/src/lib/api.test.ts`.
 */
export type EnsembleLabel = "banana"|"not_banana"|"unknown";
export type EnsembleStatus = "ok"|"degraded";
export type EnsembleVerdict = {
    label: EnsembleLabel; score : number; did_escalate : boolean; calibration_magnitude : number;
    status : EnsembleStatus;
};

export type RipenessResult = {
    label: Ripeness; confidence : number;
};
