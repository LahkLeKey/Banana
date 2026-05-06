import { copyFileSync, existsSync, mkdirSync, readdirSync, rmSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const reactRoot = resolve(__dirname, "..");
const wasmSourceDir = resolve(reactRoot, "../../../artifacts/wasm");
const wasmPublicDir = resolve(reactRoot, "public/wasm");

const requireAssets = process.env.BANANA_REQUIRE_WASM_ASSETS === "true";

function failOrWarn(message) {
  if (requireAssets) {
    throw new Error(message);
  }
  console.warn(`[prepare-wasm-assets] ${message}`);
}

function shouldCopy(name) {
  return (
    name.startsWith("banana-wasm") &&
    (name.endsWith(".js") || name.endsWith(".wasm") || name.endsWith(".json"))
  );
}

function main() {
  if (!existsSync(wasmSourceDir)) {
    failOrWarn(`source directory not found: ${wasmSourceDir}`);
    return;
  }

  const files = readdirSync(wasmSourceDir).filter(shouldCopy);
  if (files.length === 0) {
    failOrWarn(`no banana-wasm artifacts found in ${wasmSourceDir}`);
    return;
  }

  rmSync(wasmPublicDir, { recursive: true, force: true });
  mkdirSync(wasmPublicDir, { recursive: true });

  for (const file of files) {
    copyFileSync(join(wasmSourceDir, file), join(wasmPublicDir, file));
  }

  console.log(
    `[prepare-wasm-assets] copied ${files.length} artifact(s) from ${wasmSourceDir} to ${wasmPublicDir}`,
  );
}

main();