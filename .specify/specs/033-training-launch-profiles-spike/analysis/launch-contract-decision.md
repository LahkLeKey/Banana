# Launch Contract Decision (G2 + G3)

## G2 -- Per-profile entries vs input-variable

**Decision**: Per-profile entries (one launch config per trainer x
profile pair). The set is small (3 profiles per trainer, 1
trainer today) and explicit names are easier to scan in the F5
dropdown than a prompt-on-launch.

Naming: `Train: <model> (<profile> profile)` -- e.g.
`Train: not-banana (ci profile)`.

When the suite grows (SPIKE 034 adds banana + ripeness), the
follow-up slice adds `Train: banana (ci profile)`,
`Train: ripeness (ci profile)`, etc. A `compounds`-style
"Train: all (ci profile)" entry is deferred until contributors
ask for it.

Rejected alternative: a single `Train: <model>` entry with a
VS Code `inputs` prompt for the profile. Reason: prompts every
F5 -> friction in the inner loop; profile names already work as
discriminators.

## G3 -- Debugger contract

**Decision**: Use `type: debugpy`. Interpreter resolution is
delegated to the Python extension's standard `python.defaultInterpreterPath`
+ workspace `.venv` discovery; F5 entries do not pin
`pythonPath` so contributors can swap envs (Windows
`.venv/Scripts/python.exe`, Ubuntu `.venv/bin/python`).

Acceptance: a breakpoint set in `train_not_banana_model.py` halts
when F5 launches `Train: not-banana (ci profile)`.

Rejected alternative: spawn via `runtimeExecutable: python` and
forfeit step-debugging. Reason: defeats the point of an F5
contract.
