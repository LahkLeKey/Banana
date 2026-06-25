# ADR 0001: Acquire banana-k3h4-model from the published GitHub release tarball

Status: Accepted

## Context

Banana already publishes a standalone K3H4 release bundle from the existing release workflow. The new vcpkg package should stay thin and avoid re-implementing release assembly during install.

The package is intended to be architecture-independent data only. Consumers should install the model artifact and inspect a small loader manifest rather than reconstructing release URLs or release layout themselves.

## Decision

The vcpkg port for `banana-k3h4-model` will download the published GitHub release tarball produced by the current K3H4 release workflow.

The port will not build the model artifact from source during `vcpkg install`.

## Consequences

- Packaging logic remains centralized in the existing release pipeline.
- The port stays a small adapter over GitHub releases rather than a second build system.
- Downstream consumers get a deterministic install surface tied to an exact release version.
- Network access is required at install time unless vcpkg caches the release artifact.

## Rationale

This choice maximizes leverage at the seam. The release workflow remains the authoritative producer, while vcpkg acts as a consumer-facing adapter.
