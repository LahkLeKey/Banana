#!/usr/bin/env node

import {readdirSync, readFileSync} from 'node:fs';
import path from 'node:path';

import {normalizeRouteKey} from './normalize-route-key.mjs';

function walkTsFiles(rootDir, output = []) {
  for (const item of readdirSync(rootDir, {withFileTypes: true})) {
    const fullPath = path.join(rootDir, item.name);
    if (item.isDirectory()) {
      walkTsFiles(fullPath, output);
      continue;
    }
    if (item.isFile() && item.name.endsWith('.ts') && !item.name.endsWith('.test.ts')) {
      output.push(fullPath);
    }
  }
  return output;
}

export function loadFastifyRouteSurface(repoRoot) {
  const srcRoot = path.join(repoRoot, 'src', 'typescript', 'api', 'src');
  const files = walkTsFiles(srcRoot);
  const routes = [];

  const routeRegex = /app\.(get|post|put|delete|patch)\('([^']+)'/g;

  for (const filePath of files) {
    const relativePath = path.relative(repoRoot, filePath).replace(/\\/g, '/');
    const source = readFileSync(filePath, 'utf8');

    let match = routeRegex.exec(source);
    while (match !== null) {
      const method = match[1].toUpperCase();
      const route = match[2];
      const normalized = normalizeRouteKey(method, route);

      routes.push({
        ...normalized,
        source: 'fastify',
        declared_in: relativePath,
        declaration: `app.${match[1]}('${route}')`,
      });

      match = routeRegex.exec(source);
    }
  }

  const deduped = new Map();
  for (const route of routes) {
    if (!deduped.has(route.routeKey)) {
      deduped.set(route.routeKey, route);
    }
  }

  return Array.from(deduped.values()).sort((a, b) => a.routeKey.localeCompare(b.routeKey));
}
