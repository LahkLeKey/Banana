#!/usr/bin/env node

export function utcNowIso() {
  return new Date().toISOString();
}

export function createFinding({
  routeKey,
  mismatchType,
  aspnet,
  fastify,
  remediation,
}) {
  return {
    route_key: routeKey,
    mismatch_type: mismatchType,
    aspnet,
    fastify,
    remediation,
  };
}

export function summarizeFindings(findings) {
  const summary = {
    total_findings: findings.length,
    missing_route: 0,
    status_mismatch: 0,
    shape_mismatch: 0,
  };

  for (const finding of findings) {
    if (finding.mismatch_type === 'missing_route') summary.missing_route += 1;
    if (finding.mismatch_type === 'status_mismatch') summary.status_mismatch += 1;
    if (finding.mismatch_type === 'shape_mismatch') summary.shape_mismatch += 1;
  }

  return summary;
}

export function sortFindings(findings) {
  return [...findings].sort((a, b) => {
    const keyDiff = String(a.route_key).localeCompare(String(b.route_key));
    if (keyDiff !== 0) return keyDiff;
    return String(a.mismatch_type).localeCompare(String(b.mismatch_type));
  });
}

export function mergeFindings(...findingGroups) {
  const merged = [];
  for (const group of findingGroups) {
    if (!Array.isArray(group)) continue;
    for (const finding of group) {
      merged.push(finding);
    }
  }
  return sortFindings(merged);
}
