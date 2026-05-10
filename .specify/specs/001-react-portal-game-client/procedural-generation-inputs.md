# Procedural Generation Inputs

## Purpose

This file defines the deterministic input contract for Banana Engineer baseline generated assets under spec 001.

## Source of Truth

- The canonical build-preparation defaults live in `src/typescript/react/scripts/prepare-procedural-assets.mjs`.
- The resolved manifest and generated terrain/palette bundles are written to `src/typescript/react/public/generated-assets/` for the portal build contract.
- Generated asset outputs must stay aligned with the current React build defaults unless this file is updated first.

## Canonical Inputs

- `seed`: primary deterministic RNG seed for WFC tile collapse, cellular automata tie breaks, elevation noise, and resource noise.
- `profile`: named generation profile for the baseline landing and shell asset set.
- `width`: generated terrain width in cells.
- `height`: generated terrain height in cells.
- `cellularAutomataIterations`: post-process smoothing pass count applied after the initial WFC tile solution.

## Baseline Defaults

- `seed=20260509`
- `profile=default`
- `width=48`
- `height=48`
- `cellularAutomataIterations=3`

## Rule Contract

- Algorithm pipeline: `wave-function-collapse+cellular-automata`
- Tile vocabulary source: `banana_tile_defs`
- Adjacency rule: neighboring biomes may differ by at most `1`
- Tile selection: weighted random selection from tile definitions
- Elevation noise range: `[-5, 5]`
- Resource noise range: `[-3, 3]`

## Visual Rendering Contract

- Palette output must include stable display color roles and hex values for the generated terrain set, not only abstract biome or elevation labels.
- Palette color assignments must remain deterministic for the same input tuple so visual output is reproducible across runs.
- Terrain colors must remain visually distinguishable from entity mesh colors at both mobile and desktop viewport sizes.
- Any palette adjustment that changes visible terrain readability must be treated as a contract change for the generated asset bundle.

## Runtime and Build Wiring

- React build preparation reads environment variables in `src/typescript/react/scripts/prepare-procedural-assets.mjs`
- Override precedence is: environment variables -> build script defaults -> compiler CLI arguments -> generated manifest.
- Supported environment overrides:
  - `BANANA_GENERATED_ASSET_SEED`
  - `BANANA_GENERATED_ASSET_PROFILE`
  - `BANANA_GENERATED_ASSET_WIDTH`
  - `BANANA_GENERATED_ASSET_HEIGHT`
  - `BANANA_C_COMPILER`
  - `BANANA_REQUIRE_GENERATED_ASSETS`
  - `BANANA_SKIP_GENERATED_ASSET_COMPILE`
- Native compiler CLI inputs:
  - `--seed`
  - `--profile`
  - `--width`
  - `--height`
  - `--ca-iterations`
  - `--out-dir`

## Output Contract

- Canonical manifest: `banana-generated-assets.manifest.json`
- Terrain bundle: `banana-generated-terrain.json`
- Palette bundle: `banana-generated-palette.json`
- The manifest must include the deterministic inputs and rule metadata used for the generated bundle.

## Acceptance Notes

- Re-running the compiler with the same input tuple must produce an equivalent terrain checksum.
- Changing the seed must change the terrain checksum.
- The React engine route consumes the canonical manifest and generated terrain/palette bundle without manual asset authoring.
