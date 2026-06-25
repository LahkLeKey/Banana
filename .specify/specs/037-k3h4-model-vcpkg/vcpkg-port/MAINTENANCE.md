# Port Maintenance: K3H4 Model-Only Vcpkg Port

This document covers maintenance procedures, checksums, and release dependencies for banana-k3h4-model.

## Important: Model-Only Design

This port is **model data only**. It does NOT include:
- `libbanana_native.so` (native ABI library)
- `banana_native_v3.h` (ABI headers)
- Any native code

This is intentional and by design. See [README.md](README.md) for context.

Consumers who need native interop should depend on a separate `banana-native-abi` port.

## Current Version: 2026.06.14

**GitHub Release Tag**: `k3h4-model-v2026.06.14`

**Tarball**: `k3h4-model-2026.06.14.tar.gz`

**Expected Files in Tarball** (after extraction):
- `MODEL_METADATA.json` — Required
- `data/` — Model binary directory (required; may contain .bin or other formats)
- `contracts/` — Optional; format/schema documentation
- ~~`bin/libbanana_native.so`~~ **Not included in this port**
- ~~`include/banana_native_v3.h`~~ **Not included in this port**

## Checksum Retrieval

1. Navigate to the GitHub release page:
   ```
   https://github.com/LahkLeKey/Banana/releases/tag/k3h4-model-v2026.06.14
   ```

2. Download the `.sha256` checksum file from the release assets

3. Compute the SHA512 hash locally:
   ```bash
   sha512sum k3h4-model-2026.06.14.tar.gz
   ```

4. Update the `SHA512` field in `portfile.cmake` with the computed value

## Update Procedure

When a new K3H4 model version is released:

1. Confirm the new release is published on GitHub
   - Verify tarball is named `k3h4-model-<version>.tar.gz`
   - Verify it contains `MODEL_METADATA.json` and `data/` directory
   - **Confirm native files are NOT included** (safety check)

2. Obtain the tarball SHA512 checksum:
   ```bash
   sha512sum k3h4-model-2026.06.15.tar.gz
   ```

3. Update `portfile.cmake`:
   ```cmake
   set(PORT_VERSION "2026.06.15")
   # In vcpkg_download_distfile():
   SHA512 "<new-sha512-hash>"
   ```

4. Update `vcpkg.json`:
   ```json
   {
     "version": "2026.06.15",
     "port-version": 0
   }
   ```
   (Reset `port-version` to 0 when version changes; increment it for port-only fixes)

5. Test locally:
   ```bash
   vcpkg install banana-k3h4-model
   cat $(find /usr/local -name loader-manifest.json | head -1)
   ```

6. Commit with a clear message:
   ```
   refactor(vcpkg): update banana-k3h4-model to v2026.06.15
   
   - Update PORT_VERSION to 2026.06.15
   - Update SHA512 checksum
   - Verify model-only extraction (no native files)
   ```

## Independent ABI Versioning

**Key design principle**: Model version and ABI version are INDEPENDENT.

- **Model version** (e.g., `2026.06.14`) — Date-based; released when model is trained/updated
- **ABI version** (e.g., `v3`) — Semantic; released when Banana native library changes
- The loader manifest in the model port declares the ABI it expects (advisory only)
- Consumers explicitly depend on both ports if they need native interop

This means:
- New model on June 15 can use same ABI v3 from June 14
- New ABI v4 can work with June 14 model (with appropriate manifest updates)
- Decoupled release cycles; no forced lock-step updates

## Version History

| Model Version | Release Date | Native ABI | Tarball Size | Status |
|---------------|--------------|-----------|--------------|--------|
| 2026.06.14    | 2026-06-14   | v3        | ~500 MB      | Current |

(Add new entries as versions are released)

## Troubleshooting

### "SHA512 checksum mismatch"
- Verify the tarball hash matches the release asset
- Confirm you're using the correct release tag
- Check for network corruption during download

### "K3H4 release tarball missing MODEL_METADATA.json"
- The release tarball structure is invalid
- Confirm the release was built by the correct workflow (k3h4-model-release.yml)
- Do not bump the port version until the release is fixed

### "Native library files detected in K3H4 model payload"
- The release tarball incorrectly includes native files
- This is a safety violation; do not proceed
- File an issue in the Banana repo; fix the release workflow

### Port installation succeeds but model files are empty
- Check that the release tarball extracts correctly:
  ```bash
  tar -tzf k3h4-model-2026.06.14.tar.gz | head -20
  ```
- Verify the `data/` directory exists and contains .bin files
- Re-run the release build and re-publish if needed

