#!/usr/bin/env node

import {existsSync, readdirSync, readFileSync} from 'node:fs';
import path from 'node:path';

import {normalizeRouteKey} from './normalize-route-key.mjs';

function resolveLegacyApiRoot(repoRoot) {
  const configuredRoot = process.env.BANANA_API_PARITY_LEGACY_ROOT;
  if (!configuredRoot) {
    return null;
  }

  return path.isAbsolute(configuredRoot)
    ? configuredRoot
    : path.join(repoRoot, configuredRoot);
}

function loadControllerRoutes(repoRoot) {
  const legacyRoot = resolveLegacyApiRoot(repoRoot);
  const routes = [];

  if (!legacyRoot) {
    return routes;
  }

  const controllersDir = path.join(legacyRoot, 'Controllers');
  if (!existsSync(controllersDir)) {
    return routes;
  }

  for (const fileName of readdirSync(controllersDir)) {
    if (!fileName.endsWith('.cs')) continue;
    const fullPath = path.join(controllersDir, fileName);
    const relativePath = path.relative(repoRoot, fullPath).replace(/\\/g, '/');
    const source = readFileSync(fullPath, 'utf8');

    const routeMatch = source.match(/\[Route\("([^"]+)"\)\]/);
    const baseRoute = routeMatch ? routeMatch[1].trim() : '';

    const attrRegex = /\[(HttpGet|HttpPost|HttpPut|HttpDelete|HttpPatch)\("([^"]*)"\)\]/g;
    let match = attrRegex.exec(source);
    while (match !== null) {
      const verbAttr = match[1];
      const methodPath = match[2].trim();
      const method = verbAttr.replace('Http', '').toUpperCase();

      let finalPath;
      if (methodPath.startsWith('/')) {
        finalPath = methodPath;
      } else if (methodPath.length === 0) {
        finalPath = baseRoute;
      } else if (baseRoute.length === 0) {
        finalPath = methodPath;
      } else {
        finalPath = `${baseRoute}/${methodPath}`;
      }

      const normalized = normalizeRouteKey(method, finalPath);
      routes.push({
        ...normalized,
        source: 'aspnet',
        declared_in: relativePath,
        declaration: match[0],
      });
      match = attrRegex.exec(source);
    }
  }

  return routes;
}

function loadProgramMapRoutes(repoRoot) {
  const legacyRoot = resolveLegacyApiRoot(repoRoot);
  if (!legacyRoot) {
    return [];
  }

  const programPath = path.join(legacyRoot, 'Program.cs');
  if (!existsSync(programPath)) {
    return [];
  }

  const relativePath = path.relative(repoRoot, programPath).replace(/\\/g, '/');
  const source = readFileSync(programPath, 'utf8');
  const routes = [];

  const mapRegex = /app\.Map(Get|Post|Put|Delete|Patch)\("([^"]+)"/g;
  let match = mapRegex.exec(source);
  while (match !== null) {
    const method = String(match[1]).toUpperCase();
    const route = match[2];
    const normalized = normalizeRouteKey(method, route);
    routes.push({
      ...normalized,
      source: 'aspnet',
      declared_in: relativePath,
      declaration: `app.Map${match[1]}(\"${route}\")`,
    });
    match = mapRegex.exec(source);
  }

  return routes;
}

export function loadAspNetRouteSurface(repoRoot) {
  const combined = [...loadControllerRoutes(repoRoot), ...loadProgramMapRoutes(repoRoot)];
  const deduped = new Map();

  for (const route of combined) {
    if (!deduped.has(route.routeKey)) {
      deduped.set(route.routeKey, route);
    }
  }

  return Array.from(deduped.values()).sort((a, b) => a.routeKey.localeCompare(b.routeKey));
}
