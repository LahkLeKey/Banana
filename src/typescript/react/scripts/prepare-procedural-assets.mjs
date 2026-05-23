#!/usr/bin/env node

import { copyFileSync, existsSync, mkdirSync, readdirSync, rmSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { spawnSync } from "node:child_process";
import { fileURLToPath } from "node:url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const reactRoot = resolve(__dirname, "..");

const compilerSource = resolve(
  reactRoot,
  "../../../src/native/engine/tools/banana_asset_compiler.c"
);
const compilerSourceFiles = [
  resolve(reactRoot, "../../../src/native/engine/tools/banana_asset_compiler.c"),
  resolve(reactRoot, "../../../src/native/engine/tools/domain/banana_asset_domain.c"),
  resolve(reactRoot, "../../../src/native/engine/tools/domain/banana_asset_algorithms.c"),
  resolve(
    reactRoot,
    "../../../src/native/engine/tools/application/banana_asset_generation_service.c"
  ),
  resolve(reactRoot, "../../../src/native/engine/tools/infrastructure/banana_asset_json_writer.c"),
];
const compilerBuildDir = resolve(reactRoot, "../../../artifacts/native-tools");
const generatedSourceDir = resolve(reactRoot, "../../../artifacts/generated-assets");
const generatedPublicDir = resolve(reactRoot, "public/generated-assets");

const requireAssets = process.env.BANANA_REQUIRE_GENERATED_ASSETS === "true";
const isVercelBuild = process.env.VERCEL === "1";
const generatedAssetPolicy = (process.env.BANANA_GENERATED_ASSET_POLICY ?? "auto").trim().toLowerCase();
const compileMode = (process.env.BANANA_GENERATED_ASSET_COMPILE_MODE ??
  (isVercelBuild ? "skip" : "auto")).trim().toLowerCase();
const strictMode = requireAssets || generatedAssetPolicy === "strict";
const seed = process.env.BANANA_GENERATED_ASSET_SEED ?? "20260509";
const profile = process.env.BANANA_GENERATED_ASSET_PROFILE ?? "default";
const width = process.env.BANANA_GENERATED_ASSET_WIDTH ?? "48";
const height = process.env.BANANA_GENERATED_ASSET_HEIGHT ?? "48";

function failOrWarn(message, detail) {
  const rendered = detail ? `${message}: ${detail}` : message;
  if (strictMode) {
    throw new Error(rendered);
  }
  console.warn(`[prepare-procedural-assets] ${rendered}`);
}

function resolveCompileMode() {
  if (["skip", "auto", "force"].includes(compileMode)) {
    return compileMode;
  }

  failOrWarn(
    "invalid BANANA_GENERATED_ASSET_COMPILE_MODE, expected one of skip|auto|force",
    `received ${compileMode}`
  );
  return "auto";
}

function resolveAssetPolicy() {
  if (["auto", "strict"].includes(generatedAssetPolicy)) {
    return generatedAssetPolicy;
  }

  failOrWarn(
    "invalid BANANA_GENERATED_ASSET_POLICY, expected one of auto|strict",
    `received ${generatedAssetPolicy}`
  );
  return "auto";
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

function compileBinary(binaryPath) {
  if (!existsSync(compilerSource)) {
    failOrWarn(`compiler source not found at ${compilerSource}`);
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
    [...compilerSourceFiles, "-std=c11", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-o", binaryPath],
    { encoding: "utf8" }
  );

  if (compile.status !== 0) {
    failOrWarn("failed to compile banana_asset_compiler", compile.stderr || compile.stdout);
    return false;
  }

  return true;
}

function runCompiler(binaryPath) {
  rmSync(generatedSourceDir, { recursive: true, force: true });
  mkdirSync(generatedSourceDir, { recursive: true });

  const run = spawnSync(
    binaryPath,
    [
      "--out-dir",
      generatedSourceDir,
      "--seed",
      seed,
      "--profile",
      profile,
      "--width",
      width,
      "--height",
      height,
    ],
    { encoding: "utf8" }
  );

  if (run.status !== 0) {
    failOrWarn("banana_asset_compiler failed", run.stderr || run.stdout);
    return false;
  }

  if (run.stdout?.trim()) {
    console.log(run.stdout.trim());
  }

  return true;
}

function shouldCopy(name) {
  return name.startsWith("banana-generated-") && name.endsWith(".json");
}

function copyToPublic() {
  if (!existsSync(generatedSourceDir)) {
    failOrWarn(`generated source directory not found: ${generatedSourceDir}`);
    return;
  }

  const files = readdirSync(generatedSourceDir).filter(shouldCopy);
  if (files.length === 0) {
    failOrWarn(`no generated assets found in ${generatedSourceDir}`);
    return;
  }

  rmSync(generatedPublicDir, { recursive: true, force: true });
  mkdirSync(generatedPublicDir, { recursive: true });

  for (const file of files) {
    copyFileSync(join(generatedSourceDir, file), join(generatedPublicDir, file));
  }

  console.log(
    `[prepare-procedural-assets] copied ${files.length} generated asset(s) from ${generatedSourceDir} to ${generatedPublicDir}`
  );
}

function main() {
  const binaryName =
    process.platform === "win32" ? "banana_asset_compiler.exe" : "banana_asset_compiler";
  const binaryPath = resolve(compilerBuildDir, binaryName);
  const policy = resolveAssetPolicy();
  const mode = resolveCompileMode();

  console.log(
    `[prepare-procedural-assets] policy=${policy} strict=${strictMode} compileMode=${mode} vercel=${isVercelBuild}`
  );

  if (mode === "skip" || process.env.BANANA_SKIP_GENERATED_ASSET_COMPILE === "true") {
    console.log("[prepare-procedural-assets] skipping native compiler step");
    copyToPublic();
    return;
  }

  if (mode === "auto" || mode === "force") {
    const compiled = compileBinary(binaryPath);
    if (compiled) {
      runCompiler(binaryPath);
    } else if (mode === "force") {
      failOrWarn("compile mode=force requires banana_asset_compiler to build successfully");
    }
  }

  copyToPublic();
}

main();
