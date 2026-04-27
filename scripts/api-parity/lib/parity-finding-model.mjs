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
