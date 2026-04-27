#!/usr/bin/env node

import {readFileSync, writeFileSync} from 'node:fs';

import {utcNowIso} from './lib/parity-finding-model.mjs';

function parseJson(filePath) {
  return JSON.parse(readFileSync(filePath, 'utf8'));
}

function parseArgs(argv) {
  const args = {
    driftFile: '.specify/specs/047-api-parity-governance/artifacts/parity-drift-report.json',
    outFile: '.specify/specs/047-api-parity-governance/artifacts/parity-gate-result.json',
    strict: false,
  };

  for (let i = 0; i < argv.length; i += 1) {
    const token = argv[i];
    if (token === '--drift-file') {
      args.driftFile = argv[i + 1];
      i += 1;
    } else if (token === '--out-file') {
      args.outFile = argv[i + 1];
      i += 1;
    } else if (token === '--strict') {
      args.strict = true;
    }
  }

  return args;
}

export function evaluateParityGate(driftReport) {
  const unresolved = driftReport.unresolved_summary ?? driftReport.summary ?? {
    total_findings: 0,
    missing_route: 0,
    status_mismatch: 0,
    shape_mismatch: 0,
  };
  const unresolvedTotal = Number(unresolved.total_findings ?? 0);
  const passed = unresolvedTotal === 0;

  return {
    evaluated_at_utc: utcNowIso(),
    decision: passed ? 'PASS' : 'FAIL',
    unresolved_summary: {
      total_findings: unresolvedTotal,
      missing_route: Number(unresolved.missing_route ?? 0),
      status_mismatch: Number(unresolved.status_mismatch ?? 0),
      shape_mismatch: Number(unresolved.shape_mismatch ?? 0),
    },
    reasons: passed ? [] : ['Unresolved parity drift findings remain.'],
  };
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  const driftReport = parseJson(args.driftFile);
  const decision = evaluateParityGate(driftReport);
  writeFileSync(args.outFile, `${JSON.stringify(decision, null, 2)}\n`, 'utf8');

  console.log(`[banana] parity gate decision: ${decision.decision}`);
  if (args.strict && decision.decision === 'FAIL') {
    process.exit(1);
  }
}

if (process.argv[1] && process.argv[1].endsWith('evaluate-parity-gate.mjs')) {
  main();
}
