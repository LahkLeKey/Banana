#!/usr/bin/env node

import {readFileSync, writeFileSync} from 'node:fs';

import {applyParityExceptionsToFindings} from './apply-parity-exceptions.mjs';
import {buildDriftReport} from './build-drift-report.mjs';
import {evaluateParityGate} from './evaluate-parity-gate.mjs';

function parseArgs(argv) {
  const args = {
    repoRoot: process.cwd(),
    inventoryFile:
        '.specify/specs/047-api-parity-governance/artifacts/overlapping-routes.inventory.json',
    exceptionsFile:
        '.specify/specs/047-api-parity-governance/artifacts/parity-exceptions.json',
    driftFile: '.specify/specs/047-api-parity-governance/artifacts/parity-drift-report.json',
    gateFile: '.specify/specs/047-api-parity-governance/artifacts/parity-gate-result.json',
    strict: false,
  };

  for (let i = 0; i < argv.length; i += 1) {
    const token = argv[i];
    if (token === '--repo-root') {
      args.repoRoot = argv[i + 1];
      i += 1;
    } else if (token === '--inventory-file') {
      args.inventoryFile = argv[i + 1];
      i += 1;
    } else if (token === '--exceptions-file') {
      args.exceptionsFile = argv[i + 1];
      i += 1;
    } else if (token === '--drift-file') {
      args.driftFile = argv[i + 1];
      i += 1;
    } else if (token === '--gate-file') {
      args.gateFile = argv[i + 1];
      i += 1;
    } else if (token === '--strict') {
      args.strict = true;
    }
  }

  return args;
}

function parseJson(filePath) {
  return JSON.parse(readFileSync(filePath, 'utf8'));
}

export function runParityGate(args) {
  const driftReport = buildDriftReport({
    repoRoot: args.repoRoot,
    inventoryFile: args.inventoryFile,
    strict: args.strict,
  });

  const exceptionLedger = parseJson(args.exceptionsFile);
  const withExceptions = applyParityExceptionsToFindings(driftReport, exceptionLedger);
  writeFileSync(args.driftFile, `${JSON.stringify(withExceptions, null, 2)}\n`, 'utf8');

  const gate = evaluateParityGate(withExceptions);
  writeFileSync(args.gateFile, `${JSON.stringify(gate, null, 2)}\n`, 'utf8');

  return {gate, withExceptions};
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  const {gate} = runParityGate(args);

  console.log(`[banana] parity gate decision: ${gate.decision}`);
  if (args.strict && gate.decision === 'FAIL') {
    process.exit(1);
  }
}

if (process.argv[1] && process.argv[1].endsWith('run-parity-gate.mjs')) {
  main();
}
