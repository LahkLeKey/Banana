#!/usr/bin/env node

import {readFileSync, writeFileSync} from 'node:fs';

import {sortFindings, utcNowIso} from './lib/parity-finding-model.mjs';

function parseJson(filePath) {
  return JSON.parse(readFileSync(filePath, 'utf8'));
}

function toMillis(value) {
  const parsed = new Date(String(value));
  const ms = parsed.getTime();
  return Number.isNaN(ms) ? null : ms;
}

function isExceptionActive(exception, nowMs) {
  const approved = toMillis(exception.approved_at);
  const expires = toMillis(exception.expires_at);
  if (approved === null || expires === null) return false;
  if (approved > nowMs) return false;
  return expires > nowMs;
}

export function applyParityExceptionsToFindings(driftReport, exceptionLedger, now = new Date()) {
  const exceptions = Array.isArray(exceptionLedger.exceptions) ? exceptionLedger.exceptions : [];
  const nowMs = now.getTime();
  const findings = Array.isArray(driftReport.findings) ? driftReport.findings : [];

  const activeMap = new Map();
  for (const exception of exceptions) {
    if (!isExceptionActive(exception, nowMs)) continue;
    const key = `${exception.route_key}::${exception.mismatch_type}`;
    if (!activeMap.has(key)) {
      activeMap.set(key, exception);
    }
  }

  const withResolution = findings.map((finding) => {
    const key = `${finding.route_key}::${finding.mismatch_type}`;
    const exception = activeMap.get(key) ?? null;
    return {
      ...finding,
      exception_applied: Boolean(exception),
      exception,
      resolved: Boolean(exception),
    };
  });

  const unresolved = withResolution.filter((finding) => !finding.resolved);
  return {
    ...driftReport,
    generated_at_utc: utcNowIso(),
    findings: sortFindings(withResolution),
    unresolved_findings: sortFindings(unresolved),
    unresolved_summary: {
      total_findings: unresolved.length,
      missing_route: unresolved.filter((finding) => finding.mismatch_type === 'missing_route').length,
      status_mismatch:
          unresolved.filter((finding) => finding.mismatch_type === 'status_mismatch').length,
      shape_mismatch:
          unresolved.filter((finding) => finding.mismatch_type === 'shape_mismatch').length,
    },
  };
}

function main() {
  const driftFile = process.argv[2] ?? '.specify/specs/047-api-parity-governance/artifacts/parity-drift-report.json';
  const exceptionsFile = process.argv[3] ?? '.specify/specs/047-api-parity-governance/artifacts/parity-exceptions.json';
  const outFile = process.argv[4] ?? driftFile;

  const driftReport = parseJson(driftFile);
  const exceptionLedger = parseJson(exceptionsFile);
  const applied = applyParityExceptionsToFindings(driftReport, exceptionLedger);
  writeFileSync(outFile, `${JSON.stringify(applied, null, 2)}\n`, 'utf8');

  console.log(`[banana] applied parity exceptions, unresolved: ${applied.unresolved_summary.total_findings}`);
}

if (process.argv[1] && process.argv[1].endsWith('apply-parity-exceptions.mjs')) {
  main();
}
