#!/usr/bin/env node

import {mkdirSync, writeFileSync} from 'node:fs';
import path from 'node:path';

import {loadAspNetRouteSurface} from './lib/load-aspnet-route-surface.mjs';
import {loadFastifyRouteSurface} from './lib/load-fastify-route-surface.mjs';
import {
  createFinding,
  sortFindings,
  summarizeFindings,
  utcNowIso,
} from './lib/parity-finding-model.mjs';

function parseArgs(argv) {
  const args = {
    repoRoot: process.cwd(),
    outFile: '.specify/specs/047-api-parity-governance/artifacts/parity-drift-report.json',
    strict: false,
  };

  for (let i = 0; i < argv.length; i += 1) {
    const token = argv[i];
    if (token === '--repo-root') {
      args.repoRoot = argv[i + 1];
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

export function buildMissingRouteFindings(repoRoot) {
  const aspnet = loadAspNetRouteSurface(repoRoot);
  const fastify = loadFastifyRouteSurface(repoRoot);

  const aspnetByKey = new Map(aspnet.map((route) => [route.routeKey, route]));
  const fastifyByKey = new Map(fastify.map((route) => [route.routeKey, route]));

  const findings = [];

  for (const [routeKey, aspnetRoute] of aspnetByKey.entries()) {
    if (!fastifyByKey.has(routeKey)) {
      findings.push(createFinding({
        routeKey,
        mismatchType: 'missing_route',
        aspnet: {declared_in: aspnetRoute.declared_in},
        fastify: null,
        remediation: 'Add matching Fastify route or document approved temporary exception.',
      }));
    }
  }

  for (const [routeKey, fastifyRoute] of fastifyByKey.entries()) {
    if (!aspnetByKey.has(routeKey)) {
      findings.push(createFinding({
        routeKey,
        mismatchType: 'missing_route',
        aspnet: null,
        fastify: {declared_in: fastifyRoute.declared_in},
        remediation: 'Add matching ASP.NET route or document approved temporary exception.',
      }));
    }
  }

  return sortFindings(findings);
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  const findings = buildMissingRouteFindings(args.repoRoot);
  const summary = summarizeFindings(findings);

  const payload = {
    generated_at_utc: utcNowIso(),
    strict_mode: args.strict,
    summary,
    findings,
  };

  const outFile = path.isAbsolute(args.outFile) ? args.outFile : path.join(args.repoRoot, args.outFile);
  mkdirSync(path.dirname(outFile), {recursive: true});
  writeFileSync(outFile, `${JSON.stringify(payload, null, 2)}\n`, 'utf8');

  if (summary.total_findings > 0) {
    console.log(`[banana] parity gaps detected (${summary.total_findings})`);
    if (args.strict) {
      process.exit(1);
    }
    return;
  }

  console.log('[banana] no parity gaps detected');
}

main();
