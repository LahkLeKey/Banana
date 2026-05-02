# Tasks: 086-supply-chain-sbom-and-signing

**Implemented in commit**: `1ee0635`
**Summary**: SBOM generation and container image signing with Cosign/Syft

## Tasks

- [x] **T001 Add scripts/generate-sbom.sh using Syft to produce CycloneDX SBOM**
- [x] **T002 Add scripts/sign-container.sh using Cosign for image signing**
- [x] **T003 Wire SBOM generation and signing into .github/workflows/docker-build.yml**
- [x] **T004 Add SBOM verification step in .github/workflows/security-scan.yml**

_All tasks completed in batch implementation commit._
