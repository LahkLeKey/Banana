// PII classification and redaction for Banana API.
// Fields annotated with @pii in schema are passed through this module before logging/export.
export type PiiLevel = "none" | "low" | "medium" | "high";

export interface PiiAnnotation {
  field: string;
  level: PiiLevel;
}

// Canonical PII field registry (derived from Prisma schema annotations).
export const PII_FIELDS: PiiAnnotation[] = [
  { field: "audit_event.actor", level: "medium" },
  { field: "audit_event.ip_address", level: "high" },
  { field: "label_request.text", level: "low" },
  { field: "chat_message.content", level: "medium" },
];

export function redactForLevel(value: string, level: PiiLevel): string {
  if (level === "high") return "[REDACTED]";
  if (level === "medium") return `${value.substring(0, 4)}****`;
  return value; // low/none - pass through
}

export function redactFields<T extends Record<string, unknown>>(
  record: T,
  annotations: PiiAnnotation[]
): T {
  const redacted = { ...record };
  for (const ann of annotations) {
    const key = ann.field.split(".").pop()!;
    if (key in redacted && typeof redacted[key] === "string") {
      redacted[key] = redactForLevel(redacted[key] as string, ann.level) as T[keyof T];
    }
  }
  return redacted;
}
