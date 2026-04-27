#!/usr/bin/env node

function normalizePath(path) {
  const raw = String(path ?? '').trim();
  if (raw.length === 0) return '/';

  let next = raw.startsWith('/') ? raw : `/${raw}`;
  next = next.replace(/\/+/g, '/');
  if (next.length > 1 && next.endsWith('/')) {
    next = next.slice(0, -1);
  }
  return next;
}

export function normalizeRouteKey(method, path) {
  const normalizedMethod = String(method ?? '').trim().toUpperCase();
  const normalizedPath = normalizePath(path);
  return {
    method: normalizedMethod,
    path: normalizedPath,
    routeKey: `${normalizedMethod} ${normalizedPath}`,
  };
}

export function normalizePathOnly(path) {
  return normalizePath(path);
}
