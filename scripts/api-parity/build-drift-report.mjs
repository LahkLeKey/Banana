#!/usr/bin/env node

import {mkdirSync, writeFileSync} from 'node:fs';
import path from 'node:path';

import {buildStatusMismatchFindings} from './compare-status-parity.mjs';
import {buildShapeMismatchFindings} from './compare-shape-parity.mjs';
import {buildMissingRouteFindings} from './detect-missing-route-gaps.mjs';
import {mergeFindings, summarizeFindings, utcNowIso} from './lib/parity-finding-model.mjs';

function parseArgs(argv) {
  const args = {
    repoRoot: process.cwd(),
    inventoryFile:
        '.specify/specs/047-api-parity-governance/artifacts/overlapping-routes.inventory.json',
    outFile: '.specify/specs/047-api-parity-governance/artifacts/parity-drift-report.json',
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
    } else if (token === '--out-file') {
      args.outFile = argv[i + 1];
      i += 1;
    } else if (token === '--strict') {
      args.strict = true;
    }
  }

  return args;
}

export function buildDriftReport({repoRoot, inventoryFile, strict}) {
  const findings = mergeFindings(
      buildMissingRouteFindings(repoRoot),
      buildStatusMismatchFindings(inventoryFile),
      buildShapeMismatchFindings(inventoryFile));

  return {
    generated_at_utc: utcNowIso(),
    strict_mode: Boolean(strict),
    summary: summarizeFindings(findings),
    findings,
  };
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  const payload = buildDriftReport(args);
  const outFile = path.isAbsolute(args.outFile) ? args.outFile : path.join(args.repoRoot, args.outFile);

  mkdirSync(path.dirname(outFile), {recursive: true});
  writeFileSync(outFile, `${JSON.stringify(payload, null, 2)}\n`, 'utf8');

  console.log(`[banana] drift report generated (${payload.summary.total_findings} finding(s))`);
  console.log(`[banana] wrote ${path.relative(args.repoRoot, outFile).replace(/\\/g, '/')}`);
}

if (process.argv[1] && process.argv[1].endsWith('build-drift-report.mjs')) {
  main();
}
