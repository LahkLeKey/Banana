#!/usr/bin/env node
// Slice 023 -- dedupe react/react-dom across `@banana/ui` and the React
// app. When Bun installs both packages, each gets its own copy of react
// (via peerDependencies auto-install), which causes "Invalid hook call"
// errors at runtime because EscalationPanel's react reference doesn't
// match the host App's. We collapse them by symlinking shared/ui's
// react + react-dom to the React app's copies.
//
// Runs as a postinstall hook so a fresh `bun install` keeps the dedupe
// in place.

import { existsSync, lstatSync, mkdirSync, readlinkSync, rmSync, symlinkSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const reactAppRoot = resolve(__dirname, "..");
const sharedUiNm = resolve(reactAppRoot, "..", "shared", "ui", "node_modules");

if (!existsSync(sharedUiNm)) {
  mkdirSync(sharedUiNm, { recursive: true });
}

for (const pkg of ["react", "react-dom"]) {
  const target = join(reactAppRoot, "node_modules", pkg);
  const link = join(sharedUiNm, pkg);
  if (!existsSync(target)) {
    console.warn(`[dedupe-react] missing target ${target}, skipping ${pkg}`);
    continue;
  }
  if (existsSync(link) || lstatSyncSafe(link)) {
    rmSync(link, { recursive: true, force: true });
  }
  // Use junction on Windows so admin rights aren't required.
  const type = process.platform === "win32" ? "junction" : "dir";
  symlinkSync(target, link, type);
  console.log(`[dedupe-react] linked ${pkg} -> ${target}`);
}

function lstatSyncSafe(p) {
  try {
    return lstatSync(p);
  } catch {
    return null;
  }
}
