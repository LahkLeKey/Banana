#!/usr/bin/env node

import {readFileSync} from 'node:fs';

import {createFinding, sortFindings} from './lib/parity-finding-model.mjs';

function parseJson(filePath) {
  return JSON.parse(readFileSync(filePath, 'utf8'));
}

function normalizeFieldList(values) {
  if (!Array.isArray(values)) return null;
  return Array.from(new Set(values.map((value) => String(value).trim()).filter(Boolean)))
      .sort((a, b) => a.localeCompare(b));
}

function arraysEqual(left, right) {
  if (!Array.isArray(left) || !Array.isArray(right)) return false;
  if (left.length !== right.length) return false;
  for (let i = 0; i < left.length; i += 1) {
    if (left[i] !== right[i]) return false;
  }
  return true;
}

export function buildShapeMismatchFindings(inventoryFile) {
  const payload = parseJson(inventoryFile);
  const routes = Array.isArray(payload.routes) ? payload.routes : [];
  const findings = [];

  for (const route of routes) {
    const expected = normalizeFieldList(route.required_response_fields) ?? [];
    const aspnetObserved = normalizeFieldList(route.aspnet?.observed_required_fields);
    const fastifyObserved = normalizeFieldList(route.fastify?.observed_required_fields);

    const mismatch =
        (aspnetObserved && !arraysEqual(aspnetObserved, expected)) ||
        (fastifyObserved && !arraysEqual(fastifyObserved, expected)) ||
        (aspnetObserved && fastifyObserved && !arraysEqual(aspnetObserved, fastifyObserved));

    if (!mismatch) {
      continue;
    }

    findings.push(createFinding({
      routeKey: route.route_key,
      mismatchType: 'shape_mismatch',
      aspnet: {
        declared_in: route.aspnet?.declared_in ?? null,
        observed_required_fields: aspnetObserved,
      },
      fastify: {
        declared_in: route.fastify?.declared_in ?? null,
        observed_required_fields: fastifyObserved,
      },
      remediation:
          'Align required response fields across API surfaces or add a time-bounded exception.',
    }));
  }

  return sortFindings(findings);
}

function main() {
  const inventoryFile = process.argv[2] ??
      '.specify/specs/047-api-parity-governance/artifacts/overlapping-routes.inventory.json';
  const findings = buildShapeMismatchFindings(inventoryFile);
  console.log(JSON.stringify({findings}, null, 2));
}

if (process.argv[1] && process.argv[1].endsWith('compare-shape-parity.mjs')) {
  main();
}
