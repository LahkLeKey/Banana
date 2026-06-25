# K3H4 Loader Port Design: Optimize for Common Caller

## Problem & Constraint
Current design ships raw model data + consumer must handle ABI versions, linking, and data discovery. **Constraint: make the default case trivial.**

---

## 1. Proposed Consumer Interface: `k3h4.h`

```c
#pragma once
#include <stdint.h>

typedef struct k3h4_handle_t k3h4_handle_t;
typedef struct k3h4_query_result_t {
    float similarity;      /* 0.0 to 1.0 */
    uint32_t model_id;
    char label[256];
} k3h4_query_result_t;

/* Load model from embedded/bundled data; returns NULL on failure */
k3h4_handle_t* k3h4_load(void);

/* Query model; result must be freed with k3h4_free_result() */
k3h4_query_result_t* k3h4_query(k3h4_handle_t* h, const float* features, uint32_t feature_count);

/* Free handle */
void k3h4_close(k3h4_handle_t* h);
void k3h4_free_result(k3h4_query_result_t* r);

/* Get model version (e.g., "2026.06.14") */
const char* k3h4_version(void);
```

**Consumer code is trivial:**
```c
k3h4_handle_t* model = k3h4_load();
k3h4_query_result_t* result = k3h4_query(model, features, 128);
printf("Match: %s (%.2f)\n", result->label, result->similarity);
k3h4_free_result(result);
k3h4_close(model);
```

---

## 2. Package Layout

```
share/banana-k3h4-model/
  bin/
    libk3h4-loader.a           # Static loader library (includes embedded model)
  include/
    k3h4.h                     # Public interface
  share/
    k3h4-model-metadata.json   # Version, model info (human-readable)
    copyright
  lib/
    pkgconfig/
      k3h4-loader.pc           # CMake/pkg-config integration
```

**Key: Model data is *embedded* in the static library via objcopy, not side-loaded.**

---

## 3. vcpkg.json

```json
{
  "name": "banana-k3h4-model",
  "version": "2026.06.14",
  "description": "Pre-compiled K3H4 loader: stateless query interface, ABI-agnostic",
  "dependencies": [],
  "features": {
    "shared": {
      "description": "Build as shared library instead of static"
    }
  }
}
```

---

## 4. portfile.cmake (Simplified)

```cmake
vcpkg_minimum_required(VERSION 2024-01-01)

set(PORT_VERSION "2026.06.14")
set(TARBALL_URL "https://github.com/LahkLeKey/Banana/releases/download/k3h4-model-v${PORT_VERSION}/k3h4-loader-${PORT_VERSION}.tar.gz")

vcpkg_download_distfile(TARBALL URLS "${TARBALL_URL}" FILENAME "k3h4-loader-${PORT_VERSION}.tar.gz" SHA512 "PLACEHOLDER")

file(ARCHIVE_EXTRACT INPUT "${TARBALL}" DESTINATION "${CURRENT_BUILDTREES_DIR}/src")

# Copy pre-built loader library and header
file(COPY "${CURRENT_BUILDTREES_DIR}/src/lib/libk3h4-loader.a" DESTINATION "${CURRENT_PACKAGES_DIR}/lib")
file(COPY "${CURRENT_BUILDTREES_DIR}/src/include/k3h4.h" DESTINATION "${CURRENT_PACKAGES_DIR}/include")
file(COPY "${CURRENT_BUILDTREES_DIR}/src/share/" DESTINATION "${CURRENT_PACKAGES_DIR}/share/banana-k3h4-model")
```

---

## 5. What You Gain vs. Trade-offs

| Gain | Trade-off |
|------|-----------|
| **One-liner load** → no version negotiation | Slower iteration if model changes (rebuild release) |
| **No linking against full Banana ABI** → smaller binaries | Model data duplication across consumers |
| **Stateless queries** → thread-safe by design | No lazy-load; full model in memory |
| **ABI transparency** → consumer never sees native versioning | Release cadence couples model + loader binary |

---

## 6. Consumer Integration Example

**CMakeLists.txt:**
```cmake
find_package(banana-k3h4-model REQUIRED)
add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE banana::k3h4-loader)
```

**No env vars. No data paths. No ABI version discovery. Defaults work.**

---

## Release Pipeline Impact

- `build-k3h4-standalone-release.sh` builds loader binary (not data tarball)
- `libk3h4-loader.a` is pre-built; vcpkg just downloads & copies
- Breaking model changes → version bump & new release tag
