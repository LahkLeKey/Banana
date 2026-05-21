#!/usr/bin/env node

import {
  existsSync,
  mkdirSync,
  readdirSync,
  rmSync,
  cpSync,
} from "node:fs";
import { spawnSync } from "node:child_process";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const reactRoot = resolve(__dirname, "..");
const compilerBuildDir = resolve(reactRoot, "../../../artifacts/native-tools");
const conceptCompilerSource = resolve(
  reactRoot,
  "../../../src/native/engine/tools/banana_concept_mesh_compiler.c"
);
const generatedConceptRoot = resolve(reactRoot, "../../../artifacts/generated-concepts");
const publicConceptDir = resolve(reactRoot, "public/concept-assets");

const requireAssets = process.env.BANANA_REQUIRE_CONCEPT_ASSETS === "true";

function failOrWarn(message) {
  if (requireAssets) {
    throw new Error(message);
  }
  console.warn(`[prepare-concept-assets] ${message}`);
}

function findCompiler() {
  const requested = process.env.BANANA_C_COMPILER;
  if (requested) {
    return requested;
  }

  const candidates = ["cc", "clang", "gcc"];
  for (const candidate of candidates) {
    const probe = spawnSync(candidate, ["--version"], { stdio: "ignore" });
    if (probe.status === 0) {
      return candidate;
    }
  }

  return null;
}

function compileConceptGenerator(binaryPath) {
  if (!existsSync(conceptCompilerSource)) {
    failOrWarn(`concept compiler source not found at ${conceptCompilerSource}`);
    return false;
  }

  const compiler = findCompiler();
  if (!compiler) {
    failOrWarn("no C compiler found (checked BANANA_C_COMPILER, cc, clang, gcc)");
    return false;
  }

  mkdirSync(compilerBuildDir, { recursive: true });
  const compile = spawnSync(
    compiler,
    [conceptCompilerSource, "-std=c11", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-o", binaryPath],
    { encoding: "utf8" }
  );

  if (compile.status !== 0) {
    failOrWarn(`failed to compile banana_concept_mesh_compiler: ${compile.stderr || compile.stdout}`);
    return false;
  }

  return true;
}

function runConceptGenerator(binaryPath) {
  rmSync(generatedConceptRoot, { recursive: true, force: true });
  mkdirSync(generatedConceptRoot, { recursive: true });
  const run = spawnSync(binaryPath, ["--out-dir", generatedConceptRoot], { encoding: "utf8" });
  if (run.status !== 0) {
    failOrWarn(`banana_concept_mesh_compiler failed: ${run.stderr || run.stdout}`);
    return false;
  }

  if (run.stdout?.trim()) {
    console.log(run.stdout.trim());
  }
  return true;
}

function main() {
  if (process.env.BANANA_SKIP_CONCEPT_MESH_COMPILE !== "true") {
    const binaryName =
      process.platform === "win32" ? "banana_concept_mesh_compiler.exe" : "banana_concept_mesh_compiler";
    const binaryPath = resolve(compilerBuildDir, binaryName);
    const compiled = compileConceptGenerator(binaryPath);
    if (compiled) {
      runConceptGenerator(binaryPath);
    }
  }

  if (!existsSync(generatedConceptRoot)) {
    failOrWarn(`generated concept root not found: ${generatedConceptRoot}`);
    return;
  }

  const files = readdirSync(generatedConceptRoot);
  if (files.length === 0) {
    failOrWarn(`no generated concept assets found in ${generatedConceptRoot}`);
    return;
  }

  rmSync(publicConceptDir, { recursive: true, force: true });
  mkdirSync(publicConceptDir, { recursive: true });
  cpSync(generatedConceptRoot, publicConceptDir, { recursive: true });

  console.log(
    `[prepare-concept-assets] copied generated concept assets from ${generatedConceptRoot} to ${publicConceptDir}`
  );
}

main();
