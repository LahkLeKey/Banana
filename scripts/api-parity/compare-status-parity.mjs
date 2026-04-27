#!/usr/bin/env node

import {readFileSync} from 'node:fs';

import {createFinding, sortFindings} from './lib/parity-finding-model.mjs';

function parseJson(filePath) {
  return JSON.parse(readFileSync(filePath, 'utf8'));
}

function normalizeStatusList(values) {
  if (!Array.isArray(values)) return null;
  const normalized = values
      .map((value) => Number.parseInt(String(value), 10))
      .filter((value) => Number.isFinite(value));
  return Array.from(new Set(normalized)).sort((a, b) => a - b);
}

function arraysEqual(left, right) {
  if (!Array.isArray(left) || !Array.isArray(right)) return false;
  if (left.length !== right.length) return false;
  for (let i = 0; i < left.length; i += 1) {
    if (left[i] !== right[i]) return false;
  }
  return true;
}

export function buildStatusMismatchFindings(inventoryFile) {
  const payload = parseJson(inventoryFile);
  const routes = Array.isArray(payload.routes) ? payload.routes : [];
  const findings = [];

  for (const route of routes) {
    const expected = normalizeStatusList(route.expected_statuses) ?? [];
    const aspnetObserved = normalizeStatusList(route.aspnet?.observed_statuses);
    const fastifyObserved = normalizeStatusList(route.fastify?.observed_statuses);

    const mismatch =
        (aspnetObserved && !arraysEqual(aspnetObserved, expected)) ||
        (fastifyObserved && !arraysEqual(fastifyObserved, expected)) ||
        (aspnetObserved && fastifyObserved && !arraysEqual(aspnetObserved, fastifyObserved));

    if (!mismatch) {
      continue;
    }

    findings.push(createFinding({
      routeKey: route.route_key,
      mismatchType: 'status_mismatch',
      aspnet: {
        declared_in: route.aspnet?.declared_in ?? null,
        observed_statuses: aspnetObserved,
      },
      fastify: {
        declared_in: route.fastify?.declared_in ?? null,
        observed_statuses: fastifyObserved,
      },
      remediation:
          'Align status semantics with expected_statuses for both API surfaces or add a time-bounded exception.',
    }));
  }

  return sortFindings(findings);
}

function main() {
  const inventoryFile = process.argv[2] ??
      '.specify/specs/047-api-parity-governance/artifacts/overlapping-routes.inventory.json';
  const findings = buildStatusMismatchFindings(inventoryFile);
  console.log(JSON.stringify({findings}, null, 2));
}

if (process.argv[1] && process.argv[1].endsWith('compare-status-parity.mjs')) {
  main();
}
