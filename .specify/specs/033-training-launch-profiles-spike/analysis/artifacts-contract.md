# Artifacts Contract (G4)

## Where local F5 trainer runs write outputs

**Decision**: `artifacts/training/<model>/<run-id>/`, where
`<run-id>` is the trainer-emitted UTC ISO timestamp. Local F5
runs MUST NOT overwrite committed seed corpora or committed model
images; that path is the canonical safe sandbox.

The follow-up slice updates each F5 entry with explicit
`--output ${workspaceFolder}/artifacts/training/<model>/<run>/`
arguments using a VS Code `${input}` or
`${command:extension.commandVariable.transform/now}` style timestamp
(or simply a path that includes `${userHome}`-free `${env:USERNAME}`
+ a counter -- the SPIKE leaves the exact mechanism to the
implementation slice).

## Promotion path

Promotion (local artifact -> committed model image) stays
authoritative through `scripts/manage-<model>-image.py`. The F5
runner does NOT promote -- it only produces inspectable artifacts.

## Cleanup

`artifacts/` is already gitignored (workspace convention). No new
ignore entries required. The follow-up slice may add a
`scripts/clean-training-artifacts.sh` if churn becomes a problem.

## Back-compat

The committed `data/not-banana/corpus.json` shape is unchanged.
The artifacts contract is purely additive -- existing CI
invocations that pass an explicit `--output` are unaffected.
