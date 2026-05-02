<!-- breadcrumb: Architecture > Native Wrapper ABI -->

# Native Wrapper ABI

> [Home](../Home.md) › [Architecture](README.md) › Native Wrapper ABI

Related pages: [How A Request Works](../getting-started/how-a-request-works.md), [Database Pipeline Stage](database-pipeline-stage.md)

This page explains the wrapper design in simple terms.

## Problem

If the managed app calls native internals directly, every internal native change can break the app.

## Solution

Use one stable wrapper layer as the only native entry point.

That means:

- managed code talks to wrapper only,
- wrapper talks to native core domain and native DAL domain internals,
- core and DAL can evolve with lower risk to API and Electron callers.

## Benefits

- safer refactors,
- fewer breaking changes,
- clearer ownership of memory and status codes,
- easier onboarding for new contributors,
- one ABI reused by ASP.NET and Electron smoke bridges.

## What The Wrapper Owns

- exported C ABI in `src/native/wrapper/banana_wrapper.h`
- status code boundary between native and managed callers
- native string/payload allocation paired with `banana_free`
- route-level native operations for banana profile, batch lifecycle, logistics, and ripeness

## ABI Versioning

The wrapper exports `banana_native_version(int* out_major, int* out_minor)` to allow callers to assert compatibility at runtime.

Current ABI: major=2, minor=4.

Breaking changes (major bump) require coordination across managed interop, wrapper, and all downstream callers.

## Rule Of Thumb

When you add native behavior:

1. Put business logic in `src/native/core/domain` or `src/native/core/dal/domain`.
2. Expose only required functions through wrapper modules.
3. Keep managed and Electron callers on wrapper exports.
4. Keep ABI changes explicit and coordinated with interop tests.
