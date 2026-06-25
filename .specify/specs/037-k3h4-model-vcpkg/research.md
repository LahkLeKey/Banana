# Research: K3H4 Model Vcpkg Packaging

## Decisions Confirmed

- The vcpkg package should be named `banana-k3h4-model`.
- The package should be architecture-independent data only.
- The package should download the published release tarball rather than rebuilding the model during install.
- The package should expose a loader manifest with explicit ABI compatibility metadata.

## Implications

- The port stays thin and acts as a packaging adapter.
- The standalone release workflow remains the single release producer.
- Consumers need a stable manifest contract instead of guessing from file names.

## Open Design Edges

- Whether the manifest records a checksum of the tarball itself or a checksum of the extracted payload.
- Whether consumers discover the payload path from the manifest only or through an additional helper file.
- Whether future model variants share the same package family or become separate ports.
