#!/usr/bin/env node

import {mkdirSync, writeFileSync} from 'node:fs';
import path from 'node:path';

import {loadAspNetRouteSurface} from './lib/load-aspnet-route-surface.mjs';
import {loadFastifyRouteSurface} from './lib/load-fastify-route-surface.mjs';

function parseArgs(argv) {
  const args = {
    repoRoot: process.cwd(),
    outFile: '.specify/specs/047-api-parity-governance/artifacts/overlapping-routes.inventory.json',
  };

  for (let i = 0; i < argv.length; i += 1) {
    const token = argv[i];
    if (token === '--repo-root') {
      args.repoRoot = argv[i + 1];
      i += 1;
    } else if (token === '--out-file') {
      args.outFile = argv[i + 1];
      i += 1;
    }
  }

  return args;
}

const OVERRIDE_EXPECTATIONS = {
  'GET /health': {
    expected_statuses: [200],
    required_response_fields: ['status'],
  },
  'POST /ripeness/predict': {
    expected_statuses: [200],
    required_response_fields: ['label', 'confidence'],
  },
};

function buildInventory(repoRoot) {
  const aspnet = loadAspNetRouteSurface(repoRoot);
  const fastify = loadFastifyRouteSurface(repoRoot);

  const aspnetByKey = new Map(aspnet.map((route) => [route.routeKey, route]));
  const fastifyByKey = new Map(fastify.map((route) => [route.routeKey, route]));

  const overlapKeys = Array.from(aspnetByKey.keys())
      .filter((key) => fastifyByKey.has(key))
      .sort((a, b) => a.localeCompare(b));

  const routes = overlapKeys.map((routeKey) => {
    const left = aspnetByKey.get(routeKey);
    const right = fastifyByKey.get(routeKey);
    const override = OVERRIDE_EXPECTATIONS[routeKey] ?? {
      expected_statuses: [200],
      required_response_fields: [],
    };

    return {
      route_key: routeKey,
      method: left.method,
      path: left.path,
      aspnet: {
        declared_in: left.declared_in,
        declaration: left.declaration,
      },
      fastify: {
        declared_in: right.declared_in,
        declaration: right.declaration,
      },
      expected_statuses: override.expected_statuses,
      required_response_fields: override.required_response_fields,
    };
  });

  return {
    generated_at_utc: new Date().toISOString(),
    source_pair: {
      aspnet: 'src/c-sharp/asp.net',
      fastify: 'src/typescript/api/src',
    },
    routes,
  };
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  const inventory = buildInventory(args.repoRoot);
  const outFile = path.isAbsolute(args.outFile) ? args.outFile : path.join(args.repoRoot, args.outFile);

  mkdirSync(path.dirname(outFile), {recursive: true});
  writeFileSync(outFile, `${JSON.stringify(inventory, null, 2)}\n`, 'utf8');

  console.log(`[banana] overlap inventory generated (${inventory.routes.length} route(s))`);
  console.log(`[banana] wrote ${path.relative(args.repoRoot, outFile).replace(/\\/g, '/')}`);
}

main();
