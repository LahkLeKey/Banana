# Why We Use A Wrapper

> Read the [Wiki Home](Home.md) for more details.

Related pages: [How A Request Works](How-A-Request-Works.md), [How The Database Step Works](How-The-Database-Step-Works.md)

This page explains the wrapper idea in simple terms.

## Problem

If the managed app calls native internals directly, every internal native change can break the app.

## Solution

Use one stable wrapper layer as the only native entry point.

That means:

- managed code talks to wrapper only,
- wrapper talks to native core domain and native DAL domain internals,
- core and dal can evolve with lower risk to API and Electron callers.

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

## Rule Of Thumb

When you add native behavior:

1. Put business logic in `src/native/core/domain` or `src/native/core/dal/domain`.
2. Expose only required functions through wrapper modules.
3. Keep managed and Electron callers on wrapper exports.
4. Keep ABI changes explicit and coordinated with interop tests.
