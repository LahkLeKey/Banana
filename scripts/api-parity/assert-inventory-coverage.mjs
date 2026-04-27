#!/usr/bin/env node

import {readFileSync} from 'node:fs';

import {loadAspNetRouteSurface} from './lib/load-aspnet-route-surface.mjs';
import {loadFastifyRouteSurface} from './lib/load-fastify-route-surface.mjs';

function parseArgs(argv) {
  const args = {
    repoRoot: process.cwd(),
    inventoryFile: '.specify/specs/047-api-parity-governance/artifacts/overlapping-routes.inventory.json',
  };

  for (let i = 0; i < argv.length; i += 1) {
    const token = argv[i];
    if (token === '--repo-root') {
      args.repoRoot = argv[i + 1];
      i += 1;
    } else if (token === '--inventory-file') {
      args.inventoryFile = argv[i + 1];
      i += 1;
    }
  }

  return args;
}

function readInventory(filePath) {
  return JSON.parse(readFileSync(filePath, 'utf8'));
}

function keySetForOverlap(repoRoot) {
  const aspnet = loadAspNetRouteSurface(repoRoot);
  const fastify = loadFastifyRouteSurface(repoRoot);
  const fastifyKeys = new Set(fastify.map((route) => route.routeKey));
  return new Set(aspnet.map((route) => route.routeKey).filter((key) => fastifyKeys.has(key)));
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  const inventoryPath = args.inventoryFile;
  const inventory = readInventory(inventoryPath);
  const routes = Array.isArray(inventory.routes) ? inventory.routes : [];

  const errors = [];
  const seen = new Set();

  for (const [index, route] of routes.entries()) {
    const prefix = `routes[${index}]`;
    if (!route.route_key || String(route.route_key).trim().length === 0) {
      errors.push(`${prefix}.route_key is required`);
      continue;
    }
    if (seen.has(route.route_key)) {
      errors.push(`${prefix}.route_key '${route.route_key}' is duplicated`);
    }
    seen.add(route.route_key);

    if (!route.method || !route.path) {
      errors.push(`${prefix} must include method and path`);
    }
    if (!Array.isArray(route.expected_statuses) || route.expected_statuses.length === 0) {
      errors.push(`${prefix}.expected_statuses must be a non-empty array`);
    }
    if (!Array.isArray(route.required_response_fields)) {
      errors.push(`${prefix}.required_response_fields must be an array`);
    }
  }

  const overlaps = keySetForOverlap(args.repoRoot);
  for (const overlap of overlaps) {
    if (!seen.has(overlap)) {
      errors.push(`missing overlap inventory record for '${overlap}'`);
    }
  }

  if (errors.length > 0) {
    for (const err of errors) {
      console.error(`[banana] coverage error: ${err}`);
    }
    process.exit(1);
  }

  console.log(`[banana] overlap inventory coverage passed (${routes.length} record(s))`);
}

main();
