# K3H4 Model Vcpkg Glossary

- `banana-k3h4-model`: The vcpkg package that distributes the K3H4 model artifact as architecture-independent data.
- `loader manifest`: A small top-level metadata file that tells consumers which version is installed, what hash to trust, where the payload lives, and which ABI version it expects.
- `payload directory`: The versioned directory that contains the actual model artifact files.
- `exact pin`: A dependency declaration that requires one specific package version, with no range or floating update.
- `native ABI version`: The Banana runtime ABI version that the model package declares compatibility against.
- `release tarball`: The GitHub release archive produced by the existing K3H4 release workflow and consumed by the vcpkg port.
- `ports-and-adapters`: The design style where the vcpkg package is the outer port and GitHub releases are an external adapter behind a seam.
- `noarch data`: Files that do not vary by target architecture and do not need triplet-specific packaging.
- `payload hash`: The content hash or checksum consumers can use to verify the installed model payload.
- `versioned payload directory`: A directory named by the model version so multiple releases can coexist without collisions.
