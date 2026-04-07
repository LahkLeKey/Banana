# Architecture

## Intent

This repository modernizes a legacy C codebase by wrapping it with a stable ABI and exposing it through a .NET 8 Web API.
Legacy C remains untouched and isolated behind a dedicated wrapper boundary.

## Layers

- `src/native/legacy`: existing stable C logic (no direct managed consumption)
- `src/native/wrapper`: C ABI-safe exports, validation, and memory ownership rules
- `src/api`: ASP.NET Core Web API + P/Invoke adapter
- `src/electron`: optional ffi-napi bridge
- `tests`: unit and integration suites

## Boundary Rules

- Managed code only calls `src/native/wrapper` exports.
- Wrapper translates input validation and status outcomes.
- Wrapper owns native allocation and publishes a dedicated free API.
- No internal legacy headers are exposed to C# or Node.
