#!/usr/bin/env node

import {
  cpSync,
  existsSync,
  mkdirSync,
  readdirSync,
  readFileSync,
  rmSync,
  writeFileSync,
} from "node:fs";
import { spawnSync } from "node:child_process";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const reactRoot = resolve(__dirname, "..");
const publicCharacterDir = resolve(reactRoot, "public/characters");
const requireAssets = process.env.BANANA_REQUIRE_CHARACTER_ASSETS === "true";
const generatedCharacterRoot = resolve(reactRoot, "../../../artifacts/generated-characters");
const compilerBuildDir = resolve(reactRoot, "../../../artifacts/native-tools");
const characterCompilerSource = resolve(
  reactRoot,
  "../../../src/native/engine/tools/banana_character_vector_compiler.c"
);

function failOrWarn(message) {
  if (requireAssets) {
    throw new Error(message);
  }
  console.warn(`[prepare-character-assets] ${message}`);
}

function tryReadJson(filePath) {
  try {
    return JSON.parse(readFileSync(filePath, "utf8"));
  } catch {
    return null;
  }
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

function compileCharacterGenerator(binaryPath) {
  if (!existsSync(characterCompilerSource)) {
    failOrWarn(`character compiler source not found at ${characterCompilerSource}`);
    return false;
  }

  const compiler = findCompiler();
  if (!compiler) {
    failOrWarn("no C compiler found (checked BANANA_C_COMPILER, cc, clang, gcc)");
    return false;
  }

  mkdirSync(compilerBuildDir, { recursive: true });

  const compileArgs = [
    characterCompilerSource,
    "-std=c11",
    "-O2",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
  ];
  if (process.platform !== "win32") {
    compileArgs.push("-lm");
  }
  compileArgs.push("-o", binaryPath);

  const compile = spawnSync(
    compiler,
    compileArgs,
    { encoding: "utf8" }
  );

  if (compile.status !== 0) {
    failOrWarn(
      `failed to compile banana_character_vector_compiler: ${compile.stderr || compile.stdout}`
    );
    return false;
  }

  return true;
}

function runCharacterGenerator(binaryPath) {
  rmSync(generatedCharacterRoot, { recursive: true, force: true });
  mkdirSync(generatedCharacterRoot, { recursive: true });

  const characterId = process.env.BANANA_CHARACTER_ID ?? "banana-tempest";

  const run = spawnSync(
    binaryPath,
    ["--out-dir", generatedCharacterRoot, "--character-id", characterId],
    { encoding: "utf8" }
  );

  if (run.status !== 0) {
    failOrWarn(
      `banana_character_vector_compiler failed: ${run.stderr || run.stdout}`
    );
    return false;
  }

  if (run.stdout?.trim()) {
    console.log(run.stdout.trim());
  }

  return true;
}

function selectCharacterRoot() {
  if (!existsSync(generatedCharacterRoot)) {
    return null;
  }

  const dirs = readdirSync(generatedCharacterRoot, { withFileTypes: true }).filter((entry) =>
    entry.isDirectory()
  );
  return dirs.length > 0 ? generatedCharacterRoot : null;
}

function main() {
  if (process.env.BANANA_SKIP_CHARACTER_VECTOR_COMPILE !== "true") {
    const binaryName =
      process.platform === "win32"
        ? "banana_character_vector_compiler.exe"
        : "banana_character_vector_compiler";
    const binaryPath = resolve(compilerBuildDir, binaryName);
    const compiled = compileCharacterGenerator(binaryPath);
    if (compiled) {
      runCharacterGenerator(binaryPath);
    }
  }

  const characterRoot = selectCharacterRoot();
  if (!characterRoot) {
    failOrWarn(`character source folder not found in ${generatedCharacterRoot}`);
    return;
  }

  const characterIds = readdirSync(characterRoot, { withFileTypes: true })
    .filter((entry) => entry.isDirectory())
    .map((entry) => entry.name)
    .sort();

  if (characterIds.length === 0) {
    failOrWarn(`no character packs found in ${characterRoot}`);
    return;
  }

  rmSync(publicCharacterDir, { recursive: true, force: true });
  mkdirSync(publicCharacterDir, { recursive: true });

  const manifestEntries = [];

  for (const characterId of characterIds) {
    const sourcePackDir = join(characterRoot, characterId);
    const targetPackDir = join(publicCharacterDir, characterId);
    cpSync(sourcePackDir, targetPackDir, { recursive: true });

    const characterJson = join(targetPackDir, "character.2p5d.json");
    const parsed = existsSync(characterJson) ? tryReadJson(characterJson) : null;
    manifestEntries.push({
      id: characterId,
      file: `${characterId}/character.2p5d.json`,
      clips: parsed?.clips?.map((clip) => clip.name) ?? [],
    });
  }

  const manifest = {
    kind: "banana-character-asset-manifest",
    importedFrom: characterRoot,
    characters: manifestEntries,
  };

  writeFileSync(
    join(publicCharacterDir, "banana-character-assets.manifest.json"),
    `${JSON.stringify(manifest, null, 2)}\n`,
    "utf8"
  );

  console.log(
    `[prepare-character-assets] copied ${characterIds.length} character pack(s) from ${characterRoot} to ${publicCharacterDir}`
  );
}

main();
